#!/usr/bin/env gcc-otg

// @CXXFLAGS -std=c99 -Wall -Werror -pedantic
// @LDFLAGS

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    const char *execname = argv[0];

    argc--;
    argv++;

    while (argc) {
        const char *filename = *argv;
        FILE *stream;
        if (!(stream = fopen(filename, "r"))) {
            if (errno == ENOENT)
                fprintf(stderr, "%s: %s: No such file or directory\n", execname, filename);
            else {
                fprintf(stderr, "%s: %s: Could not open file: ", execname, filename);
                perror("fopen");
            }

            return EXIT_FAILURE;
        }

        char *line = NULL;
        size_t line_size = 0;
        ssize_t line_len;
        while ((line_len = getline(&line, &line_size, stream)) != -1) {
            if (line[line_len - 1] == '\n') {
                line[line_len - 1] = '\0';
                line_len--;
            }
            for (size_t i = 0; i < line_len; i++) {
                while (i < line_len) {
                    if (line[i] == ' ')
                        i++;
                    else
                        break;
                }
                if (line[i] == '#') {
                    goto next_line;
                }
                if (line[i] == '.') {
                    for (i++; i < line_len;) {
                        if (line[i] == ' ')
                            i++;
                        else
                            break;
                    }
                    if (strcmp(line + i, "shellverbose.sh"))
                        printf("File %s does not have shellverbose.sh at the top\n", filename);
                } else {
                    printf("File %s does not have shellverbose.sh at the top\n", filename);
                }
                goto next_file;
            }

next_line:;
        }
next_file:;

        fclose(stream);
        if (line)
            free(line);

        argv++;
        argc--;
    }

    return EXIT_SUCCESS;
}
