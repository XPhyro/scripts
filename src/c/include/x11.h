#ifndef HEADER_SCRIPTS_C_X11_
#define HEADER_SCRIPTS_C_X11_

#include <stdbool.h>
#include <stddef.h>
#include <sys/param.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

char *get_cursor_monitor(Display *display)
{
    bool display_given;
    Window root_window;
    char *monitor_name;
    int i, screen, root_x, root_y, win_x, win_y;
    unsigned int mask;
    Window root_return, child_return;
    XRRScreenResources *screen_resources;
    XRROutputInfo *output_info;
    XRRCrtcInfo *crtc_info;

    if (!(display_given = display)) {
        display = XOpenDisplay(NULL);
        if (!display)
            return NULL;
    }

    screen = DefaultScreen(display);
    root_window = RootWindow(display, screen);

    XQueryPointer(
        display, root_window, &root_return, &child_return, &root_x, &root_y, &win_x, &win_y, &mask);

    monitor_name = NULL;

    screen_resources = XRRGetScreenResources(display, root_window);
    if (!screen_resources)
        goto cleanup;

    for (i = 0; i < screen_resources->noutput; i++) {
        output_info = XRRGetOutputInfo(display, screen_resources, screen_resources->outputs[i]);
        if (!output_info || output_info->connection != RR_Connected || output_info->crtc == None) {
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
            goto cleanup;
        }

        XRRFreeCrtcInfo(crtc_info);
        XRRFreeOutputInfo(output_info);
    }

    XRRFreeScreenResources(screen_resources);

cleanup:
    if (!display_given)
        XCloseDisplay(display);

    return monitor_name;
}

#endif /* HEADER_SCRIPTS_C_X11_ */
