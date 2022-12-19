[ -z "${HEADER_SCRIPTS_IO_+x}" ] && {
    HEADER_SCRIPTS_IO_=

    . execinfo.sh

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
