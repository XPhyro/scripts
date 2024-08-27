// @LDFLAGS -lX11 -lXrandr

#include <stddef.h>
#include <stdlib.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include <xph/die.h>
#include <xph/exec_info.h>

DEFINE_EXEC_INFO();

char *get_cursor_monitor(void)
{
    Display *display;
    Window root_window;
    char *monitor_name;
    int i, screen, root_x, root_y, win_x, win_y;
    unsigned int mask;
    Window root_return, child_return;
    XRRScreenResources *screen_resources;
    XRROutputInfo *output_info;
    XRRCrtcInfo *crtc_info;

    display = XOpenDisplay(NULL);
    screen = DefaultScreen(display);
    root_window = RootWindow(display, screen);

    XQueryPointer(
        display, root_window, &root_return, &child_return, &root_x, &root_y, &win_x, &win_y, &mask);

    screen_resources = XRRGetScreenResources(display, root_window);
    die_if(!screen_resources, "unable to get screen resources");

    for (i = 0; i < screen_resources->noutput; i++) {
        output_info = XRRGetOutputInfo(display, screen_resources, screen_resources->outputs[i]);
        if (!output_info || output_info->connection != RR_Connected) {
            if (output_info)
                XRRFreeOutputInfo(output_info);
            continue;
        }

        crtc_info = XRRGetCrtcInfo(display, screen_resources, output_info->crtc);
        if (!crtc_info) {
            XRRFreeOutputInfo(output_info);
            continue;
        }

        if (root_x >= crtc_info->x && root_x < (int)(crtc_info->x + crtc_info->width) &&
            root_y >= crtc_info->y && root_y < (int)(crtc_info->y + crtc_info->height)) {
            monitor_name = output_info->name;
            XRRFreeCrtcInfo(crtc_info);
            XRRFreeOutputInfo(output_info);
            XRRFreeScreenResources(screen_resources);
            return monitor_name;
        }

        XRRFreeCrtcInfo(crtc_info);
        XRRFreeOutputInfo(output_info);
    }

    XRRFreeScreenResources(screen_resources);

    return NULL;
}

int main(int argc, char *argv[])
{
    const char *cursor_monitor;

    init_exec_info(argc, argv);

    cursor_monitor = get_cursor_monitor();

    if (cursor_monitor) {
        puts(cursor_monitor);
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
