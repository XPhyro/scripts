#!/usr/bin/env sh

logerrq() {
    printf "$@"
    exit 1
}

install() {
    mkdir "$prefix/wrapper" 2> /dev/null \
        && printf "%s %s\n" "There was no $prefix/wrapper directory, so it was created for you." \
                            "Be sure to add it to your PATH with high priority." >&2

    cd sh # dummy cd to reduce number of cds in the loop
    for i in bash el py sh; do
        cd "../$i"
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
    done

    cd ../c

    find '.' -mindepth 1 -type f -not -path "./.*" \
                                 -not -path "*/include/*" \
                                 -printf "%P\0" \
        | xargs -r0 -n 1 -P "$(nproc)" sh -c '
            set -e
            out="${1%.c}"
            out="${out##*/}"
            case "$1" in
                */wrapper/*) out="wrapper/$out";;
            esac
            '"$csa"' gcc -O'"${o:-3}"' -std=c99 -pedantic -Iinclude -lm -Wall "$1" -o "$prefix/$out" \
                && printf "\0%s\0" "$prefix/$out" >> ../.installed
        ' --
        
    cd ../cpp

    find '.' -mindepth 1 -type f -not -path "./.*" \
                                 -not -path "*/include/*" \
                                 -printf "%P\0" \
        | xargs -r0 -n 1 -P "$(nproc)" sh -c '
            set -e
            out="${1%.cpp}"
            out="${out##*/}"
            case "$1" in
                */wrapper/*) out="wrapper/$out";;
            esac
            g++ -O'"${o:-3}"' -std=c++23 -Iinclude -Wall "$1" -o "$prefix/$out" \
                && printf "\0%s\0" "$prefix/$out" >> ../.installed
        ' --

    cd ..
}

uninstall() {
    xargs -r0 -n 2 sh -c 'printf "%s\0" "$2"' -- < .installed | xargs -r0 rm -f --
    rm -f .installed
}

unittest() {
    cd .tests
    find '.' -mindepth 1 -type f -printf "%P\0" | parallel --bar -kr0 -I FILE -P 0 'sh -c '\''
        tmp="$(mktemp)"
        trap "rm -f \"$tmp\"" INT EXIT TERM
        eval "$1" > "$tmp"
        if cmp -s "$tmp" "$1"; then
            printf "SUCCESS: %s\n" "$1"
            exit 0
        else
            printf "\033[0;31mFAIL:\033[0m %s\n" "$1"
            exit 1
        fi
    '\'' -- FILE' || printf "\033[0;31mAt least one test failed, check above results.\033[0m\n"
}

set -ex

if [ -n "$PREFIX" ]; then
    prefix="$PREFIX/bin"
elif [ "$(id -u)" != 0 ]; then
    prefix="$HOME/.local/bin"
else
    prefix="/usr/local/bin"
fi

export prefix

[ ! -d "$prefix" ] && mkdir -p -- "$prefix"

unset csa o
case "$1" in
    install)
        shift
        for i; do
            case "$i" in
                csa=*)
                    val="${i#csa=}"
                    case "$val" in
                        true|1) csa="scan-build";;
                        *) unset csa;;
                    esac
                    ;;
                o=*)
                    val="${i#o=}"
                    case "$val" in
                        ""|0|1|2|3|g|s|fast) o="$val";;
                        *) unset o;;
                    esac
                    ;;
                *) logerrq "Unrecognised argument [%s]." "$i";;
            esac
        done

        install
        unittest
        ;;
    uninstall) uninstall;;
    test) unittest;;
    *) logerrq "Target must be 'install' or 'uninstall', not [%s]." "$1";;
esac
