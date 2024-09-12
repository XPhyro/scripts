% AGPG(1) agpg VERSION | User Commands Manual
changequote(`{{{', `}}}')%
% syscmd({{{date --date=@"$({ git log -1 --pretty="format:%ct%n" '}}}THIS{{{'; date +"%s"; } | head -n 1)" +"%Y-%m-%d %H:%M:%S"}}})
undefine({{{VERSION}}})
undefine({{{THIS}}})

# NAME

**agpg** — encrypt or decrypt files using `gpg`

# SYNOPSIS

| **agpg** \[FILE...\]

# DESCRIPTION

Encrypt or decrypt each given file. Both encryption and decryption are done
individually. While encrypting, files are also compressed with BZip2 using a
compression level of 9. Cryptographic operations use `gpg` with the key
associated with the current local user account, not an email. Files' metadata
are not checked: a file is considered to be encrypted if its base name ends
with ".gpg". Filenames are obfuscated when encrypting, and restored when
decrypting.

# OPTIONS

No options accepted.

# AUTHOR

Berke Kocaoğlu <berke.kocaoglu@icloud.com>

# BUGS & ISSUES

Report at <https://github.com/XPhyro/scripts/issues> or
<https://codeberg.org/XPhyro/scripts/issues>, or mail the author.
GitHub is preferred.

# COPYRIGHT

include({{{LICENSE}}})

# SEE ALSO

gpg(1)
