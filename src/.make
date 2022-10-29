#!/usr/bin/env sh

logerrq() {
    printf "$@"
    exit 1
}

cancompilecpp() {
    if { [ "$CXX" != "g++" ] && [ "$CXX" != "gcc" ]; } \
        || [ "$("$CXX" --version | head -n 1 | sed 's/.* \([0-9]\+\)\.[0-9]\+\.[0-9]\+/\1/')" -ge 12 ]; then
        printf 1
    else
        printf 0
    fi
}

FUNC_PARSEFLAGS='
[ -n "$SHELL_VERBOSE" ] && [ "$SHELL_VERBOSE" -gt 0 ] && set -x
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

all() {
    ! command -v ctags > /dev/null 2>&1 || index
}

index () {
    printf "%s\n" \
        "=====" \
        "INDEX" \
        "=====" \
        ""

    ctags -R --c++-kinds=+p --fields=+iaS --extras=+q .
}

install() {
    printf "%s\n" \
        "=======" \
        "INSTALL" \
        "=======" \
        ""

    mkdir -p -- "$binprefix" "$manprefix" "$dataprefix"
    mkdir "$binprefix/wrapper" 2> /dev/null \
        && printf "$C_RED%s %s$C_CLR\n" \
            "There was no $binprefix/wrapper directory, so it was created for you." \
            "Be sure to add it to your PATH with high priority."
    mkdir "$dataprefix/include" 2> /dev/null \
        && printf "$C_RED%s %s$C_CLR\n" \
            "There was no $dataprefix/include directory, so it was created for you." \
            "Be sure to add it to your PATH with high priority."

    printf "%s\n" \
        "Installing Bash, execline, Python and shell scripts:"

    for i in bash el py sh; do
        (
            cd "$i"

            find '.' -mindepth 1 -type f      -executable \
                                         -not -path "*/.archived/*" \
                                         -not -path "*/wrapper/*" \
                                         -not -path "*/include/*" \
                                         -printf "%P\0$binprefix/%f\0" \
                | tee -a ../.installed \
                | i="$i" xargs -r0 -n 2 sh -c '
                    printf "  %s -> %s\n" "$i/$1" "$2"
                    cp -f -- "$@"
                ' --

            find '.' -mindepth 1 -type f      -executable \
                                         -not -path "*/.archived/*" \
                                              -path "*/wrapper/*" \
                                         -printf "%P\0$binprefix/wrapper/%f\0" \
                | tee -a ../.installed \
                | i="$i" xargs -r0 -n 2 sh -c '
                    printf "  %s -> %s\n" "$i/$1" "$2"
                    cp -f -- "$@"
                ' --
        )
    done

    printf "\n%s\n" \
        "Installing Bash and shell libraries:"

    for i in bash sh; do
        (
            cd "$i"

            find '.' -mindepth 1 -type f -not -executable \
                                         -not -path "*/.archived/*" \
                                              -path "*/include/*" \
                                         -printf "%P\0$dataprefix/include/%f\0" \
                | tee -a ../.installed \
                | i="$i" xargs -r0 -n 2 sh -c '
                    printf "  %s -> %s\n" "$i/$1" "$2"
                    cp -f -- "$@"
                ' --
        )
    done

    printf "\n%s\n" \
        "Preparing to install C programs:"

    (
        cd c

        printf "%s\n" \
            "  Using C toolchain:" \
            "    Compiler: $CC" \
            "    Compiler version: $(
                {
                    "$CC" --version 2>&1 \
                        || "$CC" -v 2>&1 \
                        || printf "%s\n" "could not be determined"
                } | head -n 1 | grep -o "[0-9]\+\.[0-9]\+\.[0-9]\+"
            )" \
            "    Global compiler flags: $(printf "%s\n" "$CFLAGS" | tr -d '\n' | sed 's/^\s\+//;s/\s\+$//;s/\s\+/ /g')" \
            "    Global linker flags: $(printf "%s\n" "$CLDFLAGS" | tr -d '\n' | sed 's/^\s\+//;s/\s\+$//;s/\s\+/ /g')" \
            "" \
            "Installing C programs:"

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
                printf "  %s -> %s\n    Local compiler flags: %s\n    Local linker flags: %s\n" \
                    "$1" \
                    "$binprefix/$out" \
                    "$(printf "%s\n" "$flags" | tr -d "\n" | sed "s/^\s\+//;s/\s\+$//;s/\s\+/ /g")" \
                    "$(printf "%s\n" "$ldflags" | tr -d "\n" | sed "s/^\s\+//;s/\s\+$//;s/\s\+/ /g")"
                '"$CC"' '"$CFLAGS"' $flags "$1" '"$CLDFLAGS"' $ldflags -o "$binprefix/$out" \
                    && printf "\0%s\0" "$binprefix/$out" >> ../.installed
            ' --
    )

    printf "\n%s\n" \
        "Preparing to install C++ programs:"
        
    (
        cd cpp

        printf "%s\n" \
            "  Using C++ toolchain:" \
            "    Compiler: $CXX" \
            "    Compiler version: $(
                {
                    "$CXX" --version 2>&1 \
                        || "$CXX" -v 2>&1 \
                        || printf "%s\n" "could not be determined"
                } | head -n 1 | grep -o "[0-9]\+\.[0-9]\+\.[0-9]\+"
            )"

        [ "$cancompilecpp" -ne 0 ] || {
            printf "%s\n" \
                "Compiler version is too old. Cannot compile C++ programs."
            exit 0
        }

        printf "%s\n" \
            "    Global compiler flags: $(printf "%s\n" "$CXXFLAGS" | tr -d '\n' | sed 's/^\s\+//;s/\s\+$//;s/\s\+/ /g')" \
            "    Global linker flags: $(printf "%s\n" "$CXXLDFLAGS" | tr -d '\n' | sed 's/^\s\+//;s/\s\+$//;s/\s\+/ /g')" \
            "" \
            "Installing C++ programs:"

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
                printf "  %s -> %s\n    Local compiler flags: %s\n    Local linker flags: %s\n" \
                    "$1" \
                    "$binprefix/$out" \
                    "$(printf "%s\n" "$flags" | tr -d "\n" | sed "s/^\s\+//;s/\s\+$//;s/\s\+/ /g")" \
                    "$(printf "%s\n" "$ldflags" | tr -d "\n" | sed "s/^\s\+//;s/\s\+$//;s/\s\+/ /g")"
                '"$CXX"' '"$CXXFLAGS"' $flags "$1" '"$CXXLDFLAGS"' $ldflags -o "$binprefix/$out" \
                    && printf "\0%s\0" "$binprefix/$out" >> ../.installed
            ' --
    )

    printf "\n%s\n" \
        "Installing C libraries:"

    (
        cd c/include

        find '.' -type f -printf "%P\0" | xargs -r0 -n 1 -P 0 sh -c '
            fl="$1"
            printf "\0%s\0" "$includeprefix/$fl"
            printf "  %s -> %s\n" "$fl" "$includeprefix/$fl" >&2
            cp -f -t "$includeprefix" -- "$fl"
        ' -- 2>&1 >> "$rootdir/src/.installed"
    )

    printf "\n%s\n" \
        "Installing C++ libraries:"

    (
        cd cpp/include

        find '.' -type f -printf "%P\0" | xargs -r0 -n 1 -P 0 sh -c '
            fl="$1"
            printf "\0%s\0" "$includeprefix/$fl"
            printf "  %s -> %s\n" "$fl" "$includeprefix/$fl" >&2
            cp -f -t "$includeprefix" -- "$fl"
        ' -- 2>&1 >> "$rootdir/src/.installed"
    )

    printf "\n%s\n" \
        "Installing external C and C++ libraries:"

    (
        printf "%s\0" \
            "$rootdir/lib/hedley/hedley.h" \
            "$rootdir/lib/pstreams/pstream.h" \
            | xargs -r0 -n 1 -P 0 sh -c '
                fl="$1"
                printf "\0%s\0" "$includeprefix/${fl##*/}"
                printf "  %s -> %s\n" "${fl##"$rootdir/lib/"}" "$includeprefix/${fl##*/}" >&2
                cp -f -t "$includeprefix" -- "$fl"
            ' -- 2>&1 >> "$rootdir/src/.installed"
    )

    printf "\n%s\n" \
        "Installing man pages:"

    (
        cd man
        find '.' -mindepth 1 -maxdepth 1 -type d -printf "%P\n" \
            | while IFS= read -r section; do
                mkdir -p -- "$manprefix/man$section" >&2
                export section
                find "$section" \( -type f -o -type l \) -print0 | xargs -r0 -n 1 -P "$(($(nproc) * 4))" sh -c '
                    progname="$(basename -- "$1")"
                   manpath="$manprefix/man$section/${progname%.*}.$section"
                    printf "  %s -> %s\n" "$section/$progname" "$manpath" >&2
                    m4 -I"$rootdir" -DVERSION="$shorthash" -DTHIS="$1" "$1" \
                        | pandoc --standalone --to man -o "$manpath" >&2
                    printf "\0%s\0" "$manpath"
                ' --
            done 2>&1 >> ../.installed
    )
}

uninstall() {
    printf "%s\n" \
        "=========" \
        "UNINSTALL" \
        "=========" \
        ""

    [ -f .installed ] || {
        printf "%s\n" \
            "Nothing to do."
        return 0
    }

    printf "%s\n" \
        "Uninstalling all installed files."

    xargs -r0 -n 2 sh -c 'printf "%s\0" "$2"' -- < .installed | xargs -r0 sh -c '
        printf "  %s\n" "$@"
        rm -f -- "$@"
    ' --

    rm -f .installed
}

unittest() {
    printf "%s\n" \
        "====" \
        "TEST" \
        "====" \
        ""

    (
        cd tests

        tmpin="$(mktemp)"
        tmpout="$(mktemp)"
        tmperr="$(mktemp)"
        trap "rm -f -- '$tmpout' '$tmperr'; exit 1" INT EXIT HUP TERM

        printf "%s\n" \
            "Testing scripts and programs:"

        find '.' -mindepth 1 -maxdepth 1 -type d -printf "%P\n" | while IFS= read -r cmd; do
            printf "%s\n" \
                "  $cmd:"
            command -v -- "$cmd" > /dev/null 2>&1 || {
                printf "%s\n" \
                    "    Not installed, cannot test."
                continue
            }

            find "$cmd" -mindepth 1 -maxdepth 1 -type d -printf "%P\n" | while IFS= read -r testname; do
                printf "%s" \
                    "    $testname: "

                testdir="$cmd/$testname"

                "$testdir/in" > "$tmpin"
                eval "'$cmd' $("$testdir/args") > '$tmpout' 2> '$tmperr' < '$tmpin'"
                evalec="$?"

                failstr=
                "$testdir/err" | cmp -s -- "$tmperr" || failstr="stderr is different. $failstr"
                "$testdir/out" | cmp -s -- "$tmpout" || failstr="stdout is different. $failstr"
                testec="$("$testdir/ec")"
                [ "$evalec" -ne "$testec" ] && failstr="Expected exit code $testec, got $evalec. $failstr"

                if [ -n "$failstr" ]; then
                    printf "%s\n" \
                        "Failed. $failstr"
                else
                    printf "%s\n" \
                        "Passed."
                fi
            done
        done | sponge

        rm -f -- "$tmpout" "$tmperr"
        trap - INT EXIT HUP TERM
    )
}

format() {
    printf "%s\n" \
        "======" \
        "FORMAT" \
        "======" \
        ""

    printf "%s\n" \
        "Formatting Python source files:"

    find 'py' -type f -executable -print0 | xargs -r0 -n 1 sh -c '
        fl="$1"
        printf "  %s\n" "$fl"
        tmp="$(mktemp)"
        trap "rm -f -- \"\$tmp\"" INT EXIT TERM
        cp -f -- "$fl" "$tmp"
        black -q -- "$fl"
        if cmp -s -- "$tmp" "$fl"; then
            printf "    %s\n" \
                "Nothing to do."
        else
            printf "    %s\n" \
                "Fixed formatting."
        fi
        rm -f -- "$tmp"
    ' --

    printf "\n%s\n" \
        "Formatting C and C++ source and header files:"

    find 'c' 'cpp' -type f \( -iname "*.c"   -o -iname "*.h" \
                           -o -iname "*.cpp" -o -iname "*.hpp" \) -print0 \
        | xargs -r0 -n 1 sh -c '
            fl="$1"
            printf "  %s\n" "$fl"
            tmp="$(mktemp)"
            trap "rm -f -- \"\$tmp\"" INT EXIT TERM
            cp -f -- "$fl" "$tmp"
            clang-format -i --style=file -- "$fl"
            if cmp -s -- "$tmp" "$fl"; then
                printf "    %s\n" \
                    "Nothing to do."
            else
                printf "    %s\n" \
                    "Fixed formatting."
            fi
            rm -f -- "$tmp"
        ' --
}

analyse() {
    printf "%s\n" \
        "========" \
        "ANALYSE" \
        "========" \
        ""

    printf "%s\n" \
        "Preparing to analyse Bash and shell scripts and libraries:" \
        "  Validator: shfmt" \
        "  Validator version: $(shfmt --version)" \
        "  Analyser: shellcheck" \
        "  Analyser version: $(shellcheck --version | head -n 2 | tail -n 1 | cut -d' ' -f2)" \
        "" \
        "Analysing Bash and shell scripts and libraries:"

    find 'bash' -mindepth 1 -type f -executable \
        -not -path "*/.archived/*" -print0 \
        | unbuffer="$unbuffer" xargs -r0 sh -c '
            for fl; do
                printf "  %s\n" "$fl"
                shfmt -ln bash -- > /dev/null || {
                    printf "    %s\n" "Invalid syntax."
                    continue
                }
                $unbuffer shellcheck -- "$fl" 2>&1 | sed "s/^/    /"
            done
        ' --
    ec="$((ec | $?))"
    find 'sh' -mindepth 1 -type f -executable \
        -not -path "*/.archived/*" -print0 \
        | unbuffer="$unbuffer" xargs -r0 sh -c '
            for fl; do
                printf "  %s\n" "$fl"
                shfmt -p -- > /dev/null || {
                    printf "    %s\n" "Invalid syntax."
                    continue
                }
                $unbuffer shellcheck -- "$fl" 2>&1 | sed "s/^/    /"
            done
        ' --
    ec="$((ec | $?))"

    tmpout="$(mktemp)"
    trap "rm -f -- '$tmpout'" INT EXIT TERM

    printf "%s\n" \
        "" \
        "Preparing to analyse C header and source files:" \
        "  Compiler: $CC" \
        "  Compiler version: $(
            {
                "$CC" --version 2>&1 \
                    || "$CC" -v 2>&1 \
                    || printf "%s\n" "could not be determined"
            } | head -n 1 | grep -o "[0-9]\+\.[0-9]\+\.[0-9]\+"
        )" \
        "  Global compiler flags: $(printf "%s\n" "$CFLAGS" | tr -d '\n' | sed 's/^\s\+//;s/\s\+$//;s/\s\+/ /g')" \
        "  Global linker flags: $(printf "%s\n" "$CLDFLAGS" | tr -d '\n' | sed 's/^\s\+//;s/\s\+$//;s/\s\+/ /g')" \
        "  Analyser: clang-tidy" \
        "  Analyser version: $(clang-tidy --version | head -n 2 | tail -n 1 | cut -d' ' -f5)" \
        "  Analyser complementary compiler: clang" \
        "  Analyser complementary compiler version: $(clang --version | head -n 1 | cut -d' ' -f3)" \
        "" \
        "Analysing C headers:"

    find 'c/include' -mindepth 1 -type f -iname "*.h" -print0 \
        | CC="$CC" CFLAGS="$CFLAGS" CLDFLAGS="$CLDFLAGS" tmpout="$tmpout" xargs -r0 sh -c '
            for fl; do
                printf "  %s\n" "$fl"
                "$CC" $CFLAGS "$fl" $CLDFLAGS -o "$tmpout" \
                    || printf "    %s\n" "Does not compile."
            done
        ' --

    printf "\n%s\n" \
        "Analysing C source files:"

    find 'c' -mindepth 1 -type f -iname "*.c" -print0 \
        | m="$m" v="$v" view="$view" CC="$CC" CFLAGS="$CFLAGS" CLDFLAGS="$CLDFLAGS" tmpout="$tmpout" xargs -r0 -n 1 sh -c '
                '"$FUNC_PARSEFLAGS"'
                parseflags "$1"
                printf "  %s\n    Local compiler flags: %s\n    Local linker flags %s\n" \
                    "$1" \
                    "$(printf "%s\n" "$flags" | tr -d "\n" | sed "s/^\s\+//;s/\s\+$//;s/\s\+/ /g")" \
                    "$(printf "%s\n" "$ldflags" | tr -d "\n" | sed "s/^\s\+//;s/\s\+$//;s/\s\+/ /g")"
                err="$(
                    scan-build \
                        -analyze-headers --status-bugs $v $view \
                        -maxloop "$m" -no-failure-reports \
                        --use-cc="$(command -v "$CC")" \
                        "$CC" $CFLAGS $flags "$1" $CLDFLAGS $ldflags -o "$tmpout" 2>&1 > /dev/null
                )"
                [ -z "$err" ] || {
                    printf "%s\n" "    Analyser output:"
                    printf "%s\n" "$err" | sed "s/^/      /"
                }
            ' --
    ec="$((ec | $?))"

    printf "%s\n" \
        "" \
        "Preparing to analyse C++ header and source files:" \
        "  Compiler: $CXX" \
        "  Compiler version: $(
            {
                "$CXX" --version 2>&1 \
                    || "$CXX" -v 2>&1 \
                    || printf "%s\n" "could not be determined"
            } | head -n 1 | grep -o "[0-9]\+\.[0-9]\+\.[0-9]\+"
        )"

    if [ "$cancompilecpp" -eq 0 ]; then
            printf "%s\n" \
                "Compiler version is too old. Cannot analyse C++ header files."
    else
        printf "%s\n" \
            "  Compiler flags: $(printf "%s\n" "$CXXFLAGS" | tr -d '\n' | sed 's/^\s\+//;s/\s\+$//;s/\s\+/ /g')" \
            "  Linker flags: $(printf "%s\n" "$CXXLDFLAGS" | tr -d '\n' | sed 's/^\s\+//;s/\s\+$//;s/\s\+/ /g')" \
            "" \
            "Analysing C++ headers:"

            find 'cpp/include' -mindepth 1 -type f -iname "*.hpp" -print0 \
                | CXX="$CXX" CXXFLAGS="$CXXFLAGS" CXXLDFLAGS="$CXXLDFLAGS" tmpout="$tmpout" xargs -r0 sh -c '
                    for fl; do
                        printf "  %s\n" "$fl"
                        "$CXX" $CXXFLAGS "$fl" $CXXLDFLAGS -o "$tmpout" \
                            || printf "    %s\n" "Does not compile."
                    done
                ' --
    fi

    printf "\n%s\n" \
        "Analysis of C++23 source files is not fully supported by the analyser. Cannot analyse C++ source files."

    rm -f -- "$tmpout"
}

spell() {
    printf "%s\n" \
        "=====" \
        "SPELL" \
        "=====" \
        ""

    (
        cd .. || exit 1

        printf "\n%s\n" \
            "Checking files and files in directories for spelling mistakes:"
        find '.' -mindepth 1 -maxdepth 1 \( -not -name ".git" -a -not -name "lib" \) -print0 \
            | xargs -r0 sh -c '
                for fl; do
                    printf "  %s" "$fl"
                    [ -d "$fl" ] && printf "/"
                    printf "\n"
                    codespell --builtin "clear,rare,informal" -L "ans,ba,erformance" -- "$fl"
                done
            ' --
    )
}

set -e
[ -n "$SHELL_VERBOSE" ] && [ "$SHELL_VERBOSE" -gt 0 ] && set -x

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
          -Wall -Wextra -Werror -Wabi=12 \
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

all

case "$cmd" in
    index) index;;
    install) install;;
    uninstall) uninstall;;
    test) unittest;;
    format) format;;
    analyse) analyse;;
    spell) spell;;
    *) logerrq "Unkown target given: %s." "$1";;
esac

exit "${ec:-0}"
