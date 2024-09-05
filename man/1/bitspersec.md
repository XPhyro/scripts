% BITSPERSEC(1) bitspersec VERSION | User Commands Manual
changequote(`{{{', `}}}')%
% syscmd({{{date --date=@"$({ git log -1 --pretty="format:%ct%n" '}}}THIS{{{'; date +"%s"; } | head -n 1)" +"%Y-%m-%d %H:%M:%S"}}})
undefine({{{VERSION}}})
undefine({{{THIS}}})

# NAME

**bitspersec** — measure output speed

# SYNOPSIS

| **bitspersec** \[OPTION...\] \[FILE...\]

# DESCRIPTION

Measure output speed of anything that uses standard output.

# OPTIONS

## Generic Information

-h
: display help dialog and exit

## Output Control

-B
: output in bytes (default)

-b
: output in bits

## Input Control

-i NUM
: ignore the first NUM bytes (default is 0)

-n NUM
: calculate and output speed every NUM cycles (default is 10000)

# EXAMPLES

```
$ bitspersec -B -i 10000000 < /dev/urandom
dt=0.068544696 b=40960000 b/dt=5.9756629455326486e+08 || t=0.068544696 b=40960000 b/t=5.9756629455326486e+08
dt=0.068800424 b=40960000 b/dt=5.9534516822163773e+08 || t=0.137345120 b=81920000 b/t=5.9645366358848417e+08
dt=0.071120715 b=40960000 b/dt=5.7592221900468802e+08 || t=0.208465835 b=122880000 b/t=5.8944910565321171e+08
...
```

# AUTHOR

Berke Kocaoğlu <berke.kocaoglu@icloud.com>

# BUGS & ISSUES

Report at <https://github.com/XPhyro/scripts/issues> or
<https://codeberg.org/XPhyro/scripts/issues>, or mail the author.
GitHub is preferred.

# COPYRIGHT

include({{{LICENSE}}})
