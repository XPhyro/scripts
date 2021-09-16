#!/usr/bin/env sh

logerrq() {
    printf "$@"
    exit 1
}

install() {
    mkdir /usr/local/bin/wrapper 2> /dev/null \
        && printf "There was no /usr/local/bin/wrapper directory, so it was created for you. Be sure to add it to your PATH with high priority.\n" >&2

    for i in bash el py sh; do
        cd "$i"
        find '.' -mindepth 1 -type f -executable -not -path "*/.archived/*" -not -path "*/wrapper/*" -printf "%P\0/usr/local/bin/%f\0" \
            | tee -a ../.installed \
            | xargs -r0 -n 2 cp -f --
        find '.' -mindepth 1 -type f -executable -not -path "*/.archived/*" -path "*/wrapper/*" -printf "%P\0/usr/local/bin/wrapper/%f\0" \
            | tee -a ../.installed \
            | xargs -r0 -n 2 cp -f --
        cd ..
    done

    cd c
    find '.' -mindepth 1 -type f -not -path "./.*" -printf "%P\n" | while IFS= read -r i; do
        out="${i%.c}"
        out="${out##*/}"
        gcc -O3 -Wall "$i" -o /usr/local/bin/"$out" &
        printf "\0%s\0" "$out" >> ../.installed
    done
}

uninstall() {
    # need argn from c/, it's okay since were in uninstall()
    xargs -r0 argn 1 -1 2 < .installed | xargs -r0 rm -f --
    rm -f .installed
}

set -e

[ "$(id -u)" != 0 ] && logerrq "This script needs to be executed as root.\n"

case "$1" in
    install) install;;
    uninstall) uninstall;;
    *) logerrq "Arguments must be 'install' or 'uninstall', not [%s]." "$1";;
esac
