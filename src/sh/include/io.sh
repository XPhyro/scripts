[ -z "${HEADER_SCRIPTS_SH_IO_+x}" ] && {
    HEADER_SCRIPTS_SH_IO_=

    . execinfo.sh
    . meta.sh

    __std_printf() {
        std_is_set SUPPRESS_PRINTF_ || printf "$@"
    }

    unset std_stdin_backup
    __std_is_stdin_backed_up=0

    __std_backup_stdin_impl() {
        [ "$__std_is_stdin_backed_up" -ne 0 ] && return 0
        std_stdin_backup="$(mktemp -t std_backup_stdin.XXXXXXXXXX)"
        # not sure if `cp` introduces security issues, so use `cat >`
        cat - > "$std_stdin_backup"
    }

    std_read_char() {
        dd bs=1 count=1 2> /dev/null
    }

    std_read_char_tty() {
        stty -icanon -echo < /dev/tty > /dev/tty
        char="$(dd bs=1 count=1 < /dev/tty 2> /dev/null)" || return 1
        stty icanon echo < /dev/tty > /dev/tty
        printf "%s" "$char"
    }

    std_yesno() {
        [ "$#" -gt 0 ] && printf "$@"
        case "$(std_read_char_tty)" in
            Y|y) return 0;;
            *) return 1;;
        esac
    }

    std_wait_char() {
        std_read_char > /dev/null
    }

    std_wait_char_tty() {
        std_read_char_tty > /dev/null
    }

    std_read_str() {
        IFS= read -r __str || return 1
        printf "%s" "$__str"
    }

    std_wait_enter() {
        std_read_str > /dev/null
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
        __std_printf "%s: $__fmt" "$execname" "$@"
    }

    std_log() {
        for __i; do
            __std_printf "%s: %s\n" "$execname" "$__i"
        done
    }

    std_lograw() {
        for __i; do
            __std_printf "%s\n" "$__i"
        done
    }

    std_logfraw() {
        __std_printf "$@"
    }

    std_logfq() {
        __fmt="$1"
        shift
        __std_printf "%s: $__fmt" "$execname" "$@"
        exit "${ec:-1}"
    }

    std_logq() {
        for __i; do
            __std_printf "%s: %s\n" "$execname" "$__i"
        done
        exit "${ec:-1}"
    }

    std_lograwq() {
        for __i; do
            __std_printf "%s\n" "$__i"
        done
        exit "${ec:-1}"
    }

    std_logfrawq() {
        __std_printf "$@"
        exit "${ec:-1}"
    }

    std_logferr() {
        __fmt="$1"
        shift
        __std_printf "%s: $__fmt" "$execname" "$@" >&2
    }

    std_logerr() {
        for __i; do
            __std_printf "%s: %s\n" "$execname" "$__i" >&2
        done
    }

    std_logerrraw() {
        for __i; do
            __std_printf "%s\n" "$__i" >&2
        done
    }

    std_logferrraw() {
        __std_printf "$@" >&2
    }

    std_logferrq() {
        __fmt="$1"
        shift
        __std_printf "%s: $__fmt" "$execname" "$@" >&2
        exit "${ec:-1}"
    }

    std_logerrq() {
        for __i; do
            __std_printf "%s: %s\n" "$execname" "$__i" >&2
        done
        exit "${ec:-1}"
    }

    std_logerrrawq() {
        for __i; do
            __std_printf "%s\n" "$__i" >&2
        done
        exit "${ec:-1}"
    }

    std_logferrrawq() {
        __std_printf "$@" >&2
        exit "${ec:-1}"
    }
}
