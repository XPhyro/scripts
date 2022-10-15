#!/usr/bin/env sh

logerrq() {
    printf "$@"
    exit 1
}

cancompilecpp() {
    if { [ "$CXX" != "g++" ] && [ "$CXX" != "gcc" ]; } \
        || [ "$("$CXX" --version | head -n 1 | sed 's/.* \([0-9]\+\)\.[0-9]\+\.[0-9]\+/\1/')" -gt 12 ]; then
        printf 1
    else
        printf 0
    fi
}

FUNC_PARSEFLAGS='
set -x
parseflags() {
    flags=""
    ldflags=""

    firstline="$(head -n 1 -- "$1")"
    if printf "%s\n" "$firstline" | grep -q "^\s*//"; then
        multiline=0
        firstlineregex="/"
        lineregex="\/\/"
        inverselineregex="//"
    elif printf "%s\n" "$firstline" | grep -q "^\s*/\*"; then
        multiline=1
        firstlineregex="\*"
        lineregex="*?"
        inverselineregex=""
    else
        return
    fi

    eval "$(
        {   printf "%s\n" "$firstline" | sed -E "s#^\s*/$firstlineregex#$inverselineregex#"
            tail -n +2 -- "$1" | awk "{if (/^\s*$lineregex/) print; else exit;}"
        } | sed -En "s#^\s*$lineregex\s*@([A-Z0-9_-]+):?(.*)\$#\1=\"\2\"#p" \
          | while IFS= read -r line; do
                eval "$line"
                [ -n "$FLAGS" ] && flags="$flags $FLAGS"
                [ -n "$CFLAGS" ] && flags="$flags $CFLAGS"
                [ -n "$CXXFLAGS" ] && flags="$flags $CXXFLAGS"
                [ -n "$LDFLAGS" ] && ldflags="$ldflags $LDFLAGS"
                printf "flags='\''%s'\''\nldflags='\''%s'\''\n" "$flags" "$ldflags"
            done | tail -n 2
    )"
}
'

install() {
    mkdir -p -- "$binprefix" "$manprefix" "$dataprefix"
    mkdir "$binprefix/wrapper" 2> /dev/null \
        && printf "$C_RED%s %s$C_CLR\n" \
            "There was no $binprefix/wrapper directory, so it was created for you." \
            "Be sure to add it to your PATH with high priority."
    mkdir "$dataprefix/include" 2> /dev/null \
        && printf "$C_RED%s %s$C_CLR\n" \
            "There was no $dataprefix/include directory, so it was created for you." \
            "Be sure to add it to your PATH with high priority."

    for i in bash el py sh; do
        (
            cd "$i"

            find '.' -mindepth 1 -type f      -executable \
                                         -not -path "*/.archived/*" \
                                         -not -path "*/wrapper/*" \
                                         -not -path "*/include/*" \
                                         -printf "%P\0$binprefix/%f\0" \
                | tee -a ../.installed \
                | xargs -r0 -n 2 cp -f --

            find '.' -mindepth 1 -type f      -executable \
                                         -not -path "*/.archived/*" \
                                              -path "*/wrapper/*" \
                                         -printf "%P\0$binprefix/wrapper/%f\0" \
                | tee -a ../.installed \
                | xargs -r0 -n 2 cp -f --
        )
    done

    for i in bash sh; do
        (
            cd "$i"

            find '.' -mindepth 1 -type f -not -executable \
                                         -not -path "*/.archived/*" \
                                              -path "*/include/*" \
                                         -printf "%P\0$dataprefix/include/%f\0" \
                | tee -a ../.installed \
                | xargs -r0 -n 2 cp -f --
        )
    done

    (
        cd c

        "$CC" --version

        # TODO: don't write to .installed in xargs
        find '.' -mindepth 1 -type f -not -path "./.*" \
                                     -not -path "*/include/*" \
                                     -printf "%P\0" \
            | xargs -r0 -n 1 -P "$(nproc --ignore=2)" sh -c '
                '"$FUNC_PARSEFLAGS"'
                parseflags "$1"
                set -e
                out="${1%.c}"
                out="${out##*/}"
                case "$1" in
                    */wrapper/*) out="wrapper/$out";;
                esac
                '"$CC"' '"$CFLAGS"' $flags "$1" '"$CLDFLAGS"' $ldflags -o "$binprefix/$out" \
                    && printf "\0%s\0" "$binprefix/$out" >> ../.installed
            ' --
    )
        
    (
        [ "$cancompilecpp" -eq 0 ] || {
            printf "%s\n" "Not attempting to compile C++ programs as $CXX version is too old."
            exit 0
        }

        cd cpp

        printf "%s\n" "$cxxversion"

        # TODO: don't write to .installed in xargs
        find '.' -mindepth 1 -type f -not -path "./.*" \
                                     -not -path "*/include/*" \
                                     -printf "%P\0" \
            | xargs -r0 -n 1 -P "$(nproc --ignore=2)" sh -c '
                '"$FUNC_PARSEFLAGS"'
                parseflags "$1"
                set -e
                out="${1%.cpp}"
                out="${out##*/}"
                case "$1" in
                    */wrapper/*) out="wrapper/$out";;
                esac
                '"$CXX"' '"$CXXFLAGS"' $flags "$1" '"$CXXLDFLAGS"' $ldflags -o "$binprefix/$out" \
                    && printf "\0%s\0" "$binprefix/$out" >> ../.installed
            ' --
    )

    (
        cd c/include

        find '.' -type f -printf "%P\0" | xargs -r0 -n 1 -P 0 sh -c '
            fl="$1"
            printf "\0%s\0" "$includeprefix/$fl"
            cp -f -t "$includeprefix" -- "$fl"
        ' -- >> "$rootdir/src/.installed"
    )

    (
        cd cpp/include

        find '.' -type f -printf "%P\0" | xargs -r0 -n 1 -P 0 sh -c '
            fl="$1"
            printf "\0%s\0" "$includeprefix/$fl"
            cp -f -t "$includeprefix" -- "$fl"
        ' -- >> "$rootdir/src/.installed"
    )

    (
        printf "%s\0" \
            "$rootdir/lib/hedley/hedley.h" \
            "$rootdir/lib/pstreams/pstream.h" \
            | xargs -r0 -n 1 -P 0 sh -c '
                fl="$1"
                printf "\0%s\0" "$includeprefix/${fl##*/}"
                cp -f -t "$includeprefix" -- "$fl"
            ' -- >> "$rootdir/src/.installed"
    )

    (
        cd man
        find '.' -mindepth 1 -maxdepth 1 -type d -printf "%P\n" \
            | while IFS= read -r section; do
                mkdir -p -- "$manprefix/man$section" >&2
                export section
                find "$section" \( -type f -o -type l \) -print0 | xargs -r0 -n 1 -P "$(($(nproc) * 4))" sh -c '
                    progname="$(basename -- "$1")"
                    manpath="$manprefix/man$section/${progname%.*}.$section"
                    m4 -I"$rootdir" -DVERSION="$shorthash" -DTHIS="$1" "$1" \
                        | pandoc --standalone --to man -o "$manpath" >&2
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
            command -v -- "$cmd" > /dev/null 2>&1 || continue

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
    find 'py' -type f -executable -print0 | xargs -r0 black --

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
                -e SC1091 \
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
            "=====" \
            "shfmt" \
            "=====" \
            ""

        shfmt --version

        find 'sh' -mindepth 1 -type f -executable \
            -not -path "*/.archived/*" -print0 \
            | xargs -r0 $unbuffer \
                shfmt -p -- > /dev/null
        ec="$((ec | $?))"

        find 'bash' -mindepth 1 -type f -executable \
            -not -path "*/.archived/*" -print0 \
            | xargs -r0 $unbuffer \
                shfmt -ln bash -- > /dev/null
        ec="$((ec | $?))"
    }

    tmpout="$(mktemp)"
    trap "rm -f -- '$tmpout'" INT EXIT TERM

    printf "%s\n" \
        "" \
        "==="\
        "$CC" \
        "==="\
        ""

    "$CC" --version
    find 'c/include' -mindepth 1 -type f -iname "*.h" -print0 \
        | xargs -r0 -I FILE "$CC" $CFLAGS FILE $CLDFLAGS -o "$tmpout"

    printf "%s\n" \
        "" \
        "==="\
        "$CXX" \
        "==="\
        ""

    if [ "$cancompilecpp" -eq 0 ]; then
        printf "%s\n" "Not attempting to analyse C++ headers as $CXX version is too old."
    else
        "$CXX" --version
        find 'cpp/include' -mindepth 1 -type f -iname "*.hpp" -print0 \
            | xargs -r0 -I FILE "$CXX" $CXXFLAGS FILE $CXXLDFLAGS -o "$tmpout"
    fi

    printf "%s\n" \
        "" \
        "==========" \
        "scan-build" \
        "==========" \
        ""

    clang --version
    clang-tidy --version

    find 'c' -mindepth 1 -type f -iname "*.c" -print0 \
        | m="$m" v="$v" view="$view" CC="$CC" CFLAGS="$CFLAGS" CLDFLAGS="$CLDFLAGS" tmpout="$tmpout" xargs -r0 -n 1 sh -c '
                '"$FUNC_PARSEFLAGS"'
                parseflags "$1"
                scan-build \
                    -analyze-headers --status-bugs $v $view \
                    -maxloop "$m" -no-failure-reports \
                    --use-cc="$(command -v "$CC")" \
                    "$CC" $CFLAGS $flags "$1" $CLDFLAGS $ldflags -o "$tmpout"
            ' --
    ec="$((ec | $?))"
    # TODO: re-enable this after clang implements c++2b ranges
    # find 'cpp' -mindepth 1 -type f -iname "*.cpp" -print0 \
    #     | xargs -r0 -I FILE \
    #         scan-build -analyze-headers --status-bugs \
    #             $v $view -maxloop "$m" -no-failure-reports \
    #             "$CXX" $CXXFLAGS 'FILE' $CXXLDFLAGS -o "$tmpout" || ec="$?"
    # ec="$((ec | $?))"

    rm -f -- "$tmpout"
}

spell() {
    (
        cd .. || exit 1
        find . -mindepth 1 -maxdepth 1 \( -not -name ".git" -a -not -name "lib" \) -print0 \
            | xargs -r0 codespell --builtin "clear,rare,informal" -L "ans,ba,erformance" --
    )
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
includeprefix="$prefix/include"
manprefix="$prefix/share/man"
dataprefix="$prefix/share/scripts"

export prefix
export includeprefix
export binprefix
export manprefix

mkdir -p -- "$prefix" "$binprefix" "$includeprefix" "$manprefix" "$dataprefix"
[ -d "$binprefix" ] && [ -d "$includeprefix" ] && [ -d "$manprefix" ] && [ -d "$dataprefix" ]

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
       -Wno-unused-parameter -Wno-unused-result -Wswitch-default \
       -Wimplicit-fallthrough=5 -Wno-sign-compare \
       -Wfloat-equal -Wdouble-promotion -Wjump-misses-init -Wstringop-overflow=4 \
       -Wold-style-definition -Winline -Wpadded -Wpacked -Wdisabled-optimization \
       -ffp-contract=on -fassociative-math -ffast-math \
       -Iinclude -I'$rootdir/lib/hedley'"
CLDFLAGS=""
C_INCLUDE_PATH="$PWD/c/include:$rootdir/lib/hedley"
export C_INCLUDE_PATH

CXX="g++"
CXXFLAGS="-O${o:-3} $g $ndebug -std=c++2b \
          -Wall -Wextra -Werror -Wabi=11 \
          -Wswitch-default \
          -Wimplicit-fallthrough=5 -Wstringop-overflow=4 \
          -Wfloat-equal -Wdouble-promotion -Wdisabled-optimization \
          -Wstrict-null-sentinel -Wold-style-cast -Wsign-promo \
          -fpermissive -ffp-contract=on -fassociative-math -ffast-math \
          -Iinclude -I'$rootdir/src/c/include' \
          -I'$rootdir/lib/hedley' -I'$rootdir/lib/pstreams'"
CXXLDFLAGS=""
CPLUS_INCLUDE_PATH="$PWD/cpp/include:$PWD/c/include:$rootdir/lib/hedley:$rootdir/lib/pstreams"
export CPLUS_INCLUDE_PATH

ec=0
cancompilecpp="$(cancompilecpp "$CXX")"

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
