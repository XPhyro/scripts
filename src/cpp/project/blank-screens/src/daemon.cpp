#include "daemon.hpp"

#include <filesystem>
#include <fstream>
#include <ranges>
#include <thread>

#include <cstdlib>

#include <sys/stat.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include <xph/die.hpp>
#include <xph/math.hpp>

namespace fs = std::filesystem;

bs::daemon::daemon(const cli& cli) : m_cli(cli)
{
    m_display = XOpenDisplay(nullptr);
    xph::die_if(!m_display, "unable to open display");

    const auto& monitors = m_cli.monitors();
    for (const auto& monitor : monitors)
        add_monitor(monitor);

    m_last_alpha = 0.0;
    lerp_alpha(cli.alpha());
}

[[noreturn]] void bs::daemon::run()
{
    if (::mkfifo(m_cli.fifo_path().c_str(), 0600))
        xph::die("could not create fifo");

    for (;;) {
        std::ifstream fifo{ m_cli.fifo_path() };
        if (!fifo.is_open())
            xph::die("could not open fifo");

        for (std::string line; std::getline(fifo, line); this->dispatch(line)) {}
    }

    std::exit(EXIT_FAILURE);
}

static std::string get_cursor_monitor()
{
    Display* display = XOpenDisplay(nullptr);
    xph::die_if(!display, "unable to open display");

    int screen = DefaultScreen(display);
    Window root_window = RootWindow(display, screen);

    int root_x, root_y, win_x, win_y;
    unsigned int mask;
    Window root_return, child_return;

    XQueryPointer(
        display, root_window, &root_return, &child_return, &root_x, &root_y, &win_x, &win_y, &mask);

    XRRScreenResources* screen_resources = XRRGetScreenResources(display, root_window);
    xph::die_if(!screen_resources, "unable to get screen resources");

    for (int i = 0; i < screen_resources->noutput; ++i) {
        XRROutputInfo* output_info =
            XRRGetOutputInfo(display, screen_resources, screen_resources->outputs[i]);
        if (!output_info || output_info->connection != RR_Connected) {
            if (output_info)
                XRRFreeOutputInfo(output_info);
            continue;
        }

        XRRCrtcInfo* crtc_info = XRRGetCrtcInfo(display, screen_resources, output_info->crtc);
        if (!crtc_info) {
            XRRFreeOutputInfo(output_info);
            continue;
        }

        if (root_x >= crtc_info->x && root_x < static_cast<int>(crtc_info->x + crtc_info->width) &&
            root_y >= crtc_info->y && root_y < static_cast<int>(crtc_info->y + crtc_info->height)) {
            std::string monitor_name = output_info->name;
            XRRFreeCrtcInfo(crtc_info);
            XRRFreeOutputInfo(output_info);
            XRRFreeScreenResources(screen_resources);
            XCloseDisplay(display);
            return monitor_name;
        }

        XRRFreeCrtcInfo(crtc_info);
        XRRFreeOutputInfo(output_info);
    }

    XRRFreeScreenResources(screen_resources);
    XCloseDisplay(display);

    return {};
}

void bs::daemon::dispatch(const std::string& command_line)
{
    static std::vector<std::string> argv{};
    static std::istringstream iss;
    static std::string token;

    argv.clear();
    iss.clear();
    iss.str(command_line);

    for (iss.str(command_line); iss >> token; argv.push_back(token)) {}

    if (argv.empty())
        return;

    if (argv[0] == "alpha") {
        std::cerr << xph::exec_name << ": setting alpha to " << argv[1] << '\n';
        const auto alpha = argv.size() < 2 ? m_cli.alpha() : std::stod(argv[1]);
        lerp_alpha(alpha);
    } else if (argv[0] == "add") {
        for (const auto& monitor : argv | std::views::drop(1)) {
            const auto& target_monitor = monitor == "@cursor" ? get_cursor_monitor() : monitor;
            std::cerr << xph::exec_name << ": adding monitor " << target_monitor << '\n';
            add_monitor(target_monitor);
        }
    } else if (argv[0] == "remove") {
        for (const auto& monitor : argv | std::views::drop(1)) {
            const auto& target_monitor = monitor == "@cursor" ? get_cursor_monitor() : monitor;
            std::cerr << xph::exec_name << ": removing monitor " << target_monitor << '\n';
            remove_monitor(target_monitor);
        }
    } else if (argv[0] == "toggle") {
        for (const auto& monitor : argv | std::views::drop(1)) {
            const auto& target_monitor = monitor == "@cursor" ? get_cursor_monitor() : monitor;
            std::cerr << xph::exec_name << ": toggling monitor " << target_monitor << '\n';
            toggle_monitor(target_monitor);
        }
    } else if (argv[0] == "exit") {
        std::exit(EXIT_SUCCESS);
    } else {
        std::cerr << xph::exec_name << ": unknown command [" << argv[0] << "]\n";
    }
}

void bs::daemon::lerp_alpha(double alpha)
{
    auto current_alpha = m_last_alpha;

    while (!xph::approx_eq(current_alpha, alpha, km_epsilon)) {
        current_alpha =
            std::clamp(current_alpha + (alpha - current_alpha) * m_cli.lerp_factor(), 0.0, 1.0);

        for (auto& blind : m_blinds)
            blind.set_alpha(current_alpha);

        std::this_thread::sleep_for(m_cli.frame_time());
    }

    m_last_alpha = alpha;
}

void bs::daemon::add_monitor(const std::string& target_monitor)
{
    for (const auto& blind : m_blinds) {
        if (blind.m_monitor == target_monitor) {
            std::cerr << xph::exec_name << ": monitor " << target_monitor << " already exists\n";
            return;
        }
    }

    const auto default_screen = DefaultScreen(m_display);
    const auto root_window = RootWindow(m_display, default_screen);

    const auto screen_resources = XRRGetScreenResources(m_display, root_window);
    xph::die_if(!screen_resources, "unable to get screen resources");

    auto primary_output = XRRGetOutputPrimary(m_display, root_window);

    for (decltype(screen_resources->noutput) i = 0; i < screen_resources->noutput; ++i) {
        if (m_cli.ignore_primary() && screen_resources->outputs[i] == primary_output)
            continue;

        const auto output_info =
            XRRGetOutputInfo(m_display, screen_resources, screen_resources->outputs[i]);
        if (!output_info) {
            std::cerr << xph::exec_name << ": unable to get information for monitor " << i << '\n';
            continue;
        }

        if (output_info->connection)
            goto next;
        if (output_info->connection || output_info->name != target_monitor)
            goto next;

        XRRCrtcInfo* crtc_info;
        crtc_info = XRRGetCrtcInfo(m_display, screen_resources, output_info->crtc);
        if (!crtc_info) {
            std::cerr << xph::exec_name << ": unable to get information for monitor " << i << '\n';
            goto next;
        }

        m_blinds.emplace_back(std::make_shared<cli>(m_cli),
                              target_monitor,
                              m_display,
                              default_screen,
                              root_window,
                              crtc_info);

        XRRFreeCrtcInfo(crtc_info);
next:
        XRRFreeOutputInfo(output_info);
    }
    XRRFreeScreenResources(screen_resources);
}

void bs::daemon::remove_monitor(const std::string& target_monitor)
{
    for (auto it = m_blinds.begin(); it != m_blinds.end(); ++it) {
        if (it->m_monitor == target_monitor) {
            m_blinds.erase(it);
            break;
        }
    }
}

void bs::daemon::toggle_monitor(const std::string& target_monitor)
{
    for (auto it = m_blinds.begin(); it != m_blinds.end(); ++it) {
        if (it->m_monitor == target_monitor) {
            std::cerr << xph::exec_name << ": removing monitor " << target_monitor << '\n';
            m_blinds.erase(it);
            return;
        }
    }

    std::cerr << xph::exec_name << ": adding monitor " << target_monitor << '\n';
    add_monitor(target_monitor);
}

void bs::daemon::handle_signals([[maybe_unused]] int signal)
{
    fs::remove(m_cli.fifo_path());
    fs::remove_all(m_cli.lock_path());

    lerp_alpha(0.0);

    m_blinds.clear();

    XCloseDisplay(m_display);
}
