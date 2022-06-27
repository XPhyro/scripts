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
            '"$csa"' gcc -O'"${o:-3}"' '"$g"' -std=c99 -pedantic \
                -Wall -Wextra -Werror -Wabi=11 \
                -Wno-unused-parameter -Wno-unused-result \
                -Wno-implicit-fallthrough -Wno-sign-compare \
                -Wfloat-equal -Wdouble-promotion -Wjump-misses-init \
                -Wold-style-definition -Winline -Wpadded -Wpacked -Wdisabled-optimization \
                -Iinclude -lm -lmagic "$1" -o "$prefix/$out" \
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
            g++ -O'"${o:-3}"' '"$g"' -std=c++23 \
                -Wall -Wextra -Werror -Wabi=11 \
                -Wno-unused-parameter -Wno-unused-result \
                -Wno-implicit-fallthrough -Wno-sign-compare \
                -Wfloat-equal -Wdouble-promotion -Wdisabled-optimization \
                -Iinclude "$1" -o "$prefix/$out" \
                && printf "\0%s\0" "$prefix/$out" >> ../.installed
        ' --

    cd ..
}

uninstall() {
    xargs -r0 -n 2 sh -c 'printf "%s\0" "$2"' -- < .installed | xargs -r0 rm -f --
    rm -f .installed
}

unittest() {
    isroot && return 0

    cd .tests

    tmpout="$(mktemp)"
    tmperr="$(mktemp)"
    trap "rm -f -- '$tmpout' '$tmperr'" INT EXIT HUP TERM

    find '.' -mindepth 1 -maxdepth 1 -type d -printf "%P\n" | while IFS= read -r cmd; do
        find "$cmd" -mindepth 1 -maxdepth 1 -type d -printf "%P\n" | while IFS= read -r testname; do
            testdir="$cmd/$testname"
            args="$(cat "$testdir/args")"
            eval "'$cmd' $args > '$tmpout' 2> '$tmperr' < '$testdir/in'"
            ec="$?"

            failstr=
            cmp -s -- "$testdir/err" "$tmperr" || failstr="stderr is different. $failstr"
            cmp -s -- "$testdir/out" "$tmpout" || failstr="stdout is different. $failstr"
            testec="$(cat "$testdir/ec")"
            [ "$ec" -ne "$testec" ] && failstr="Expected exit code $testec, got $ec. $failstr"

            [ -n "$failstr" ] \
                && printf "\033[0;31m%s: Test %s failed. %s\033[0m\n" \
                    "$cmd" \
                    "$testdir" \
                    "$failstr"
        done
    done | sponge

    cd ..
}

format() {
    find 'c' 'cpp' -type f \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) -print0 \
        | sort -z \
        | xargs -r0 clang-format -i --style=file --
}

set -ex

if [ "$(id -u)" -eq 0 ]; then
    isroot() {
        true
    }
else
    isroot() {
        false
    }
fi

if [ -n "$PREFIX" ]; then
    prefix="$PREFIX/bin"
elif isroot; then
    prefix="/usr/local/bin"
else
    prefix="$HOME/.local/bin"
fi

export prefix

mkdir -p -- "$prefix"
[ -d "$prefix" ]

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
                        false|0|"") unset csa;;
                        *) exit 1;;
                    esac
                    ;;
                o=*)
                    val="${i#o=}"
                    case "$val" in
                        ""|0|1|2|3|g|s|fast) o="$val";;
                        *) exit 1;;
                    esac
                    ;;
                g=*)
                    val="${i#g=}"
                    case "$val" in
                        "") unset g;;
                        g) g="-g";;
                        gdb) g="-ggdb";;
                        *) exit 1;;
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
    format) format;;
    *) logerrq "Target must be 'install' or 'uninstall', not [%s]." "$1";;
esac
