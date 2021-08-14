#!/usr/bin/env sh

logerrq() {
    printf "$@"
    exit 1
}

install() {
    printf "%s\n" sh py | while IFS= read -r i; do
        cd "$i"
        find '.' -mindepth 1 -type f -executable -not -path "./.archived/*" | tee -a ../.installed | xargs -d '\n' -r install -t /usr/local/bin --
        cd ..
    done

    cd c
    find '.' -mindepth 1 -maxdepth 1 -type d -not -path "./.*" -printf "%P\n" | tee -a ../.installed | while IFS= read -r i; do
        cd "$i"
        gcc -O3 -Wall main.c -o "$i"
        mv -f "$i" /usr/local/bin/"$i"
        cd ..
    done
}

uninstall() {
    cat .installed | sort -u | xargs -d '\n' -rI '{}' rm -f '/usr/local/bin/{}'
    rm -f .installed
}

set -e

[ "$(id -u)" != 0 ] && logerrq "This script needs to be executed as root.\n"

case "$*" in
    install) install;;
    uninstall) uninstall;;
    *) logerrq "Arguments must be 'install' or 'uninstall', not [%s]." "$*";;
esac
