#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xph/stdlib.h>
#include <xph/string.h>

int main(int argc, const char *argv[])
{
    int i;
    size_t len, seplen;
    const char *sep;
    char *s, *p;

    if (argc < 3)
        return EXIT_SUCCESS;

    sep = argv[1];
    seplen = strlen(sep);
    argc -= 2;
    argv += 2;

    for (i = 0, len = (argc - 1) * seplen + 1; i < argc; len += strlen(argv[i++])) {}
    s = amalloc(len * sizeof(char));
    p = s;

    for (i = 0; i < argc - 1; i++) {
        strcpy(p, argv[i]);
        p += strlen(argv[i]);
        strcpy(p, sep);
        p += seplen;
    }
    strcpy(p, argv[i]);
    s[len - 1] = '\0';

    puts(s);

    free(s);

    return EXIT_FAILURE;
}
