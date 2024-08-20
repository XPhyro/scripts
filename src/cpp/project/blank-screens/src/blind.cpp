#include "blind.hpp"

#include <algorithm>

#include <X11/Xatom.h>

#include <xph/exec_info.hpp>

bs::blind::blind(std::shared_ptr<bs::cli> cli,
                 std::string_view monitor,
                 Display* display,
                 int default_screen,
                 Window root_window,
                 XRRCrtcInfo* crtc_info)
    : m_monitor(monitor)
    , m_cli(cli)
    , m_display(display)
{
    m_window = XCreateSimpleWindow(display,
                                   root_window,
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
    XSetClassHint(display, m_window, class_hint);

    XSetWindowAttributes window_attributes;
    window_attributes.override_redirect = True;

    const auto wm_state = XInternAtom(display, "_NET_WM_STATE", False);
    const auto wm_state_above = XInternAtom(display, "_NET_WM_STATE_ABOVE", False);
    const auto wm_window_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    const auto wm_window_type_dock = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DOCK", False);
    XChangeProperty(display,
                    m_window,
                    wm_state,
                    XA_ATOM,
                    32,
                    PropModeAppend,
                    reinterpret_cast<const unsigned char*>(&wm_state_above),
                    1);
    XChangeProperty(display,
                    m_window,
                    wm_window_type,
                    XA_ATOM,
                    32,
                    PropModeReplace,
                    reinterpret_cast<const unsigned char*>(&wm_window_type),
                    1);
    XChangeProperty(display,
                    m_window,
                    wm_window_type_dock,
                    XA_ATOM,
                    32,
                    PropModeReplace,
                    reinterpret_cast<const unsigned char*>(&wm_window_type_dock),
                    1);

    XChangeWindowAttributes(display, m_window, CWOverrideRedirect, &window_attributes);

    XMapWindow(display, m_window);
    XRaiseWindow(display, m_window);
    XFlush(display);

    set_alpha(cli->alpha());

    XSetWindowBackground(display, m_window, BlackPixel(display, default_screen));
    XClearWindow(display, m_window);
    XFlush(display);
}

bs::blind::~blind(void)
{
    XDestroyWindow(m_display, m_window);
    XFlush(m_display);
}

void bs::blind::set_alpha(double alpha)
{
    alpha = std::clamp(alpha, m_cli->min_alpha(), m_cli->max_alpha());

    unsigned long opacity;
    if (alpha > 1 - m_cli->snap_threshold())
        opacity = 0xFFFFFFFFul;
    else if (alpha < m_cli->snap_threshold())
        opacity = 0x00000000ul;
    else
        opacity = 0xFFFFFFFFul * alpha;

    const auto opacity_atom = XInternAtom(m_display, "_NET_WM_WINDOW_OPACITY", False);
    XChangeProperty(m_display,
                    m_window,
                    opacity_atom,
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char*>(&opacity),
                    1L);
    XFlush(m_display);
}
