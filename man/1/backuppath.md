% BACKUPPATH(1) backuppath VERSION | User Commands Manual
changequote(`{{{', `}}}')%
% syscmd({{{date --date=@"$({ git log -1 --pretty="format:%ct%n" '}}}THIS{{{'; date +"%s"; } | head -n 1)" +"%Y-%m-%d %H:%M:%S"}}})
undefine({{{VERSION}}})
undefine({{{THIS}}})

# NAME

**backuppath** — generate backup paths for files

# SYNOPSIS

| **backuppath** \[OPTION...\] \[FILE...\]

# DESCRIPTION

Generate backup paths for files. Generated paths are compliant with the
`--backup` syntax used by GNU utilities.

# OPTIONS

## Generic Information

-h
: display help dialog and exit

-V
: display version and exit

## I/O Control

-0
: use NUL to separate the output

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
