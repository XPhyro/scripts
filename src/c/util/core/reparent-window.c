/* @LDFLAGS -lX11
 */

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>

#include <xph/die.h>
#include <xph/exec_info.h>
#include <xph/stdlib.h>

DEFINE_EXEC_INFO();

int main(int argc, char *argv[])
{
    int i;

    init_exec_info(argc, argv);

    if (argc < 3) {
        printf("Usage: %s PARENT_ID CHILD_ID [CHILD_ID...]\n", execname);
        return EXIT_FAILURE;
    }

    Display *display = XOpenDisplay(NULL);
    die_if(!display, "could not open display");

    Window parent_window = astrtoul(argv[1], "parent window ID is invalid");
    for (i = 2; i < argc; i++) {
        Window child_window = astrtoul(argv[i], "child window ID is invalid");
        XReparentWindow(display, child_window, parent_window, 0, 0);
    }

    XFlush(display);
    XCloseDisplay(display);

    return EXIT_SUCCESS;
}
