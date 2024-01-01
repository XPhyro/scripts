// @LDFLAGS -lX11 -lXrandr

// C++
#include <cstdlib>
#include <exec_info.hpp>
#include <future>
#include <iostream>
#include <vector>

// C
#include <unistd.h>

// C++ libraries
#include <die.hpp>
#include <sysutil.hpp>

// third-party
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

static Display* display;
static std::vector<Window> windows;

DEFINE_EXEC_INFO();

void handle_signals([[maybe_unused]] int sig)
{
    for (auto& window : windows)
        XDestroyWindow(display, window);
    XCloseDisplay(display);
    std::exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    xph::gather_exec_info(argc, argv);

    xph::die_if(!(display = XOpenDisplay(NULL)), "unable to open display");
    int default_screen = DefaultScreen(display);

    Window root_window = RootWindow(display, default_screen);

    XRRScreenResources* screen_resources = XRRGetScreenResources(display, root_window);
    xph::die_if(!screen_resources, "unable to get screen resources");

    RROutput primary_output = XRRGetOutputPrimary(display, root_window);

    for (decltype(screen_resources->noutput) i = 0; i < screen_resources->noutput; ++i) {
        if (screen_resources->outputs[i] == primary_output)
            continue;

        XRROutputInfo* output_info =
            XRRGetOutputInfo(display, screen_resources, screen_resources->outputs[i]);
        if (!output_info) {
            std::cerr << xph::exec_name << ": unable to get information for monitor " << i << '\n';
            continue;
        }

        if (output_info->connection)
            continue;

        XRRCrtcInfo* crtc_info = XRRGetCrtcInfo(display, screen_resources, output_info->crtc);
        if (!crtc_info) {
            std::cerr << xph::exec_name << ": unable to get information for monitor " << i << '\n';
            continue;
        }

        windows.push_back(XCreateSimpleWindow(display,
                                              root_window,
                                              crtc_info->x,
                                              crtc_info->y,
                                              crtc_info->width,
                                              crtc_info->height,
                                              0,
                                              0,
                                              0));

        XRRFreeOutputInfo(output_info);
        XRRFreeCrtcInfo(crtc_info);
    }
    XRRFreeScreenResources(screen_resources);

    for (auto& window : windows) {
        XSetWindowAttributes attr;
        attr.override_redirect = True;
        XChangeWindowAttributes(display, window, CWOverrideRedirect, &attr);

        XMapWindow(display, window);
        XFlush(display);

        XSetWindowBackground(display, window, BlackPixel(display, default_screen));
        XClearWindow(display, window);
        XFlush(display);
    }

    xph::sys::signals<4>({ SIGINT, SIGTERM, SIGQUIT, SIGHUP }, handle_signals);
    std::promise<void>().get_future().wait();

    return 0;
}
