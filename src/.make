#!/usr/bin/env sh

if [ -t 1 ]; then
    C_RED='\033[0;31m'
    C_CLR='\033[0m'
    unbuffer="unbuffer"
else
    unset C_RED C_CLR unbuffer
fi

logerrq() {
    printf "$@"
    exit 1
}

install() {
    mkdir "$prefix/wrapper" 2> /dev/null \
        && printf "%s %s\n" "There was no $prefix/wrapper directory, so it was created for you." \
                            "Be sure to add it to your PATH with high priority." >&2

    for i in bash el py sh; do
        (
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
        )
    done

    (
        cd c
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
                    -Iinclude "$1" -lm -lmagic -o "$prefix/$out" \
                    && printf "\0%s\0" "$prefix/$out" >> ../.installed
            ' --
    )
        
    (
        cd cpp
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
    )
}

uninstall() {
    xargs -r0 -n 2 sh -c 'printf "%s\0" "$2"' -- < .installed | xargs -r0 rm -f --
    rm -f .installed
}

unittest() {
    (
        cd .tests

        tmpin="$(mktemp)"
        tmpout="$(mktemp)"
        tmperr="$(mktemp)"
        trap "rm -f -- '$tmpout' '$tmperr'" INT EXIT HUP TERM

        find '.' -mindepth 1 -maxdepth 1 -type d -printf "%P\n" | while IFS= read -r cmd; do
            find "$cmd" -mindepth 1 -maxdepth 1 -type d -printf "%P\n" | while IFS= read -r testname; do
                testdir="$cmd/$testname"

                "$testdir/in" > "$tmpin"
                eval "'$cmd' $("$testdir/args") > '$tmpout' 2> '$tmperr' < '$tmpin'"
                ec="$?"

                failstr=
                "$testdir/err" | cmp -s -- "$tmperr" || failstr="stderr is different. $failstr"
                "$testdir/out" | cmp -s -- "$tmpout" || failstr="stdout is different. $failstr"
                testec="$("$testdir/ec")"
                [ "$ec" -ne "$testec" ] && failstr="Expected exit code $testec, got $ec. $failstr"

                [ -n "$failstr" ] \
                    && printf "$C_RED%s: Test %s failed. %s$C_CLR\n" \
                        "$cmd" \
                        "$testdir" \
                        "$failstr"
            done
        done | sponge
    )
}

format() {
    find 'c' 'cpp' -type f \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) -print0 \
        | sort -z \
        | xargs -r0 clang-format -i --style=file --
}

analyse() {
    find 'bash' 'sh' -mindepth 1 -type f -executable \
        -not -path "*/.archived/*" -print0 \
        | xargs -r0 "$unbuffer" \
            shellcheck \
                -e SC1003 \
                -e SC1007 \
                -e SC2015 \
                -e SC2016 \
                -e SC2046 \
                -e SC2059 \
                -e SC2064 \
                -e SC2086 \
                -e SC2088 \
                -e SC2188 \
                -- \
        | less -RN
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
        ;;
    uninstall) uninstall;;
    test) unittest;;
    format) format;;
    analyse) analyse;;
    *) logerrq "Target must be 'install' or 'uninstall', not [%s]." "$1";;
esac
