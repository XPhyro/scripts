% CALCTDIFF(1) calctdiff VERSION | User Commands Manual
changequote(`{{{', `}}}')%
% syscmd({{{date --date=@"$({ git log -1 --pretty="format:%ct%n" '}}}THIS{{{'; date +"%s"; } | head -n 1)" +"%Y-%m-%d %H:%M:%S"}}})
undefine({{{VERSION}}})
undefine({{{THIS}}})

# NAME

**calctdiff** — calculate time differences

# SYNOPSIS

| **calctdiff** BEGIN_DATE END_DATE

# DESCRIPTION

Calculate the time difference between two dates. The output result is
`(END_DATE - BEGIN_DATE)`. The provided dates may use any syntax parseable by
`date(1)`. If END_DATE is not given, current date is used.

# OPTIONS

No options accepted.

# EXAMPLES

```
$ calctdiff 2020-10-11 2024-09-06
1426 days, 0 hours, 0 minutes, 0 seconds

$ calctdiff 2020-10-11
1426 days, 0 hours, 0 minutes, 0 seconds

$ calctdiff 2020-10-11 "@$(date +%s)"
1426 days, 1 hours, 22 minutes, 1 seconds

$ calctdiff 2020-10-11 today
1426 days, 1 hours, 22 minutes, 1 seconds

$ calctdiff 2020-10-11 '1 hour ago'
1426 days, 0 hours, 22 minutes, 1 seconds
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

date(1)
