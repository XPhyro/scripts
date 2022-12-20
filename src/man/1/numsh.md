% NUMSH(1) numsh VERSION | User Commands Manual
changequote(`{{{', `}}}')%
% syscmd({{{date --date=@"$({ git log -1 --pretty="format:%ct%n" '}}}THIS{{{'; date +"%s"; } | head -n 1)" +"%Y-%m-%d %H:%M:%S"}}})
undefine({{{VERSION}}})
undefine({{{THIS}}})

# NAME

**numsh** — like NumPy, but for the shell

# SYNOPSIS

| **numsh** \[OPTION...\] \[NUMBER...\]

# DESCRIPTION

Do mathematical calculations on all given numbers.

With no NUMBER, read standard input. Empty lines are ignored when reading
standard input.

If FUNC requires additional arguments, option -o must be given respectively.

# OPTIONS

## Generic Information

-h
: display help dialog and exit

-L
: list all supported functions and exit

## Functions

-f FUNC
: function to pass numbers through

-o ARG
: pass additional arguments to FUNC. must be given after -f, can be given
multiple times.

# AUTHOR

Berke Kocaoğlu <berke.kocaoglu@metu.edu.tr>

# BUGS & ISSUES

Report at <https://github.com/XPhyro/scripts/issues> or
<https://codeberg.org/XPhyro/scripts/issues>.

# COPYRIGHT

include({{{LICENSE}}})

# SEE ALSO

**math.h(0)**, **bc(1)**, **NumPy** <https://github.com/numpy/numpy>
