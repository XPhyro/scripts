#define _POSIX_C_SOURCE 200809L

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

#include <hedley.h>
#include <strutil.h>

typedef enum {
    METHOD_MEAN,
    METHOD_COEFF,
    METHOD_SRGB,
    METHOD_RED,
    METHOD_GREEN,
    METHOD_BLUE,
    METHOD_HSV_H,
    METHOD_HSV_S,
    METHOD_HSV_V,
} method_t;
char *execname;

HEDLEY_NO_RETURN void invalidarg(void)
{
    fprintf(stderr, "Try '%s -h' for more information.\n", execname);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    ssize_t nread;
    enum { graybuflen = 1024, buflen = graybuflen * 3 };
    unsigned char graybuf[graybuflen + 1], buf[buflen];
    unsigned char *bufptr = buf;
    uint64_t bufdiff;
    double rf, gf, bf, h, s, v;
    uint8_t r8, g8, b8, M, m;
    int i, bufpool, grayidx;
    method_t method = METHOD_SRGB;

    execname = argv[0];

    while ((i = getopt(argc, argv, "hm:")) != -1) {
        switch (i) {
            case 'h':
                printf(
                    "Usage: %s [OPTION...]\n"
                    "Convert RGB24 to GRAY with the given method.\n"
                    "\n"
                    "  -m METHOD  method to use. can be one of: mean, coeff, srgb, red, green, blue, hsv-value. default is srgb.\n",
                    execname);
                exit(EXIT_SUCCESS);
                break;
            case 'm':
                if (strcaseeq(optarg, "mean"))
                    method = METHOD_MEAN;
                else if (strcaseeq(optarg, "weighted_sum"))
                    method = METHOD_COEFF;
                else if (strcaseeq(optarg, "srgb"))
                    method = METHOD_SRGB;
                else if (strcaseeq(optarg, "red"))
                    method = METHOD_RED;
                else if (strcaseeq(optarg, "green"))
                    method = METHOD_GREEN;
                else if (strcaseeq(optarg, "blue"))
                    method = METHOD_BLUE;
                else if (strcaseeq(optarg, "hue"))
                    method = METHOD_HSV_H;
                else if (strcaseeq(optarg, "saturation"))
                    method = METHOD_HSV_S;
                else if (strcaseeq(optarg, "value"))
                    method = METHOD_HSV_V;
                else {
                    fputs("invalid method given\n", stderr);
                    invalidarg();
                }
                break;
            default:
                invalidarg();
                break;
        }
    }

#ifndef __clang_analyzer__
    argv += optind;
    argc -= optind;
#endif /* ifndef __clang_analyzer__ */

    while ((nread = read(STDIN_FILENO, bufptr, buflen - (bufdiff = bufptr - buf))) > 0) {
        if ((bufpool = nread + bufdiff) < 3) {
            bufptr += nread;
            continue;
        }
        bufpool -= bufpool % 3;
        for (i = 0, grayidx = 0; i < bufpool; i += 3, grayidx++) {
            switch (method) {
                case METHOD_MEAN:
                    r8 = buf[i];
                    g8 = buf[i + 1];
                    b8 = buf[i + 2];
                    graybuf[grayidx] = (r8 + g8 + b8) / 3;
                    break;
                case METHOD_COEFF:
                    rf = buf[i] * 0.2126;
                    gf = buf[i + 1] * 0.7152;
                    bf = buf[i + 2] * 0.0722;
                    graybuf[grayidx] = rf + gf + bf;
                    break;
                case METHOD_SRGB:
                    rf = pow(buf[i] / 255.0, 2.2) * 0.2126;
                    gf = pow(buf[i + 1] / 255.0, 2.2) * 0.7152;
                    bf = pow(buf[i + 2] / 255.0, 2.2) * 0.0722;
                    graybuf[grayidx] = MIN(pow(rf + gf + bf, 0.454545) * 255, 255);
                    break;
                case METHOD_RED:
                    graybuf[grayidx] = buf[i];
                    break;
                case METHOD_GREEN:
                    graybuf[grayidx] = buf[i + 1];
                    break;
                case METHOD_BLUE:
                    graybuf[grayidx] = buf[i + 2];
                    break;
                case METHOD_HSV_H:
                    r8 = buf[i];
                    g8 = buf[i + 1];
                    b8 = buf[i + 2];

                    h = acos((r8 - g8 / 2.0 - b8 / 2.0) /
                             sqrt(r8 * r8 + g8 * g8 + b8 * b8 - r8 * g8 - r8 * b8 - g8 * b8));

                    graybuf[grayidx] = (uint8_t)(h);
                    break;
                case METHOD_HSV_S:
                    r8 = buf[i];
                    g8 = buf[i + 1];
                    b8 = buf[i + 2];

                    M = MAX(r8, MAX(g8, b8));
                    m = MIN(r8, MIN(g8, b8));

                    s = !M ? 0 : 1 - (double)m / (double)M;

                    graybuf[grayidx] = (uint8_t)(s);
                    break;
                case METHOD_HSV_V:
                    r8 = buf[i];
                    g8 = buf[i + 1];
                    b8 = buf[i + 2];

                    M = MAX(r8, MAX(g8, b8));

                    v = M / 255.0;

                    graybuf[grayidx] = (uint8_t)(v);
                    break;
                default:
                    fprintf(stderr, "%s: given method is not implemented\n", execname);
                    exit(EXIT_FAILURE);
                    break;
            }
        }
        write(STDOUT_FILENO, graybuf, grayidx);
    }

    return 0;
}
