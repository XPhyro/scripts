#!/usr/bin/env sh

logerrq() {
    printf "$@"
    exit 1
}

getrealuser() {
    ppid="$PPID"
    self="$(id -un)"
    while ppid="$(ps -o ppid= -p "$ppid" | tr -d '[:space:]')"; do
        owner="$(ps -o user= -p "$ppid")" || {
            printf "%s" "$self"
            return
        }
        [ "$owner" != "$self" ] && {
            printf "%s" "$owner"
            return
        }
    done
}

cancompilecpp() {
    if { [ "$CXX" != "g++" ] && [ "$CXX" != "gcc" ]; } \
        || [ "$("$CXX" --version | head -n 1 | awk '{print $3}' | cut -d. -f1)" -ge 12 ] 2> /dev/null; then
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
    if command -v ctags > /dev/null 2>&1; then
        index
    fi
}

index() {
    printf "%s\n" \
        "=====" \
        "INDEX" \
        "=====" \
        "Indexing all source files."

    for i in */; do
        (cd -- "$i" && ctags -R --c++-kinds=+p --fields=+iaS --extras=+q .)
    done
}

install() {
    printf "%s\n" \
        "=======" \
        "INSTALL" \
        "======="

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
        "Installing AWK, Bash, execline, Python, Perl and shell scripts:"

    for i in awk bash el py pl sh; do
        (
            cd "$i"

            find '.' -mindepth 1 -type f      -executable \
                                         -not -path "*/.archived/*" \
                                         -not -path "*/wrapper/*" \
                                         -not -path "*/include/*" \
                                         -printf "%P\0$binprefix/%f\0" \
                | i="$i" xargs -r0 -n 2 sh -c '
                    printf "  %s -> %s\n" "$i/$1" "$2"
                    cp -f -- "$@"
                ' --

            find '.' -mindepth 1 -type f      -executable \
                                         -not -path "*/.archived/*" \
                                              -path "*/wrapper/*" \
                                         -printf "%P\0$binprefix/wrapper/%f\0" \
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
                | i="$i" xargs -r0 -n 2 sh -c '
                    printf "  %s -> %s\n" "$i/$1" "$2"
                    cp -f -- "$@"
                ' --
        )
    done

    printf "\n%s\n" \
        "Installing C libraries:"

    (
        cd c/include
        [ -d "$includeprefix/xph" ] || mkdir -- "$includeprefix/xph"

        # shellcheck disable=SC2069
        find '.' -type f -printf "%P\0" | xargs -r0 -n 1 -P "${MAKE_JOBS:-0}" sh -c '
            fl="$1"
            printf "  %s -> %s\n" "$fl" "$includeprefix/xph/$fl" >&2
            cp -f -t "$includeprefix/xph" -- "$fl"
        ' --
    )

    printf "\n%s\n" \
        "Installing C++ libraries:"

    (
        cd cpp/include

        # shellcheck disable=SC2069
        find '.' -type f -printf "%P\0" | xargs -r0 -n 1 -P "${MAKE_JOBS:-0}" sh -c '
            fl="$1"
            printf "  %s -> %s\n" "$fl" "$includeprefix/xph/$fl" >&2
            cp -f -t "$includeprefix/xph" -- "$fl"
        ' --
    )

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

        find '.' -mindepth 1 -type f -not -path "./.*" \
                                     -not -name "tags" \
                                     -not -path "*/include/*" \
                                     -not -path "*/.archived/*" \
                                     -printf "%P\0" \
            | xargs -r0 -n 1 -P "$CPU_PROC" sh -c '
                '"$FUNC_PARSEFLAGS"'
                parseflags "$1"
                set -e
                out="${1%.c}"
                out="${out##*/}"
                case "$1" in
                    shared/*|*/shared/*)
                        out="$out.so"
                        installprefix="$libprefix"
                        extraflags=-shared
                        ;;
                    *)
                        installprefix="$binprefix"
                        extraflags=
                        ;;
                esac
                if [ -f "$installprefix/$out" ] && [ "$installprefix/$out" -nt "$1" ]; then
                    printf "  %s -> %s\n    %s\n" \
                        "$1" \
                        "$installprefix/$out" \
                        "Already up-to-date."
                    exit 0
                fi
                case "$1" in
                    */wrapper/*) out="wrapper/$out";;
                esac
                printf "  %s -> %s\n    Local compiler flags: %s\n    Local linker flags: %s\n" \
                    "$1" \
                    "$installprefix/$out" \
                    "$(printf "%s\n" "$flags" | tr -d "\n" | sed "s/^\s\+//;s/\s\+$//;s/\s\+/ /g")" \
                    "$(printf "%s\n" "$ldflags" | tr -d "\n" | sed "s/^\s\+//;s/\s\+$//;s/\s\+/ /g")"
                '"$CC"' '"$CFLAGS"' $flags $extraflags "$1" '"$CLDFLAGS"' $ldflags -o "$installprefix/$out"
            ' --
    )

    printf "\n%s\n" \
        "Installing external C and C++ libraries:"

    (
        # shellcheck disable=SC2069
        printf "%s\0" \
            "$rootdir/lib/hedley/hedley.h" \
            "$rootdir/lib/pstreams/pstream.h" \
            "$rootdir/lib/imtui-xphyro/include/imtui/" \
            "$rootdir/lib/lyra-xphyro/include/lyra/" \
            | xargs -r0 -n 1 -P "${MAKE_JOBS:-0}" sh -c '
                fl="$1"
                printf "  %s -> %s\n" "${fl##"$rootdir/lib/"}" "$includeprefix/${fl##*/}" >&2
                cp -rf -t "$includeprefix" -- "$fl"
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

        find '.' -mindepth 1 -type f -not -path "./.*" \
                                     -not -name "tags" \
                                     -not -path "*/include/*" \
                                     -not -path "*/.archived/*" \
                                     -not -path "*/project/*" \
                                     -printf "%P\0" \
            | xargs -r0 -n 1 -P "$CPU_PROC" sh -c '
                '"$FUNC_PARSEFLAGS"'
                parseflags "$1"
                set -e
                out="${1%.cpp}"
                out="${out##*/}"
                case "$1" in
                    shared/*|*/shared/*)
                        out="$out.so"
                        installprefix="$libprefix"
                        extraflags=-shared
                        ;;
                    *)
                        installprefix="$binprefix"
                        extraflags=
                        ;;
                esac
                if [ -f "$installprefix/$out" ] && [ "$installprefix/$out" -nt "$1" ]; then
                    printf "  %s -> %s\n    %s\n" \
                        "$1" \
                        "$installprefix/$out" \
                        "Already up-to-date."
                    exit 0
                fi
                case "$1" in
                    */wrapper/*) out="wrapper/$out";;
                esac
                printf "  %s -> %s\n    Local compiler flags: %s\n    Local linker flags: %s\n" \
                    "$1" \
                    "$installprefix/$out" \
                    "$(printf "%s\n" "$flags" | tr -d "\n" | sed "s/^\s\+//;s/\s\+$//;s/\s\+/ /g")" \
                    "$(printf "%s\n" "$ldflags" | tr -d "\n" | sed "s/^\s\+//;s/\s\+$//;s/\s\+/ /g")"
                '"$CXX"' '"$CXXFLAGS"' $flags $extraflags "$1" '"$CXXLDFLAGS"' $ldflags -o "$installprefix/$out"
            ' --

        printf "\n%s\n" \
            "Installing C++ projects:"

        (
            cd project

            for dir in */; do
                (
                    cd "$dir"
                    name="${dir%/}"

                    [ -f ".no-runner" ] && [ -d "/home/runner" ] && {
                        printf "  %s\n" "Runner detected, not compiling $name."
                        exit 0
                    }

                    make -j"$CPU_PROC" "$name"
                    cp -f -t "$binprefix" -- "$name"
                ) &
            done
            wait
        )
    )

    printf "\n%s\n" \
        "Preparing to install Go programs:"

    (
        cd go

        printf "%s\n" \
            "  Using Go toolchain:" \
            "    Compiler: go" \
            "    Compiler version: $(go version | awk '{print $3}' | sed 's/^go//')" \
            "" \
            "Installing Go programs:"

        find '.' -mindepth 1 -type f -name "*.go" -print0 \
            | unbuffer="$unbuffer" xargs -r0 -n 1 -P "$CPU_PROC" sh -c '
                in="$1"
                out="${in##*/}"
                out="$binprefix/${out%.go}"
                printf "  %s -> %s\n" \
                    "$in" \
                    "$out"
                [ -f "$out" ] && [ "$out" -nt "$in" ] && {
                    printf "    %s\n" \
                        "Already up-to-date."
                    exit 0
                }
                $unbuffer go build -o "$out" "$in"
            ' --
    )

    printf "\n%s\n" \
        "Preparing to install Rust programs:"

    (
        cd rs

        printf "%s\n" \
            "  Using Rust toolchain:" \
            "    Compiler: cargo" \
            "    Compiler version: $(cargo --version | awk '{print $2}')" \
            "" \
            "Installing Rust programs:"

        find '.' -mindepth 1 -type f -name "Cargo.toml" -printf "%h\0" \
            | unbuffer="$unbuffer" xargs -r0 -n 1 -P "$CPU_PROC" sh -c '
                exe="${1##./}"
                cd "$exe"
                out="target/release/$exe"
                printf "  %s -> %s\n" \
                    "$exe" \
                    "$binprefix/$exe"
                tmp="$(mktemp)"
                trap "rm -f -- \"$tmp\"" EXIT INT TERM
                $unbuffer cargo build --release --all-features 2>&1 | tee -a -- "$tmp" \
                    && cp -f -t "$binprefix" -- "$out" \
                    || {
                        tail -n 1 -- "$tmp" \
                            | grep -E "error: package \`[a-zA-Z0-9_-] [v0-9.]+\` cannot be built because it requires rustc [v0-9.]+ or newer, while the currently active rustc version is [v0-9.]+" \
                            && exit 0
                    }
            ' --
    )

    printf "\n%s\n" \
        "Installing man pages:"

    (
        cd ../man

        command -v pandoc > /dev/null 2>&1 || {
            printf "%s\n" '  `pandoc` is not installed, cannot generate man pages.'
            exit 0
        }

        # shellcheck disable=SC2069
        find '.' -mindepth 1 -maxdepth 1 -type d -printf "%P\n" \
            | while IFS= read -r section; do
                mkdir -p -- "$manprefix/man$section" >&2
                export section
                find "$section" \( -type f -o -type l \) -print0 | xargs -r0 -n 1 -P "$IO_PROC" sh -c '
                    progname="$(basename -- "$1")"
                    manpath="$manprefix/man$section/${progname%.*}.$section"
                    printf "  %s -> %s\n" "$section/$progname" "$manpath" >&2
                    m4 -I"$rootdir" -DVERSION="$shorthash" -DTHIS="$1" "$1" \
                        | pandoc --standalone --to man -o "$manpath" >&2
                ' --
            done
    )

    printf "\n%s\n" \
        "Installing systemd services:"

    (
        cd ../systemd
        for dir in root user; do
            printf "  %s\n" "$dir:"

            case "$dir" in
                root) pfx=/usr/local/lib/systemd/system;;
                user)
                    if [ "$realuser" = "root" ]; then
                        pfx=/usr/local/lib/systemd/system
                    else
                        pfx="$realhome/.config/systemd/user"
                    fi
                    ;;
            esac

            [ -d "$pfx" ] || {
                printf "    %s\n" \
                    "Skipping installation of $dir systemd services as $pfx does not exist or is not a directory."
                continue
            }

            for fl in "$dir"/*; do
                flname="${fl##*/}"
                printf "    %s -> %s\n" "$flname" "$pfx/$flname"
                m4 -I"$rootdir" -DHOME="$realhome" -DBIN_PREFIX="$binprefix" \
                    -DDATA_PREFIX="$dataprefix" "$fl" > "$pfx/$flname"
                [ "$dir" = "user" ] && chown "$realuser:users" "$pfx/$flname"
            done
        done

        printf "\n%s\n" \
            "Reloading systemd services:"
        if isroot; then
            printf "  %s\n" "root"
            systemctl daemon-reload
        else
            printf "  %s\n" "$realuser"
            systemctl --user daemon-reload
        fi
    )

    printf "\n%s\n" \
        "Installing program resources:"

    (
        cd ../rsrc

        find '.' -mindepth 1 -type f -printf "%P\0" \
            | xargs -r0 -n 1 sh -c '
                printf "  %s -> %s\n" "$1" "$dataprefix/$1" >&2
            ' --
        rsync -abiPq -- ./ "$dataprefix/"
    )

    (
        cd js

        printf "\n%s\n" \
            "Building browser extensions:"

        (
            cd browser

            command -v web-ext > /dev/null 2>&1 || {
                printf "  %s\n" '`web-ext` is not installed, cannot build browser extensions.'
                exit 0
            }

            find '.' -mindepth 1 -maxdepth 1 -type d -not -name "build" -printf "%P\n" | while IFS= read -r addon; do
                (
                    cd "$addon"
                    printf "  %s -> %s\n" "$addon" "$rootdir/src/js/browser/build/$addon-$(jq .version < manifest.json | sed 's/^"//;s/"$//').zip"
                    web-ext --no-input --artifacts-dir=../build \
                        build --overwrite-dest > /dev/null
                )
            done
        )
    )

    {
        [ -f .installed ] && cat .installed
        find awk bash el py pl sh -mindepth 1 -type f \
            -executable \
            -not -path "*/.archived/*" \
            -not -path "*/wrapper/*" \
            -not -path "*/include/*" \
            -printf "$binprefix/%f\0"
        find awk bash el py pl sh -mindepth 1 -type f \
            -executable \
            -not -path "*/.archived/*" \
            -path "*/wrapper/*" \
            -printf "$binprefix/wrapper/%f\0"
        find bash sh -mindepth 1 -type f \
            -not -executable \
            -not -path "*/.archived/*" \
            -path "*/include/*" \
            -printf "$dataprefix/include/%f\0"
        find c/include -type f -printf "$includeprefix/xph/%f\0"
        find cpp/include -type f \
            -printf "$includeprefix/xph/%f\0"
        find c -mindepth 1 -type f \
            -not -path "c/.*" \
            -not -path "*/include/*" \
            -not -path "*/.archived/*" \
            -printf "$binprefix/%f\0" \
            | sed -z 's/\.c$//'
        find cpp -mindepth 1 -type f \
            -not -path "cpp/.*" \
            -not -path "*/include/*" \
            -not -path "*/.archived/*" \
            -not -path "*/project/*" \
            -printf "$binprefix/%f\0" \
            | sed -z 's/\.cpp$//'
        printf "%s\0" \
            "$includeprefix/hedley.h" \
            "$includeprefix/pstream.h"
        find "$includeprefix/imtui/" "$includeprefix/lyra/" \
            -print0
        find cpp/project -mindepth 1 -maxdepth 1 -type d \
            -printf "$binprefix/%f\0"
        find go -mindepth 1 -type f -name "*.go" \
            -printf "$binprefix/%f\0" \
            | sed -z 's/\.go$//'
        find rs -mindepth 1 -maxdepth 1 -type d \
            -printf "$binprefix/%f\0"
        find ../man -mindepth 1 -type f -printf "$manprefix/man%P\0" \
            | sed -z 's|/man\([0-9]\+\)\(.*\)\.md$|/man\1\2.\1|'
    } | sort -ruVz | sponge .installed
}

uninstall() {
    printf "%s\n" \
        "=========" \
        "UNINSTALL" \
        "========="

    [ -f .installed ] || {
        printf "%s\n" \
            "Nothing to do."
        return 0
    }

    printf "%s\n" \
        "Uninstalling all installed files."

    sort -ruVz .installed | xargs -r0 sh -c '
        for i; do
            if [ -d "$i" ]; then
                printf "  %s/\n" "$i"
                rmdir -- "$i"
            else
                printf "  %s\n" "$i"
                rm -f -- "$i"
            fi
        done
    ' --

    rm -f .installed
}

clean() {
    printf "%s\n" \
        "=====" \
        "CLEAN" \
        "====="

    (
        cd cpp/project || exit 1
        find '.' -mindepth 1 -maxdepth 1 -type d -printf "%P\n" \
            | while IFS= read -r project; do
                (cd "$project" && make clean) &
            done
        wait
    )

    (
        cd rs || exit 1
        find '.' -mindepth 1 -maxdepth 1 -type d -not -path "*/.archived/*" -not -name ".archived" -printf "%P\n" \
            | while IFS= read -r project; do
                (cd "$project" && cargo clean) &
            done
        wait
    )

    (
        cd js || exit 1
        cd browser || exit 1
        rm -rf -- build/
    )
}

# shellcheck disable=SC2120
unittest() {
    printf "%s\n" \
        "====" \
        "TEST" \
        "===="

    (
        cd ../test

        tmpout="$(mktemp)"
        tmperr="$(mktemp)"
        trap "rm -f -- '$tmpout' '$tmperr'; exit 1" INT EXIT HUP TERM

        printf "%s\n" \
            "Testing scripts and programs:"

        last_binary=
        # shellcheck disable=SC2154
        for test in *.sh; do
            binary="$(grep '^test_binary=' "$test")"
            binary="${binary#*=}"
            [ -z "$last_binary" ] || [ "$binary" != "$last_binary" ] && printf "\n  %s\n" "$binary"
            last_binary="$binary"

            (
                set +e
                . "./$test"

                printf "%s" \
                    "    $test_name: "

                command -v -- "$test_binary" > /dev/null 2>&1 || {
                    printf "%s\n" \
                        "Not installed, cannot test."
                    exit 0
                }

                command -v command_stdin > /dev/null 2>&1 || command_stdin() {
                    :
                }
                command_stdin | "$test_binary" "$@" 1> "$tmpout" 2> "$tmperr"
                cmdec="$?"

                failstr=

                command -v expected_stderr > /dev/null 2>&1 \
                    && { expected_stderr | cmp -s -- "$tmperr" || failstr="stderr is different. $failstr"; }

                command -v expected_stdout > /dev/null 2>&1 \
                    && { expected_stdout | cmp -s -- "$tmpout" || failstr="stdout is different. $failstr"; }

                command -v test_stderr > /dev/null 2>&1 \
                    && { test_stderr "$tmperr" || failstr="stderr is invalid. $failstr"; }

                command -v test_stdout > /dev/null 2>&1 \
                    && { test_stdout "$tmpout" || failstr "stdout is invalid. $failstr"; }

                if [ "$test_exit_code" = "nonzero" ]; then
                    [ "$cmdec" -eq 0 ] && failstr="Expected nonzero exit code, got $cmdec. $failstr"
                else
                    [ "$cmdec" -ne "$test_exit_code" ] && failstr="Expected exit code $test_exit_code, got $cmdec. $failstr"
                fi

                if [ -n "$failstr" ]; then
                    printf "%s\n" \
                        "Failed: $failstr"
                else
                    printf "%s\n" \
                        "Passed."
                fi
            )
        done | {
            case "$-" in
                *x*) sponge;;
                *) cat;;
            esac
        }

        rm -f -- "$tmpout" "$tmperr"
        trap - INT EXIT HUP TERM
    )
}

format() {
    printf "%s\n" \
        "======" \
        "FORMAT" \
        "======"

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
        "========"

    printf "%s\n" \
        "Preparing to analyse Bash and shell scripts and libraries:"

    if command -v shfmt > /dev/null 2>&1; then
        printf "%s\n" \
            "  Validator: shfmt" \
            "  Validator version: $(shfmt --version)"
        shfmt=shfmt
    else
        shfmt=true
    fi

    printf "%s\n" \
        "  Analyser: shellcheck" \
        "  Analyser version: $(shellcheck --version | head -n 2 | tail -n 1 | cut -d' ' -f2)" \
        "" \
        "Analysing Bash and shell scripts and libraries:"

    {
        find 'bash' -mindepth 1 -type f -executable \
            -not -path "*/.archived/*" -not -path "*/include/*" -print0
        find 'bash' -mindepth 1 -type f -iname "*.sh" \
            -not -path "*/.archived/*" -path "*/include/*" -print0
    } | unbuffer="$unbuffer" shfmt="$shfmt" xargs -r0 -n 16 -P "${MAKE_JOBS:-0}" sh -c '
            for fl; do
                (
                    printf "  %s\n" "$fl"
                    "$shfmt" -ln bash -- > /dev/null || {
                        printf "    %s\n" "Invalid syntax."
                        continue
                    }
                    $unbuffer shellcheck -- "$fl" 2>&1 | sed "s/^/    /"
                ) | sponge
            done
        ' --
    ec="$((ec | $?))"

    {
        find 'sh' -mindepth 1 -type f -executable \
            -not -path "*/.archived/*" -not -path "*/include/*" -print0
        find 'sh' -mindepth 1 -type f -iname "*.sh" \
            -not -path "*/.archived/*" -path "*/include/*" -print0
    } | unbuffer="$unbuffer" shfmt="$shfmt" xargs -r0 -n 1 -P "${MAKE_JOBS:-0}" sh -c '
            fl="$1"
            (
                printf "  %s\n" "$fl"
                "$shfmt" -p -- > /dev/null || {
                    printf "    %s\n" "Invalid syntax."
                    continue
                }
                $unbuffer shellcheck -- "$fl" 2>&1 | sed "s/^/    /"
            ) | sponge
        ' --
    ec="$((ec | $?))"

    printf "%s\n" \
        "" \
        "Preparing to analyse Python scripts:" \
        "  Analyser: pylint" \
        "  Analyser versions:" \
        "$(pylint --version | sed 's/^/    /')" \
        "" \
        "Analysing Python scripts:"

    find 'py' -mindepth 1 -type f -executable \
        -not -path "*/.archived/*" -not -path "*/include/*" -print0 \
        | xargs -r0 -n 1 -P "${MAKE_JOBS:-0}" sh -c '
            fl="$1"
            (
                printf "  %s\n" "$fl"
                pylint --rcfile py/.pylintrc --score=false -- "$fl" | sed "s/^/    /"
            ) | sponge
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
        "  Analyser complementary compiler version: $(clang --version | head -n 1 | cut -d' ' -f3)"

    printf "\n%s\n" \
        "Analysing C headers:"

    find 'c/include' -mindepth 1 -type f -iname "*.h" -not -path "*/project/*" -print0 \
        | CC="$CC" CFLAGS="$CFLAGS" CLDFLAGS="$CLDFLAGS" tmpout="$tmpout" xargs -r0 -n 1 -P "${MAKE_JOBS:-0}" sh -c '
            [ -n "$SHELL_VERBOSE" ] && [ "$SHELL_VERBOSE" -gt 0 ] && set -x
            fl="$1"
            (
                printf "  %s\n" "$fl"
                "$CC" $CFLAGS "$fl" $CLDFLAGS -o "$tmpout" \
                    || {
                        printf "    %s\n" "Does not compile."
                        exit 1
                    }
            ) | sponge
        ' --
    ec="$((ec | $?))"

    printf "\n%s\n" \
        "Analysing C source files:"

    find 'c' -mindepth 1 -type f -iname "*.c" -not -path "*/project/*" -print0 \
        | m="$m" v="$v" view="$view" CC="$CC" CFLAGS="$CFLAGS" CLDFLAGS="$CLDFLAGS" tmpout="$tmpout" xargs -r0 -n 1 sh -c '
                '"$FUNC_PARSEFLAGS"'
                parseflags "$1"
                printf "  %s\n    Local compiler flags: %s\n    Local linker flags: %s\n" \
                    "$1" \
                    "$(printf "%s\n" "$flags" | tr -d "\n" | sed "s/^\s\+//;s/\s\+$//;s/\s\+/ /g")" \
                    "$(printf "%s\n" "$ldflags" | tr -d "\n" | sed "s/^\s\+//;s/\s\+$//;s/\s\+/ /g")"
                err="$(
                    eval '"scan-build \
                        -analyze-headers --status-bugs \$v \$view \
                        -maxloop \"\$m\" -no-failure-reports \
                        --use-cc=\"\$(command -v \"\$CC\")\" \
                        \"\$CC\" $CFLAGS $flags \"\$1\" $CLDFLAGS $ldflags -o \"\$tmpout\" 2>&1 > /dev/null"'
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
            "  Linker flags: $(printf "%s\n" "$CXXLDFLAGS" | tr -d '\n' | sed 's/^\s\+//;s/\s\+$//;s/\s\+/ /g')"

        printf "\n%s\n" \
            "Analysing C++ headers:"

            find 'cpp/include' -mindepth 1 -type f -iname "*.hpp" -not -path "*/project/*" -print0 \
                | CXX="$CXX" CXXFLAGS="$CXXFLAGS" CXXLDFLAGS="$CXXLDFLAGS" tmpout="$tmpout" xargs -r0 -n 1 -P "${MAKE_JOBS:-0}" sh -c '
                    [ -n "$SHELL_VERBOSE" ] && [ "$SHELL_VERBOSE" -gt 0 ] && set -x
                    fl="$1"
                    (
                        printf "  %s\n" "$fl"
                        "$CXX" $CXXFLAGS "$fl" $CXXLDFLAGS -o "$tmpout" \
                            || {
                                printf "    %s\n" "Does not compile."
                                exit 1
                            }
                    ) | sponge
                ' --
            ec="$((ec | $?))"
    fi

    printf "\n%s\n" \
        "Analysis of C++23 source files is not fully supported by this analyser. Cannot analyse C++ source files."

    printf "%s\n" \
        "" \
        "Preparing to analyse C and C++ header and source files:" \
        "  Analyser: cppcheck" \
        "  Analyser version: $(cppcheck --version | cut -d' ' -f2)" \
        ""

    printf "%s\n" "Analysing C header and source files:"
    (cd c && eval "
        find . \( -iname '*.c' -o -iname '*.h' \) -not -path '*/project/*' -print0 \
            | xargs -r0 $unbuffer cppcheck \
                --enable=warning,style,performance,portability,information,missingInclude \
                --quiet --inline-suppr -j\"$MAX_PROC\" \
                --force --error-exitcode=1 --max-ctu-depth=16 \
                --platform=unix64 --std=c99 -Iinclude \
                $CPPCHECK_SUPPRESS $C_CPPCHECK_SUPPRESS" 2>&1 \
        | sed 's/^/  /')
    ec="$((ec | $?))"

    printf "\n%s\n" "Analysing C++ header and source files:"
    (cd cpp && eval "
        find . \( -iname '*.cpp' -o -iname '*.hpp' \) -not -path '*/project/*' -print0 \
            | xargs -r0 $unbuffer cppcheck \
                --enable=warning,style,performance,portability,information,missingInclude \
                --quiet --inline-suppr -j\"$MAX_PROC\" \
                --force --error-exitcode=1 --max-ctu-depth=16 \
                --platform=unix64 --std=c++23 -Iinclude \
                $CPPCHECK_SUPPRESS $CXX_CPPCHECK_SUPPRESS" 2>&1 \
        | sed 's/^/  /')
    ec="$((ec | $?))"

    printf "%s\n" \
        "" \
        "Preparing to analyse Rust projects:" \
        "  Analyser: cargo" \
        "  Analyser version: $(cargo --version)" \
        ""

    printf "%s\n" "Analysing Rust projects:"
    (
        cd rs || exit 1
        find '.' -mindepth 1 -maxdepth 1 -type d -not -path "*/.archived/*" -not -name ".archived" -printf "%P\n" \
            | while IFS= read -r project; do
                printf "  %s\n" "$project"
                cd "$project" || {
                    printf "    Failed analysis\n"
                    continue
                }
                cargo clippy --all-targets --all-features 2>&1
                cd .. || exit 1
            done
    )
    ec="$((ec | $?))"

    rm -f -- "$tmpout"
}

spell() {
    printf "%s\n" \
        "=====" \
        "SPELL" \
        "====="

    (
        cd .. || exit 1

        printf "%s\n" \
            "Checking for typos:"
        find '.' -type f -not -name "tags" \
                         -not -path "*/.git/*" \
                         -not -path "*/rsrc/*" \
                         -not -path "*/lib/*" \
                         -not -path "*/.archived/*" \
                         -not -path "*/rs/*/target/*" \
                         -not -path "*/src/tests/*/out" \
            -print0 \
            | sort -z \
            | xargs -r0 \
                codespell \
                    --builtin "clear,rare,informal" \
                    -L "ans,ba,crate,doas,erformance,creat,hist,ond,ommit,tage" \
                    --
    )
}

stats() {
    printf "%s\n" \
        "==========" \
        "STATISTICS" \
        "=========="

    tmp="$(mktemp)"
    trap 'rm -f "$tmp"' INT QUIT TERM EXIT

    files="$(find -P 'awk' -mindepth 1 -type f -executable)"
    awkn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    awkbytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    awkloc="$(wc -l < "$tmp")"
    awksloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find -P 'bash' -mindepth 1 -type f -executable)"
    bashn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    bashbytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    bashloc="$(wc -l < "$tmp")"
    bashsloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find -P 'c' -mindepth 1 -type f \( -iname "*.c" -o -iname "*.h" \); printf "%s\n" "$rootdir/.hooks/has-std-sh.c")"
    cn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    cbytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    cloc="$(wc -l < "$tmp")"
    csloc="$(sed '/^\s*$/d' < "$tmp" | wc -l)"

    files="$(find -P 'cpp' -mindepth 1 -type f \( -iname "*.cpp" -o -iname "*.hpp" \))"
    cppn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    cppbytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    cpploc="$(wc -l < "$tmp")"
    cppsloc="$(sed '/^\s*$/d' < "$tmp" | wc -l)"

    files="$(find -P 'el' -mindepth 1 -type f -executable)"
    eln="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    elbytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    elloc="$(wc -l < "$tmp")"
    elsloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find -P 'go' -mindepth 1 -type f -name "*.go")"
    gon="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    gobytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    goloc="$(wc -l < "$tmp")"
    gosloc="$(sed '/^\s*$/d;/^\s*\/\//d' < "$tmp" | wc -l)"

    files="$(find -P 'js' -mindepth 1 -type f -iname "*.js")"
    jsn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    jsbytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    jsloc="$(wc -l < "$tmp")"
    jssloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find -P '../man' -mindepth 1 -type f)"
    mann="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    manbytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    manloc="$(wc -l < "$tmp")"
    mansloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find -P 'pl' -mindepth 1 -type f -executable)"
    pln="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    plbytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    plloc="$(wc -l < "$tmp")"
    plsloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find -P 'py' -mindepth 1 -type f -executable)"
    pyn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    pybytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    pyloc="$(wc -l < "$tmp")"
    pysloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find -P 'rs' -mindepth 1 -type f -iname "*.rs")"
    rsn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    rsbytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    rsloc="$(wc -l < "$tmp")"
    rssloc="$(sed '/^\s*$/d' < "$tmp" | wc -l)"

    files="$(
        find -P 'sh' -mindepth 1 -type f -executable
        find -P '../test' -mindepth 1 -type f -iname "*.sh"
        printf "%s\n" "$realexecpath" "$rootdir/.hooks/pre-commit"
    )"
    shn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    shbytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    shloc="$(wc -l < "$tmp")"
    shsloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find -P '../systemd' -mindepth 1 -type f -iname "*.service")"
    systemdn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    systemdbytes="$(tr -d '[:space:]' < "$tmp" | wc -m)"
    systemdloc="$(wc -l < "$tmp")"
    systemdsloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    totaln="$((awkn + bashn + cn + cppn + eln + gon + jsn + mann + pln + pyn + rsn + shn + systemdn))"
    totalbytes="$((awkbytes + bashbytes + cbytes + cppbytes + elbytes + gobytes + jsbytes + manbytes + plbytes + pybytes + rsbytes + shbytes + systemdbytes))"
    totalloc="$((awkloc + bashloc + cloc + cpploc + elloc + goloc + jsloc + manloc + plloc + pyloc + rsloc + shloc + systemdloc))"
    totalsloc="$((awksloc + bashsloc + csloc + cppsloc + elsloc + gosloc + jssloc + mansloc + plsloc + pysloc + rssloc + shsloc + systemdsloc))"

    printf "%s\n" \
        "Git:" \
        "  First commit:  $(git log --reverse | head -n 3 | tail -n 1 | sed 's/^Date:\s*//')" \
        "  Last commit:   $(git log | head -n 3 | tail -n 1 | sed 's/^Date:\s*//')" \
        "  Total commits: $(git rev-list --all --count)" \
        "" \
        "Number of Source Files:" \
        "  Awk:        $awkn" \
        "  Bash:       $bashn" \
        "  C:          $cn" \
        "  C++:        $cppn" \
        "  execline:   $eln" \
        "  Go:         $gon" \
        "  JavaScript: $jsn" \
        "  groff:      $mann" \
        "  Perl:       $pln" \
        "  Python:     $pyn" \
        "  Rust:       $rsn" \
        "  shell:      $shn" \
        "  systemd:    $systemdn" \
        "  Total:      $totaln" \
        "" \
        "Non-Whitespace Characters:" \
        "  Awk:        $awkbytes" \
        "  Bash:       $bashbytes" \
        "  C:          $cbytes" \
        "  C++:        $cppbytes" \
        "  execline:   $elbytes" \
        "  Go:         $gobytes" \
        "  JavaScript: $jsbytes" \
        "  groff:      $manbytes" \
        "  Perl:       $plbytes" \
        "  Python:     $pybytes" \
        "  Rust:       $rsbytes" \
        "  shell:      $shbytes" \
        "  systemd:    $systemdbytes" \
        "  Total:      $totalbytes" \
        "" \
        "Lines of Code:" \
        "  Awk:        $awkloc" \
        "  Bash:       $bashloc" \
        "  C:          $cloc" \
        "  C++:        $cpploc" \
        "  execline:   $elloc" \
        "  Go:         $goloc" \
        "  JavaScript: $jsloc" \
        "  groff:      $manloc" \
        "  Perl:       $plloc" \
        "  Python:     $pyloc" \
        "  Rust:       $rsloc" \
        "  shell:      $shloc" \
        "  systemd:    $systemdloc" \
        "  Total:      $totalloc" \
        "" \
        "Source Lines of Code:" \
        "  Awk:        $awksloc" \
        "  Bash:       $bashsloc" \
        "  C:          $csloc" \
        "  C++:        $cppsloc" \
        "  execline:   $elsloc" \
        "  Go:         $gosloc" \
        "  JavaScript: $jssloc" \
        "  groff:      $mansloc" \
        "  Perl:       $plsloc" \
        "  Python:     $pysloc" \
        "  Rust:       $rssloc" \
        "  shell:      $shsloc" \
        "  systemd:    $systemdsloc" \
        "  Total:      $totalsloc"

    if command -v -- scc > /dev/null 2>&1; then
        printf "\n"
        scc --cocomo-project-type=semi-detached --sort=lines --wide --avg-wage=85000
    fi
}

[ -n "$SHELL_VERBOSE" ] && [ "$SHELL_VERBOSE" -gt 0 ] && {
    set -x
    printf "PARENT { "
    cat -- "/proc/$PPID/cmdline" | xargs -r0 printf '"%s", '
    printf "}\nTHIS { "
    printf '"%s", ' "$@"
    printf "}\n"
}
set -e

if [ "$#" -eq 0 ]; then
    pid="$PPID"
    while :; do
        case "$(basename -- "$(head -n 1 -z -- "/proc/$pid/cmdline")")" in
            sh|bash|dash|ksh|zsh)
                pid="$(ps -o ppid= -p "$pid" | tr -d '[:space:]')"
                [ -z "$pid" ] && exit 1
                ;;
            make) break;;
            *) exit 1;;
        esac
    done

    if sh -c ': > /dev/tty 2>&1'; then
        redirect="> /dev/tty 2>&1"
    else
        unset redirect
    fi

    xargs -r0 sh -c '
        [ -n "$SHELL_VERBOSE" ] && [ "$SHELL_VERBOSE" -gt 0 ] && set -x
        shift
        while [ "$#" -gt 0 ]; do
            case "$1" in
                --) shift; printf "%s\0" "$@"; exit 0;;
                -*) shift;;
                *) printf "%s\0" "$1"; shift;;
            esac
        done
    ' -- < "/proc/$pid/cmdline" | eval "setsid xargs -r0 \"\$0\" $redirect"
    exec kill -TERM "$PPID"
else
    printf "argv:"
    printf " %s" "$@"
    printf "\n"
fi

realexecpath="$(realpath -- "$0")" # may be used in tests

rootdir="$PWD"
shorthash="$(git rev-parse --short HEAD)"
cd src/ || exit 1
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
    realuser="$(getrealuser)"
    realhome="$(getent passwd "$realuser" | cut -d: -f6)"
else
    prefix="$HOME/.local"
    realuser="$(id -un)"
    realhome="$HOME"
fi
binprefix="$prefix/bin"
libprefix="$prefix/lib/scripts"
includeprefix="$prefix/include"
manprefix="$prefix/share/man"
dataprefix="$prefix/share/scripts"

export prefix
export binprefix
export libprefix
export includeprefix
export manprefix
export dataprefix

mkdir -p -- "$prefix" \
    "$binprefix" \
    "$libprefix" \
    "$includeprefix" \
    "$manprefix" \
    "$dataprefix"
[ -d "$binprefix" ] \
    && [ -d "$libprefix" ] \
    && [ -d "$includeprefix" ] \
    && [ -d "$manprefix" ] \
    && [ -d "$dataprefix" ]

unset o g ndebug view v
while getopts "O:g:Vvm:" i; do
    case "$i" in
        O)
            case "$OPTARG" in
                ""|0|1|2|3|g|s|fast) o="$OPTARG";;
                *) exit 1;;
            esac
            ;;
        g)
            case "$OPTARG" in
                "") ndebug="-DNDEBUG";;
                g) g="-g";;
                gdb) g="-ggdb";;
                *) exit 1;;
            esac
            ;;
        V) view="--view";;
        v) v="$v -v";;
        m)
            if [ -z "$OPTARG" ] || [ "$OPTARG" -le 0 ]; then
                m=4
            else
                m="$OPTARG"
            fi
            ;;
        *) exit 1;;
    esac
done
shift "$((OPTIND - 1))"

CC="gcc"
C_INCLUDE_FLAGS="-I'$includeprefix' -I'$rootdir/lib/hedley'"
CFLAGS="-O${o:-3} $g $ndebug -std=c99 -pedantic \
       -Wall -Wextra -Werror -Wabi=11 \
       -Wno-unused-parameter -Wno-unused-result -Wswitch-default \
       -Wimplicit-fallthrough=5 -Wno-sign-compare \
       -Wfloat-equal -Wdouble-promotion -Wstringop-overflow=4 \
       -Winline -Wpadded -Wpacked -Wdisabled-optimization \
       -Wshadow-compatible-local \
       -ffp-contract=on -fassociative-math -ffast-math -flto \
       $C_INCLUDE_FLAGS"
CLDFLAGS=""
C_INCLUDE_PATH="$rootdir/lib/hedley:$C_INCLUDE_PATH"
export C_INCLUDE_PATH

CXX="g++"
CXX_INCLUDE_FLAGS="-I'$includeprefix' -I'$rootdir/lib/hedley' -I'$rootdir/lib/pstreams' -I'$rootdir/lib/lyra-xphyro/include'"
case "$(awk -F= '/^ID=/ {print $2}' /etc/os-release)" in
    fedora) CXX_OS_FLAGS=" -fPIC ";;
    *) CXX_OS_FLAGS="";;
esac
CXXFLAGS="-O${o:-3} $g $ndebug -std=c++2b \
          -Wall -Wextra -Werror -Wabi=19 \
          -Wswitch-default \
          -Wimplicit-fallthrough=5 -Wstringop-overflow=4 \
          -Wfloat-equal -Wdouble-promotion -Wdisabled-optimization \
          -Wstrict-null-sentinel -Wold-style-cast -Wsign-promo \
          -Wshadow-compatible-local \
          -ffp-contract=on -fassociative-math -ffast-math -flto \
          -fpermissive -fvtable-verify=none $CXX_OS_FLAGS \
          $CXX_INCLUDE_FLAGS"
CXXLDFLAGS=""
CPLUS_INCLUDE_PATH="$rootdir/lib/hedley:$rootdir/lib/pstreams:$CPLUS_INCLUDE_PATH"
export CPLUS_INCLUDE_PATH

C_CPPCHECK_SUPPRESS="--suppress=variableScope"
CXX_CPPCHECK_SUPPRESS="--suppress=noExplicitConstructor"
CPPCHECK_SUPPRESS="--suppress=unmatchedSuppression:\* \
                   --suppress=missingIncludeSystem \
                   --suppress=ConfigurationNotChecked \
                   --suppress=invalidPrintfArgType_uint \
                   --suppress=shadowFunction \
                   --suppress=shadowVariable \
                   --suppress=unusedFunction \
                   --suppress=unusedStructMember \
                   --suppress=uninitvar"

ec=0
cancompilecpp="$(cancompilecpp "$CXX")"

if [ -n "$MAKE_JOBS" ]; then
    MAX_PROC="$MAKE_JOBS"
    CPU_PROC="$MAKE_JOBS"
    IO_PROC="$MAKE_JOBS"
else
    MAX_PROC="$(nproc)"
    if [ "$GITHUB_ACTIONS" = true ]; then
        CPU_PROC="$MAX_PROC"
        IO_PROC="$((MAX_PROC * 4))"
    else
        CPU_PROC="$(nproc --ignore=2)"
        IO_PROC="$((MAX_PROC * 4))"
    fi
fi

all

for cmd; do
    case "$cmd" in
        index) index;;
        install) install;;
        uninstall) uninstall;;
        clean) clean;;
        test) unittest;;
        format) format;;
        analyse) analyse;;
        spell) spell;;
        stats) stats;;
        *) logerrq "Unkown target given: %s." "$cmd";;
    esac
done

exit "${ec:-0}"
