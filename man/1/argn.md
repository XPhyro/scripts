% ARGN(1) argn VERSION | User Commands Manual
changequote(`{{{', `}}}')%
% syscmd({{{date --date=@"$({ git log -1 --pretty="format:%ct%n" '}}}THIS{{{'; date +"%s"; } | head -n 1)" +"%Y-%m-%d %H:%M:%S"}}})
undefine({{{VERSION}}})
undefine({{{THIS}}})

# NAME

**argn** — slice arguments

# SYNOPSIS

| **argn** \[OPTION...\] SLICE [ARGUMENT...]

# DESCRIPTION

Slice arguments using Python's slice syntax.

With no arguments, use standard input.

# OPTIONS

-h
: display help dialog and exit

-V
: display version and exit

# EXAMPLES

```
$ seq 7 | argn ::2
1
3
5
7

$ seq 7 | argn 1:3
2
3
```

# AUTHOR

Berke Kocaoğlu <berke.kocaoglu@icloud.com>

# BUGS & ISSUES

Report at <https://github.com/XPhyro/scripts/issues> or
<https://codeberg.org/XPhyro/scripts/issues>, or mail the author.
GitHub is preferred.

# COPYRIGHT

include({{{LICENSE}}})

# SEE ALSO

python(1)
