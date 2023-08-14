% SHUFR(1) shufr VERSION | User Commands Manual
changequote(`{{{', `}}}')%
% syscmd({{{date --date=@"$({ git log -1 --pretty="format:%ct%n" '}}}THIS{{{'; date +"%s"; } | head -n 1)" +"%Y-%m-%d %H:%M:%S"}}})
undefine({{{VERSION}}})
undefine({{{THIS}}})

# NAME

**shufr** — repeatedly generate random permutations

# SYNOPSIS

| **shufr** \[OPTION...\] \[FILE...\]
| **shufr** -e \[OPTION...\] \[ARG...\]
| **shufr** -i LO-HI \[OPTION...\]

# DESCRIPTION

Repeatedly write a random permutation of the input lines to standard
output.

With no FILE, or when FILE is `/dev/stdin`, read standard input.

# OPTIONS

## Generic

-h
: display help dialog and exit

## Permutation Generation Control

-U
: alias for `-u $input_line_count`. overrides `-u`.

-u COUNT
: make sure any sequence of COUNT lines have unique indices. if the input lines
are unique, the output lines will also be unique. COUNT must be less than or
equal to the count of lines provided via standard input.

## General Input Control

-e
: treat each ARG as an input line

-i LO-HI
: treat each number LO through HI as an input line. LO must be less than HI.
if HI is negative, it is interpreted as (18446744073709551615 - (|HI| - 1)).

## General Output Control

-n COUNT
: output at most COUNT lines

-z
: line delimiter is NUL, not newline

-0
: line delimiter is NUL, not newline

## Other

-r
: ignored. provided for partial compatibility with GNU shuf.

# Notes

This program is not suitable for cryptographic use.

# AUTHOR

Berke Kocaoğlu <kberke@metu.edu.tr>

# BUGS & ISSUES

Report at <https://github.com/XPhyro/scripts/issues> or
<https://codeberg.org/XPhyro/scripts/issues>.

# COPYRIGHT

include({{{LICENSE}}})

# SEE ALSO

shuf(1)
