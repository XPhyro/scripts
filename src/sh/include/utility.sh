[ -z "${HEADER_SCRIPTS_UTILITY_+x}" ] && {
    HEADER_SCRIPTS_UTILITY_=

    execname="${0##*/}"

    std_is_set() {
        for i; do
            eval "[ -z "\${$i+x}" ]" && return 1
        done

        return 0
    }

    std_logf() {
        fmt="$1"
        shift
        printf "%s: $fmt" "$execname" "$@"
    }

    std_log() {
        for i; do
            printf "%s: %s\n" "$execname" "$i"
        done
    }

    std_lograw() {
        for i; do
            printf "%s\n" "$i"
        done
    }

    std_logfq() {
        fmt="$1"
        shift
        printf "%s: $fmt" "$execname" "$@"
        exit "${ec:-1}"
    }

    std_logq() {
        for i; do
            printf "%s: %s\n" "$execname" "$i"
        done
        exit "${ec:-1}"
    }

    std_lograwq() {
        for i; do
            printf "%s\n" "$i"
        done
        exit "${ec:-1}"
    }

    std_logferr() {
        fmt="$1"
        shift
        printf "%s: $fmt" "$execname" "$@" >&2
    }

    std_logerr() {
        for i; do
            printf "%s: %s\n" "$execname" "$i" >&2
        done
    }

    std_logerrraw() {
        for i; do
            printf "%s\n" "$i" >&2
        done
    }

    std_logferrq() {
        fmt="$1"
        shift
        printf "%s: $fmt" "$execname" "$@" >&2
        exit "${ec:-1}"
    }

    std_logerrq() {
        for i; do
            printf "%s: %s\n" "$execname" "$i" >&2
        done
        exit "${ec:-1}"
    }

    std_logerrrawq() {
        for i; do
            printf "%s\n" "$i" >&2
        done
        exit "${ec:-1}"
    }
}
