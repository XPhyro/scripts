#!/usr/bin/env sh

logerrq() {
    printf "$@"
    exit 1
}

install() {
    mkdir -p -- "$binprefix" "$manprefix"
    mkdir "$binprefix/wrapper" 2> /dev/null \
        && printf "$C_RED%s %s$C_CLR\n" \
            "There was no $binprefix/wrapper directory, so it was created for you." \
            "Be sure to add it to your PATH with high priority."

    for i in bash el py sh; do
        (
            cd "$i"
            find '.' -mindepth 1 -type f -executable -not -path "*/.archived/*" \
                                                     -not -path "*/wrapper/*" \
                                                     -printf "%P\0$binprefix/%f\0" \
                | tee -a ../.installed \
                | xargs -r0 -n 2 cp -f --
            find '.' -mindepth 1 -type f -executable -not -path "*/.archived/*" \
                                                          -path "*/wrapper/*" \
                                                     -printf "%P\0$binprefix/wrapper/%f\0" \
                | tee -a ../.installed \
                | xargs -r0 -n 2 cp -f --
        )
    done

    (
        cd c

        "$CC" --version

        find '.' -mindepth 1 -type f -not -path "./.*" \
                                     -not -path "*/include/*" \
                                     -printf "%P\0" \
            | xargs -r0 -n 1 -P "$(nproc --ignore=2)" sh -c '
                set -e
                out="${1%.c}"
                out="${out##*/}"
                case "$1" in
                    */wrapper/*) out="wrapper/$out";;
                esac
                '"$CC"' '"$CFLAGS"' "$1" '"$CLIBS"' -o "$binprefix/$out" \
                    && printf "\0%s\0" "$binprefix/$out" >> ../.installed
            ' --
    )
        
    (
        cd cpp

        "$CXX" --version

        find '.' -mindepth 1 -type f -not -path "./.*" \
                                     -not -path "*/include/*" \
                                     -printf "%P\0" \
            | xargs -r0 -n 1 -P "$(nproc --ignore=2)" sh -c '
                set -e
                out="${1%.cpp}"
                out="${out##*/}"
                case "$1" in
                    */wrapper/*) out="wrapper/$out";;
                esac
                '"$CXX"' '"$CXXFLAGS"' "$1" '"$CXXLIBS"' -o "$binprefix/$out" \
                    && printf "\0%s\0" "$binprefix/$out" >> ../.installed
            ' --
    )

    (
        cd man
        find '.' -mindepth 1 -maxdepth 1 -type d -printf "%P\n" \
            | while IFS= read -r section; do
                mkdir -p -- "$manprefix/man$section" >&2
                export section
                find "$section" -type f -print0 | xargs -r0 -n 1 -P "$(($(nproc) * 4))" sh -c '
                    progname="$(basename -- "$1")"
                    manpath="$manprefix/man$section/${progname%.*}.1"
                    m4 -I"$rootdir" -DVERSION="$shorthash" -DTHIS="$1" "$1" | pandoc --standalone --to man -o "$manpath" >&2
                    printf "\0%s\0" "$manpath"
                ' --
            done >> ../.installed
    )
}

uninstall() {
    xargs -r0 -n 2 sh -c 'printf "%s\0" "$2"' -- < .installed | xargs -r0 rm -f --
    rm -f .installed
}

unittest() {
    (
        cd tests

        tmpin="$(mktemp)"
        tmpout="$(mktemp)"
        tmperr="$(mktemp)"
        trap "rm -f -- '$tmpout' '$tmperr'; exit 1" INT EXIT HUP TERM

        find '.' -mindepth 1 -maxdepth 1 -type d -printf "%P\n" | while IFS= read -r cmd; do
            find "$cmd" -mindepth 1 -maxdepth 1 -type d -printf "%P\n" | while IFS= read -r testname; do
                testdir="$cmd/$testname"

                "$testdir/in" > "$tmpin"
                eval "'$cmd' $("$testdir/args") > '$tmpout' 2> '$tmperr' < '$tmpin'"
                ec="$((ec | $?))"

                failstr=
                "$testdir/err" | cmp -s -- "$tmperr" || failstr="stderr is different. $failstr"
                "$testdir/out" | cmp -s -- "$tmpout" || failstr="stdout is different. $failstr"
                testec="$("$testdir/ec")"
                [ "$ec" -ne "$testec" ] && failstr="Expected exit code $testec, got $ec. $failstr"

                [ -n "$failstr" ] \
                    && printf "$C_RED%s: Test %s failed. %s$C_CLR\n" \
                        "$cmd" \
                        "$testdir" \
                        "$failstr" \
                    || true
            done
        done | sponge

        rm -f -- "$tmpout" "$tmperr"
        trap - INT EXIT HUP TERM
    )
}

format() {
    find 'c' 'cpp' -type f \( -iname "*.c"   -o -iname "*.h" \
                           -o -iname "*.cpp" -o -iname "*.hpp" \) -print0 \
        | sort -z \
        | xargs -r0 clang-format -i --style=file --
}

analyse() {
    printf "%s\n" \
        "" \
        "==========" \
        "shellcheck" \
        "==========" \
        ""

    shellcheck --version

    find 'bash' 'sh' -mindepth 1 -type f -executable \
        -not -path "*/.archived/*" -print0 \
        | xargs -r0 $unbuffer \
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
                --
    ec="$((ec | $?))"

    command -v shfmt > /dev/null && {
        printf "%s\n" \
            "" \
            "==========" \
            "shfmt" \
            "==========" \
            ""

        shfmt --version

        find 'sh' -mindepth 1 -type f -executable \
            -not -path "*/.archived/*" -print0 \
            | xargs -r0 $unbuffer \
                shfmt -p -- > /dev/null
        ec="$((ec | $?))"
    }

    find 'bash' -mindepth 1 -type f -executable \
        -not -path "*/.archived/*" -print0 \
        | xargs -r0 $unbuffer \
            shfmt -ln bash -- > /dev/null
    ec="$((ec | $?))"

    printf "%s\n" \
        "" \
        "==========" \
        "scan-build" \
        "==========" \
        ""

    clang --version
    clang-tidy --version

    tmpout="$(mktemp)"
    trap "rm -f -- '$tmpout'" INT EXIT TERM

    find 'c' -mindepth 1 -type f -iname "*.c" -print0 \
        | xargs -r0 -I FILE \
            scan-build -analyze-headers --status-bugs \
                $v $view -maxloop "$m" -no-failure-reports \
                "$CC" $CFLAGS 'FILE' $CLIBS -o "$tmpout"
    ec="$((ec | $?))"
    find 'cpp' -mindepth 1 -type f -iname "*.cpp" -print0 \
        | xargs -r0 -I FILE \
            scan-build -analyze-headers --status-bugs \
                $v $view -maxloop "$m" -no-failure-reports \
                "$CXX" $CXXFLAGS 'FILE' $CXXLIBS -o "$tmpout" || ec="$?"
    ec="$((ec | $?))"
}

spell() {
    (cd .. && codespell --builtin "clear,rare,informal" -L "ans,ba,erformance" -- **)
}

set -ex

cd ..
rootdir="$PWD"
shorthash="$(git rev-parse --short HEAD)"
cd "$OLDPWD"
export rootdir
export shorthash


if [ -t 1 ]; then
    C_RED='\033[0;31m'
    C_CLR='\033[0m'
    unbuffer="unbuffer"
else
    unset C_RED C_CLR unbuffer
fi

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
    prefix="$PREFIX"
elif isroot; then
    prefix="/usr/local"
else
    prefix="$HOME/.local"
fi
binprefix="$prefix/bin"
manprefix="$prefix/share/man"

export prefix
export binprefix
export manprefix

mkdir -p -- "$prefix" "$manprefix"
[ -d "$binprefix" ] && [ -d "$manprefix" ]

cmd="$1"
shift
unset o g ndebug view v
for i; do
    case "$i" in
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
                "") ndebug="-DNDEBUG";;
                g) g="-g";;
                gdb) g="-ggdb";;
                *) exit 1;;
            esac
            ;;
        view=*)
            val="${i#view=}"
            case "$val" in
                true|1) view="--view";;
                false|0|"") unset view;;
                *) exit 1;;
            esac
            ;;
        v=*)
            val="${i#v=}"
            case "$val" in
                0|"") unset v;;
                1) v="-v";;
                2) v="-v -v";;
                3) v="-v -v -v";;
                4) v="-v -v -v -v";;
                *) exit 1;;
            esac
            ;;
        m=*)
            val="${i#m=}"
            if [ -z "$val" ] || [ "$val" -le 0 ]; then
                m=4
            else
                m="$val"
            fi
            ;;
        *) logerrq "Unrecognised argument [%s]." "$i";;
    esac
done

CC="gcc"
CFLAGS="-O${o:-3} $g $ndebug -std=c99 -pedantic \
       -Wall -Wextra -Werror -Wabi=11 \
       -Wno-unused-parameter -Wno-unused-result \
       -Wno-implicit-fallthrough -Wno-sign-compare \
       -Wfloat-equal -Wdouble-promotion -Wjump-misses-init -Wstringop-overflow=4 \
       -Wold-style-definition -Winline -Wpadded -Wpacked -Wdisabled-optimization \
       -Iinclude"
CLIBS="-lm -lmagic"
export C_INCLUDE_PATH="$PWD/c/include"

CXX="g++"
CXXFLAGS="-O${o:-3} $g $ndebug -std=c++2b \
          -Wall -Wextra -Werror -Wabi=11 \
          -Wno-unused-parameter -Wno-unused-result \
          -Wno-implicit-fallthrough -Wno-sign-compare -Wstringop-overflow=4 \
          -Wfloat-equal -Wdouble-promotion -Wdisabled-optimization \
          -Iinclude"
CXXLIBS=""
export CPLUS_INCLUDE_PATH="$PWD/cpp/include"

ec=0

case "$cmd" in
    install) install;;
    uninstall) uninstall;;
    test) unittest;;
    format) format;;
    analyse) analyse;;
    spell) spell;;
    *) logerrq "Unkown target given: %s." "$1";;
esac

exit "${ec:-0}"
