#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <linux/usbdevice_fs.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <xph/exec_info.h>
#include <xph/stdlib.h>

DEFINE_EXEC_INFO()

int ctl_reset(const char *dev)
{
    int fd, ret = EXIT_SUCCESS;

    if ((fd = open(dev, O_WRONLY)) < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (ioctl(fd, USBDEVFS_RESET, 0) < 0) {
        perror("ioctl");
        ret = EXIT_FAILURE;
    }

    close(fd);

    return ret;
}

void print_usage(void)
{
    printf("Usage: %s [OPTION...] BUS DEVICE\n"
           "       %s [OPTION...] FILE\n"
           "Control a USB device.\n"
           "\n"
           "  -h        display this help and exit\n"
           "  -r        reset the device\n",
           execname,
           execname);
    exit(EXIT_SUCCESS);
}

void print_help(void)
{
    fprintf(stderr, "Try '%s -h' for more information.\n", execname);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int i, ret = EXIT_SUCCESS;
    unsigned int optbus, optdev;
    bool optreset = false;
    const char *fl;
    char flbuf[21];

    init_exec_info(argc, argv);

    while ((i = getopt(argc, argv, "hr")) != -1) {
        switch (i) {
            case 'h':
                print_usage();
                break;
            case 'r':
                optreset = true;
                break;
            default:
                print_help();
                break;
        }
    }

    argc -= optind;
    argv += optind;

    switch (argc) {
        case 1:
            fl = argv[0];
            break;
        case 2:
            optbus = astrtol(argv[0], "invalid bus number");
            optdev = astrtol(argv[1], "invalid device number");
            snprintf(flbuf, sizeof(flbuf), "/dev/bus/usb/%.3d/%.3d", optbus, optdev);
            fl = flbuf;
            break;
        default:
            print_help();
            break;
    }

    if (optreset)
        ret |= ctl_reset(fl);

    return ret;
}
