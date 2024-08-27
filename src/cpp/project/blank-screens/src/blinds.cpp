#include "blinds.hpp"

#include <algorithm>
#include <cassert>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <ctype.h>

#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

#include <xph/die.hpp>
#include <xph/math.hpp>

#include <xph/x11.h>

// // PUBLIC // //

bs::blinds::blinds(const bs::cli& cli)
    : m_cli(cli)
    , m_display(XOpenDisplay(nullptr))
    , m_screen(DefaultScreen(m_display))
    , m_root_window(RootWindow(m_display, m_screen))
{
    xph::die_if(!m_display, "unable to open display");

    const auto& monitors = m_cli.monitors();
    for (const auto& monitor : monitors)
        this->add_monitor(monitor);
}

bs::blinds::~blinds(void)
{
    // TODO: create xph::assert for DEBUG
    xph::die_if(!m_display, "we shouldn't be able to reach here if m_display is null");

    destroy_windows();

    XCloseDisplay(m_display);
}

bool bs::blinds::add_monitor(const std::string& monitor_expr, bool commit_changes)
{
    const auto& optional_monitor = eval_monitor_expr(monitor_expr);
    if (!optional_monitor)
        return false;

    const auto& monitor = *optional_monitor;

    if (std::all_of(monitor.begin(), monitor.end(), ::isspace))
        return false;

    if (std::find(m_monitors.begin(), m_monitors.end(), monitor) != m_monitors.end())
        return false;

    m_monitors.push_back(monitor);
    m_alphas.push_back(0.0);
    if (commit_changes)
        update_windows();

    return true;
}

bool bs::blinds::remove_monitor(const std::string& monitor_expr, bool commit_changes)
{
    if (monitor_expr.starts_with(k_expr_invert_prefix)) {
        std::string_view sv = monitor_expr;
        sv = sv.substr(k_expr_invert_prefix.size());

        bool ret = false;

        for (auto it = m_monitors.end() - 1; it > m_monitors.begin(); --it) {
            if (*it == sv)
                continue;

            ret = true;
            remove_monitor(*it, false);
        }

        if (commit_changes)
            update_windows();

        return ret;
    }

    const auto& optional_monitor = eval_monitor_expr(monitor_expr);
    if (!optional_monitor)
        return false;

    const auto& monitor = *optional_monitor;

    auto monitor_it = std::find(m_monitors.begin(), m_monitors.end(), monitor);
    if (monitor_it == m_monitors.end())
        return false;

    lerp_alpha(0.0, monitor);

    m_alphas.erase(m_alphas.begin() + (monitor_it - m_monitors.begin()));
    m_monitors.erase(monitor_it);
    if (commit_changes)
        update_windows();

    return true;
}

void bs::blinds::toggle_monitor(const std::string& monitor_expr, bool commit_changes)
{
    if (!add_monitor(monitor_expr, commit_changes))
        remove_monitor(monitor_expr, commit_changes);
}

void bs::blinds::clear_monitors(bool commit_changes)
{
    if (m_monitors.empty())
        return;

    lerp_alpha(0.0, std::nullopt);

    for (auto it = m_monitors.end() - 1; it >= m_monitors.begin(); --it)
        remove_monitor(*it, false);

    if (commit_changes)
        update_windows();
}

void bs::blinds::commit_monitor_changes(void)
{
    update_windows();

    std::cerr << "\nmonitors:";
    for (const auto& monitor : m_monitors)
        std::cerr << ' ' << monitor;
    std::cerr << "\nwindows:";
    for (const auto& window : m_windows)
        std::cerr << ' ' << window;
    std::cerr << "\nalphas:";
    for (const auto& alpha : m_alphas)
        std::cerr << ' ' << alpha;
    std::cerr << '\n';
}

void bs::blinds::lerp_alpha(double alpha, std::optional<std::string> monitor_expr)
{
    const constexpr double epsilon = 0.0001;

    static std::vector<std::size_t> m_lerp_idx;

    if (monitor_expr) {
        const auto& optional_monitor = eval_monitor_expr(*monitor_expr);
        if (!optional_monitor)
            return;

        const auto& monitor = *optional_monitor;

        const auto& idx = std::find(m_monitors.begin(), m_monitors.end(), monitor);

        if (idx == m_monitors.end())
            return;

        m_lerp_idx.push_back(std::distance(m_monitors.begin(), idx));
    } else {
        m_lerp_idx.reserve(m_windows.size());
        for (std::size_t i = 0; i < m_windows.size(); ++i)
            m_lerp_idx.push_back(i);
    }

    while (m_lerp_idx.size()) {
        for (auto it = m_lerp_idx.end() - 1; it >= m_lerp_idx.begin(); --it) {
            const auto& window = m_windows[*it];
            const auto& last_alpha = m_alphas[*it];

            if (xph::approx_eq(last_alpha, alpha, epsilon)) {
                m_lerp_idx.erase(it);
                continue;
            }

            const auto& new_alpha = last_alpha + (alpha - last_alpha) * m_cli.lerp_factor();
            m_alphas[*it] = set_window_alpha(window, new_alpha);
        }

        XFlush(m_display);

        std::this_thread::sleep_for(m_cli.frame_time());
    }
}

// // PRIVATE // //

std::optional<std::string> bs::blinds::eval_monitor_expr(const std::string& monitor_expr)
{
    std::string_view sv = monitor_expr;

    if (sv.starts_with(k_expr_special_prefix)) {
        const auto& special = sv.substr(k_expr_special_prefix.size());
        if (special == k_expr_cursor)
            return { get_cursor_monitor(m_display) };
        return {};
    }

    return { monitor_expr };
}

void bs::blinds::update_windows(void)
{
    destroy_windows();
    create_windows();
}

void bs::blinds::create_windows(void)
{
    m_windows.reserve(m_monitors.size());
    while (m_windows.size() < m_monitors.size())
        m_windows.push_back(-1);

    const auto screen_resources = XRRGetScreenResources(m_display, m_root_window);
    xph::die_if(!screen_resources, "unable to get screen resources");

    auto primary_output = XRRGetOutputPrimary(m_display, m_root_window);

    for (decltype(screen_resources->noutput) i = 0; i < screen_resources->noutput; ++i) {
        if (m_cli.ignore_primary() && screen_resources->outputs[i] == primary_output)
            continue;

        const auto output_info =
            XRRGetOutputInfo(m_display, screen_resources, screen_resources->outputs[i]);
        if (!output_info) {
            std::cerr << xph::exec_name << ": unable to get information for monitor " << i << '\n';
            continue;
        }

        decltype(m_monitors.begin()) idx;

        if (output_info->connection)
            goto next;

        idx = std::find(m_monitors.begin(), m_monitors.end(), output_info->name);
        if (idx == m_monitors.end())
            goto next;

        XRRCrtcInfo* crtc_info;
        crtc_info = XRRGetCrtcInfo(m_display, screen_resources, output_info->crtc);
        if (!crtc_info) {
            std::cerr << xph::exec_name << ": unable to get information for monitor " << i << '\n';
            goto next;
        }

        {
            const auto window = XCreateSimpleWindow(m_display,
                                                    m_root_window,
                                                    crtc_info->x,
                                                    crtc_info->y,
                                                    crtc_info->width,
                                                    crtc_info->height,
                                                    0,
                                                    0,
                                                    0);

            auto* class_hint = XAllocClassHint();
            static std::string class_name(xph::exec_name);
            class_hint->res_name = class_name.data();
            class_hint->res_class = class_name.data();
            XSetClassHint(m_display, window, class_hint);

            m_windows[std::distance(m_monitors.begin(), idx)] = window;
        }

        XRRFreeCrtcInfo(crtc_info);
next:
        XRRFreeOutputInfo(output_info);
    }
    XRRFreeScreenResources(screen_resources);

    auto begin = m_windows.begin();
    auto end = m_windows.end();
    for (auto it = begin; it != end; ++it) {
        XSetWindowAttributes window_attributes;
        window_attributes.override_redirect = True;

        const auto wm_state = XInternAtom(m_display, "_NET_WM_STATE", False);
        const auto wm_state_above = XInternAtom(m_display, "_NET_WM_STATE_ABOVE", False);
        const auto wm_window_type = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE", False);
        const auto wm_window_type_dock = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE_DOCK", False);
        XChangeProperty(m_display,
                        *it,
                        wm_state,
                        XA_ATOM,
                        32,
                        PropModeAppend,
                        reinterpret_cast<const unsigned char*>(&wm_state_above),
                        1);
        XChangeProperty(m_display,
                        *it,
                        wm_window_type,
                        XA_ATOM,
                        32,
                        PropModeReplace,
                        reinterpret_cast<const unsigned char*>(&wm_window_type),
                        1);
        XChangeProperty(m_display,
                        *it,
                        wm_window_type_dock,
                        XA_ATOM,
                        32,
                        PropModeReplace,
                        reinterpret_cast<const unsigned char*>(&wm_window_type_dock),
                        1);

        XChangeWindowAttributes(m_display, *it, CWOverrideRedirect, &window_attributes);

        XMapWindow(m_display, *it);
        XRaiseWindow(m_display, *it);
        XFlush(m_display);

        m_alphas[it - begin] = set_window_alpha(*it, m_alphas[it - begin]);
        XSetWindowBackground(m_display, *it, BlackPixel(m_display, m_screen));
        XClearWindow(m_display, *it);
        XFlush(m_display);
    }

    lerp_alpha(m_cli.alpha());
}

void bs::blinds::destroy_windows(void)
{
    for (auto& window : m_windows)
        XDestroyWindow(m_display, window);
    m_windows.clear();
}

double bs::blinds::set_window_alpha(Window window, double alpha)
{
    alpha = std::clamp(alpha, m_cli.min_alpha(), m_cli.max_alpha());

    unsigned long opacity;
    if (alpha > 1 - m_cli.snap_threshold())
        opacity = 0xFFFFFFFFul;
    else if (alpha < m_cli.snap_threshold())
        opacity = 0x00000000ul;
    else
        opacity = 0xFFFFFFFFul * alpha;

    const auto opacity_atom = XInternAtom(m_display, "_NET_WM_WINDOW_OPACITY", False);
    XChangeProperty(m_display,
                    window,
                    opacity_atom,
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char*>(&opacity),
                    1L);

    return alpha;
}
