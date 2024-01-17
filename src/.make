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
        || [ "$("$CXX" --version | head -n 1 | sed 's/\s*[0-9]*$//;s/.* \([0-9]\+\)\.[0-9]\+\.[0-9]\+/\1/')" -ge 12 ] 2> /dev/null; then
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

index() {
    printf "%s\n" \
        "=====" \
        "INDEX" \
        "=====" \
        "Indexing all source files."

    ctags -R --c++-kinds=+p --fields=+iaS --extras=+q .
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
                                     -not -path "*/.archived/*" \
                                     -printf "%P\0" \
            | xargs -r0 -n 1 -P "$(nproc --ignore=2)" sh -c '
                '"$FUNC_PARSEFLAGS"'
                parseflags "$1"
                set -e
                out="${1%.c}"
                out="${out##*/}"
                case "$1" in
                    shared/*|*/shared/*)
                        out="$out.so"
                        installprefix="$libprefix"
                        ;;
                    *)
                        installprefix="$binprefix"
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
                '"$CC"' '"$CFLAGS"' $flags -shared "$1" '"$CLDFLAGS"' $ldflags -o "$installprefix/$out" \
                    && printf "\0%s\0" "$installprefix/$out" >> ../.installed
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
                                     -not -path "*/.archived/*" \
                                     -printf "%P\0" \
            | xargs -r0 -n 1 -P "$(nproc --ignore=2)" sh -c '
                '"$FUNC_PARSEFLAGS"'
                parseflags "$1"
                set -e
                out="${1%.cpp}"
                out="${out##*/}"
                case "$1" in
                    shared/*|*/shared/*)
                        out="$out.so"
                        installprefix="$libprefix"
                        ;;
                    *)
                        installprefix="$binprefix"
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
                '"$CXX"' '"$CXXFLAGS"' $flags -shared "$1" '"$CXXLDFLAGS"' $ldflags -o "$installprefix/$out" \
                    && printf "\0%s\0" "$installprefix/$out" >> ../.installed
            ' --
    )

    printf "\n%s\n" \
        "Installing C libraries:"

    (
        cd c/include
        [ -d "$includeprefix/xph" ] || mkdir -- "$includeprefix/xph"

        # shellcheck disable=SC2069
        find '.' -type f -printf "%P\0" | xargs -r0 -n 1 -P 0 sh -c '
            fl="$1"
            printf "\0%s\0" "$includeprefix/xph/$fl"
            printf "  %s -> %s\n" "$fl" "$includeprefix/xph/$fl" >&2
            cp -f -t "$includeprefix/xph" -- "$fl"
        ' -- 2>&1 >> "$rootdir/src/.installed"
    )

    printf "\n%s\n" \
        "Installing C++ libraries:"

    (
        cd cpp/include

        # shellcheck disable=SC2069
        find '.' -type f -printf "%P\0" | xargs -r0 -n 1 -P 0 sh -c '
            fl="$1"
            printf "\0%s\0" "$includeprefix/xph/$fl"
            printf "  %s -> %s\n" "$fl" "$includeprefix/xph/$fl" >&2
            cp -f -t "$includeprefix/xph" -- "$fl"
        ' -- 2>&1 >> "$rootdir/src/.installed"
    )

    printf "\n%s\n" \
        "Installing external C and C++ libraries:"

    (
        # shellcheck disable=SC2069
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
        "Preparing to install Rust programs:"

    (
        cd rs

        printf "%s\n" \
            "  Using Rust toolchain:" \
            "    Compiler: cargo" \
            "    Compiler version: $(cargo --version | awk '{print $2}')" \
            "" \
            "Installing Rust programs:"

        # TODO: don't write to .installed in xargs
        find '.' -mindepth 1 -type f -name "Cargo.toml" -printf "%h\0" \
            | unbuffer="$unbuffer" xargs -r0 -n 1 -P "$(nproc --ignore=2)" sh -c '
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
                    && printf "\0%s\0" "$binprefix/$exe" >> ../../.installed \
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
        cd man

        command -v pandoc > /dev/null 2>&1 || {
            printf "%s\n" '  `pandoc` is not installed, cannot generate man pages.'
            exit 0
        }

        # shellcheck disable=SC2069
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

    printf "\n%s\n" \
        "Installing systemd services:"

    (
        cd systemd
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
                printf "\0%s\0" "$pfx/$flname"
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
                printf "\0%s\0" "$dataprefix/$1"
            ' -- >> ../src/.installed
        rsync -abiPq -- ./ "$dataprefix/"
    )
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

    xargs -r0 -n 2 sh -c 'printf "%s\0" "$2"' -- < .installed | xargs -r0 sh -c '
        printf "  %s\n" "$@"
        rm -f -- "$@"
    ' --

    rm -f .installed
}

clean() {
    printf "%s\n" \
        "=====" \
        "CLEAN" \
        "====="

    (
        cd rs || exit 1
        find '.' -mindepth 1 -maxdepth 1 -type d -not -path "*/.archived/*" -not -name ".archived" -printf "%P\n" \
            | while IFS= read -r project; do
                cd "$project" || continue
                cargo clean
                cd ..
            done
    )
}

unittest() {
    printf "%s\n" \
        "====" \
        "TEST" \
        "===="

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
    } | unbuffer="$unbuffer" shfmt="$shfmt" xargs -r0 -n 16 -P 0 sh -c '
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
    } | unbuffer="$unbuffer" shfmt="$shfmt" xargs -r0 -n 1 -P 0 sh -c '
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
        | xargs -r0 -n 1 -P 0 sh -c '
            fl="$1"
            (
                printf "  %s\n" "$fl"
                pylint --rcfile py/.pylintrc --score=false -- "$fl" | sed "s/^/    /"
            ) | sponge
        ' --

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
        | CC="$CC" CFLAGS="$CFLAGS" CLDFLAGS="$CLDFLAGS" tmpout="$tmpout" xargs -r0 -n 1 -P 0 sh -c '
            fl="$1"
            (
                printf "  %s\n" "$fl"
                "$CC" $CFLAGS "$fl" $CLDFLAGS -o "$tmpout" \
                    || printf "    %s\n" "Does not compile."
            ) | sponge
        ' --

    printf "\n%s\n" \
        "Analysing C source files:"

    find 'c' -mindepth 1 -type f -iname "*.c" -print0 \
        | m="$m" v="$v" view="$view" CC="$CC" CFLAGS="$CFLAGS" CLDFLAGS="$CLDFLAGS" tmpout="$tmpout" xargs -r0 -n 1 sh -c '
                '"$FUNC_PARSEFLAGS"'
                parseflags "$1"
                printf "  %s\n    Local compiler flags: %s\n    Local linker flags: %s\n" \
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
                | CXX="$CXX" CXXFLAGS="$CXXFLAGS" CXXLDFLAGS="$CXXLDFLAGS" tmpout="$tmpout" xargs -r0 -n 1 -P 0 sh -c '
                    fl="$1"
                    (
                        printf "  %s\n" "$fl"
                        "$CXX" $CXXFLAGS "$fl" $CXXLDFLAGS -o "$tmpout" \
                            || printf "    %s\n" "Does not compile."
                    ) | sponge
                ' --
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
        find . \( -iname '*.c' -o -iname '*.h' \) -print0 \
            | xargs -r0 $unbuffer cppcheck \
                --enable=warning,style,performance,portability,information,missingInclude \
                --quiet --inline-suppr -j\"$(nproc)\" \
                --force --error-exitcode=1 --max-ctu-depth=16 \
                --platform=unix64 --std=c99 -Iinclude \
                $CPPCHECK_SUPPRESS $C_CPPCHECK_SUPPRESS" 2>&1 \
        | sed 's/^/  /')
    ec="$((ec | $?))"

    printf "\n%s\n" "Analysing C++ header and source files:"
    (cd cpp && eval "
        find . \( -iname '*.cpp' -o -iname '*.hpp' \) -print0 \
            | xargs -r0 $unbuffer cppcheck \
                --enable=warning,style,performance,portability,information,missingInclude \
                --quiet --inline-suppr -j\"$(nproc)\" \
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
            "Checking files and files in directories for spelling mistakes:"
        find '.' -type f -not -path "*/.git/*" \
                         -not -path "*/lib/*" \
                         -not -path "*/.archived/*" \
                         -not -path "*/rs/*/target/*" \
                         -not -name "tags" \
            -print0 \
            | xargs -r0 \
                codespell \
                    -e \
                    --builtin "clear,rare,informal" \
                    -L "ans,ba,erformance,crate,doas,ond,hist" \
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

    files="$(find 'awk' -mindepth 1 -type f -executable)"
    awkn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    awkloc="$(wc -l < "$tmp")"
    awksloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find 'bash' -mindepth 1 -type f -executable)"
    bashn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    bashloc="$(wc -l < "$tmp")"
    bashsloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find 'el' -mindepth 1 -type f -executable)"
    eln="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    elloc="$(wc -l < "$tmp")"
    elsloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find 'py' -mindepth 1 -type f -executable)"
    pyn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    pyloc="$(wc -l < "$tmp")"
    pysloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find 'pl' -mindepth 1 -type f -executable)"
    pln="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    plloc="$(wc -l < "$tmp")"
    plsloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find 'sh' -mindepth 1 -type f -executable)"
    shn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    shloc="$(wc -l < "$tmp")"
    shsloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    files="$(find 'rs' -mindepth 1 -type f -iname "*.rs")"
    rsn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    rsloc="$(wc -l < "$tmp")"
    rssloc="$(sed '/^\s*$/d' < "$tmp" | wc -l)"

    files="$(find 'c' -mindepth 1 -type f \( -iname "*.c" -o -iname "*.h" \))"
    cn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    cloc="$(wc -l < "$tmp")"
    csloc="$(sed '/^\s*$/d' < "$tmp" | wc -l)"

    files="$(find 'cpp' -mindepth 1 -type f \( -iname "*.cpp" -o -iname "*.hpp" \))"
    cppn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    cpploc="$(wc -l < "$tmp")"
    cppsloc="$(sed '/^\s*$/d' < "$tmp" | wc -l)"

    files="$(find 'systemd' -mindepth 1 -type f -iname "*.service")"
    systemdn="$(printf "%s\n" "$files" | wc -l)"
    printf "%s\n" "$files" | xargs -r -d '\n' cat -- > "$tmp"
    systemdloc="$(wc -l < "$tmp")"
    systemdsloc="$(sed '/^\s*$/d;/^\s*#/d' < "$tmp" | wc -l)"

    totaln="$((bashn + eln + pyn + pln + shn + cn + cppn + systemdn))"
    totalloc="$((bashloc + elloc + pyloc + plloc + shloc + cloc + cpploc + systemdloc))"
    totalsloc="$((bashsloc + elsloc + pysloc + plsloc + shsloc + csloc + cppsloc + systemdsloc))"

    printf "%s\n" \
        "Git:" \
        "  First commit:  $(git log --reverse | head -n 3 | tail -n 1 | sed 's/^Date:\s*//')" \
        "  Last commit:   $(git log | head -n 3 | tail -n 1 | sed 's/^Date:\s*//')" \
        "  Total commits: $(git rev-list --all --count)" \
        "" \
        "Number of Source Files:" \
        "  Awk:      $awkn" \
        "  Bash:     $bashn" \
        "  execline: $eln" \
        "  Python:   $pyn" \
        "  Perl:     $pln" \
        "  shell:    $shn" \
        "  Rust:     $rsn" \
        "  C:        $cn" \
        "  C++:      $cppn" \
        "  systemd:  $systemdn" \
        "  Total:    $totaln" \
        "" \
        "Lines of Code:" \
        "  Awk:      $awkloc" \
        "  Bash:     $bashloc" \
        "  execline: $elloc" \
        "  Python:   $pyloc" \
        "  Perl:     $plloc" \
        "  shell:    $shloc" \
        "  Rust:     $rsloc" \
        "  C:        $cloc" \
        "  C++:      $cpploc" \
        "  systemd:  $systemdloc" \
        "  Total:    $totalloc" \
        "" \
        "Source Lines of Code:" \
        "  Awk:      $awksloc" \
        "  Bash:     $bashsloc" \
        "  execline: $elsloc" \
        "  Python:   $pysloc" \
        "  Perl:     $plsloc" \
        "  shell:    $shsloc" \
        "  Rust:     $rssloc" \
        "  C:        $csloc" \
        "  C++:      $cppsloc" \
        "  systemd:  $systemdsloc" \
        "  Total:    $totalsloc"
}

[ -n "$SHELL_VERBOSE" ] && [ "$SHELL_VERBOSE" -gt 0 ] && set -x
set -e

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
C_INCLUDE_FLAGS="-Iinclude -I'$rootdir/lib/hedley'"
CFLAGS="-O${o:-3} $g $ndebug -std=c99 -pedantic \
       -Wall -Wextra -Werror -Wabi=11 \
       -Wno-unused-parameter -Wno-unused-result -Wswitch-default \
       -Wimplicit-fallthrough=5 -Wno-sign-compare \
       -Wfloat-equal -Wdouble-promotion -Wjump-misses-init -Wstringop-overflow=4 \
       -Wold-style-definition -Winline -Wpadded -Wpacked -Wdisabled-optimization \
       -Wshadow-compatible-local \
       -ffp-contract=on -fassociative-math -ffast-math -flto \
       $C_INCLUDE_FLAGS"
CLDFLAGS=""
C_INCLUDE_PATH="$PWD/c/include:$rootdir/lib/hedley"
export C_INCLUDE_PATH

CXX="g++"
CXX_INCLUDE_FLAGS="-Iinclude -I'$rootdir/src/c/include' \
                   -I'$rootdir/lib/hedley' -I'$rootdir/lib/pstreams'"
CXXFLAGS="-O${o:-3} $g $ndebug -std=c++2b \
          -Wall -Wextra -Werror -Wabi=18 \
          -Wswitch-default \
          -Wimplicit-fallthrough=5 -Wstringop-overflow=4 \
          -Wfloat-equal -Wdouble-promotion -Wdisabled-optimization \
          -Wstrict-null-sentinel -Wold-style-cast -Wsign-promo \
          -Wshadow-compatible-local \
          -ffp-contract=on -fassociative-math -ffast-math -flto \
          -fpermissive -fvtable-verify=none \
          $CXX_INCLUDE_FLAGS"
CXXLDFLAGS=""
CPLUS_INCLUDE_PATH="$PWD/cpp/include:$PWD/c/include:$rootdir/lib/hedley:$rootdir/lib/pstreams"
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

case "$cmd" in
    index) :;;
    *) all;;
esac

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
    *) logerrq "Unkown target given: %s." "$1";;
esac

exit "${ec:-0}"
