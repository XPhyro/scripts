// @LDFLAGS -lX11 -lXrandr

#include <stdio.h>
#include <stdlib.h>

#include <xph/x11.h>

int main(int argc, char *argv[])
{
    const char *cursor_monitor;

    cursor_monitor = get_cursor_monitor(NULL);

    if (cursor_monitor) {
        puts(cursor_monitor);
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
