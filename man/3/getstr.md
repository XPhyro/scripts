% GETSTR(3) getstr VERSION | Library Functions Manual
changequote(`{{{', `}}}')%
% syscmd({{{date --date=@"$({ git log -1 --pretty="format:%ct%n" '}}}THIS{{{'; date +"%s"; } | head -n 1)" +"%Y-%m-%d %H:%M:%S"}}})
undefine({{{VERSION}}})
undefine({{{THIS}}})

# NAME

**fgetstr**, **getstr** — delimited string input

# SYNOPSIS

```c
#include <ioutil.h>

char *fgetstr(FILE *stream, int delim);
char *getstr(int argc, char *argv[], int delim);
```

Feature Test Macro Requirements

```c
fgetstr(), getstr():
    Since glibc 2.10:
        _POSIX_C_SOURCE >= 200809L
    Before glibc 2.10:
        _GNU_SOURCE
```

# DESCRIPTION

`fgetstr()` is an easy-to-use wrapper for `getdelim(3)`.

`getstr()` is similar to `fgetstr()`. If `argc` is not 0, it gathers input from
`argv`. Otherwise, it gathers input using `fgetstr()` on `stdin`.

# RETURN VALUE

On success, `fgetstr()` and `getstr()` return a line of string.

Both functions return `NULL` on failure.

# ERRORS

Not used.

# CONFORMING TO

Conforms to nothing. Requires a POSIX.1-2008-compatible compiler.

# NOTES

`fgetstr()` and `getstr()` should not be used together on `stdin`.

# EXAMPLES

```c
/* fgetstr() */

#include <stdio.h>

#include <ioutil.h>

void processline(char *line)
{
    puts(line);
}

int main(int argc, char *argv[])
{
    char *line;
    while ((line = fgetstr(stdin, delim)))
        processline(line);
    free(line);
}


/* getstr() */

#include <stdio.h>

#include <ioutil.h>

void processline(char *line)
{
    puts(line);
}

int main(int argc, char *argv[])
{
    char *line;
    while ((line = getstr(argc, argv, delim)))
        processline(line);
    free(line);
}
```

# AUTHOR

Berke Kocaoğlu <berke.kocaoglu@metu.edu.tr>

# BUGS & ISSUES

Report at <https://github.com/XPhyro/scripts/issues> or
<https://codeberg.org/XPhyro/scripts/issues>.

# COPYRIGHT

include({{{LICENSE}}})

# SEE ALSO

**getdelim(3)**
