[ -z "${HEADER_SCRIPTS_SH_IO_+x}" ] && {
    HEADER_SCRIPTS_SH_IO_=

    . execinfo.sh
    . meta.sh

    printf() {
        ! std_is_set IN_HEADER_SPACE_ || ! std_is_set SUPPRESS_PRINTF_ && command printf "$@"
    }

    unset std_stdin_backup
    __std_is_stdin_backed_up=0

    __std_backup_stdin_impl() {
        [ "$__std_is_stdin_backed_up" -ne 0 ] && return 0
        std_stdin_backup="$(mktemp -t std_backup_stdin.XXXXXXXXXX)"
        # not sure if `cp` introduces security issues, so use `cat >`
        cat - > "$std_stdin_backup"
    }

    std_backup_stdin() {
        if [ "$#" -eq 0 ]; then
            __std_backup_stdin_impl
        else
            __std_backup_stdin_impl
            for __i; do
                # not sure if `cp` introduces security issues, so use `cat >`
                cat "$std_stdin_backup" > "$__i"
            done
        fi
    }

    std_is_stdin_empty() {
        [ "$(fionread)" -eq 0 ] # TODO: find a shell-only solution for this
    }

    std_is_stdin_tty() {
        [ -t 0 ]
    }

    std_logf() {
        __fmt="$1"
        shift
        # shellcheck disable=SC2154
        printf "%s: $__fmt" "$execname" "$@"
    }

    std_log() {
        for __i; do
            printf "%s: %s\n" "$execname" "$__i"
        done
    }

    std_lograw() {
        for __i; do
            printf "%s\n" "$__i"
        done
    }

    std_logfraw() {
        printf "$@"
    }

    std_logfq() {
        __fmt="$1"
        shift
        printf "%s: $__fmt" "$execname" "$@"
        exit "${ec:-1}"
    }

    std_logq() {
        for __i; do
            printf "%s: %s\n" "$execname" "$__i"
        done
        exit "${ec:-1}"
    }

    std_lograwq() {
        for __i; do
            printf "%s\n" "$__i"
        done
        exit "${ec:-1}"
    }

    std_logfrawq() {
        printf "$@"
        exit "${ec:-1}"
    }

    std_logferr() {
        __fmt="$1"
        shift
        printf "%s: $__fmt" "$execname" "$@" >&2
    }

    std_logerr() {
        for __i; do
            printf "%s: %s\n" "$execname" "$__i" >&2
        done
    }

    std_logerrraw() {
        for __i; do
            printf "%s\n" "$__i" >&2
        done
    }

    std_logferrraw() {
        printf "$@" >&2
    }

    std_logferrq() {
        __fmt="$1"
        shift
        printf "%s: $__fmt" "$execname" "$@" >&2
        exit "${ec:-1}"
    }

    std_logerrq() {
        for __i; do
            printf "%s: %s\n" "$execname" "$__i" >&2
        done
        exit "${ec:-1}"
    }

    std_logerrrawq() {
        for __i; do
            printf "%s\n" "$__i" >&2
        done
        exit "${ec:-1}"
    }

    std_logferrrawq() {
        printf "$@" >&2
        exit "${ec:-1}"
    }
}
