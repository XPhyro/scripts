#!/usr/bin/env sh

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

[ "$(id -u)" != 0 ] && {
    printf "This script needs to be executed as root.\n"
    exit 1
}

case "$*" in
    install) install;;
    uninstall) uninstall;;
    *) exit 1;;
esac
