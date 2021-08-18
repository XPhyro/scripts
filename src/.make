#!/usr/bin/env sh

logerrq() {
    printf "$@"
    exit 1
}

install() {
    printf "%s\n" el py sh | while IFS= read -r i; do
        cd "$i"
        find '.' -mindepth 1 -type f -executable -not -path "./.archived/*" -printf "%P\n" | tee -a ../.installed | xargs -d '\n' -r install -t /usr/local/bin --
        cd ..
    done

    cd c
    find '.' -mindepth 1 -type f -not -path "./.*" -printf "%P\n" | while IFS= read -r i; do
        out="${i%.c}"
        out="${out##*/}"
        gcc -O3 -Wall "$i" -o /usr/local/bin/"$out" &
        printf "%s\n" "$out" >> ../.installed
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
