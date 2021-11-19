#!/usr/bin/env sh

logerrq() {
    printf "$@"
    exit 1
}

contains() {
    case "$1" in
        *"$2"*) return 0;;
    esac
    return 1
}

install() {
    mkdir "$prefix/wrapper" 2> /dev/null \
        && printf "%s %s\n" "There was no $prefix/wrapper directory, so it was created for you." \
                            "Be sure to add it to your PATH with high priority." >&2

    for i in bash el py sh; do
        cd "$i"
        find '.' -mindepth 1 -type f -executable -not -path "*/.archived/*" \
                                                 -not -path "*/wrapper/*" \
                                                 -printf "%P\0$prefix/%f\0" \
            | tee -a ../.installed \
            | xargs -r0 -n 2 cp -f --
        find '.' -mindepth 1 -type f -executable -not -path "*/.archived/*" \
                                                      -path "*/wrapper/*" \
                                                 -printf "%P\0$prefix/wrapper/%f\0" \
            | tee -a ../.installed \
            | xargs -r0 -n 2 cp -f --
        cd ..
    done

    cd c
    find '.' -mindepth 1 -type f -not -path "./.*" \
                                 -not -path "*/include/*" \
                                 -printf "%P\n" \
        | while IFS= read -r i; do
            out="${i%.c}"
            out="${out##*/}"
            contains "$i" "/wrapper/" && out="wrapper/$out"
            gcc -O3 -std=c99 -pedantic -Wall "$i" -o "$prefix/$out" &
            printf "\0%s\0" "$prefix/$out" >> ../.installed
        done
    cd ..

    cd cpp
    find '.' -mindepth 1 -type f -not -path "./.*" \
                                 -not -path "*/include/*" \
                                 -printf "%P\n" \
        | while IFS= read -r i; do
            out="${i%.cpp}"
            out="${out##*/}"
            contains "$i" "/wrapper/" && out="wrapper/$out"
            g++ -O3 -std=c++23 -Wall "$i" -o "$prefix/$out" &
            printf "\0%s\0" "$prefix/$out" >> ../.installed
        done
    cd ..
}

uninstall() {
    xargs -r0 -n 2 sh -c 'printf "%s\0" "$2"' -- < .installed | xargs -r0 rm -f --
    rm -f .installed
}

set -ex

if [ -n "$PREFIX" ]; then
    prefix="$PREFIX/bin"
elif [ "$(id -u)" != 0 ]; then
    prefix="$HOME/.local/bin"
else
    prefix="/usr/local/bin"
fi

[ ! -d "$prefix" ] && mkdir -p -- "$prefix"

case "$*" in
    install) install;;
    uninstall) uninstall;;
    *) logerrq "Arguments must be 'install' or 'uninstall', not [%s]." "$*";;
esac
