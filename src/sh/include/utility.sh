[ -z "${HEADER_SCRIPTS_UTILITY_+x}" ] && {
    HEADER_SCRIPTS_UTILITY_=

    execname="${0##*/}"

    std_is_set() {
        for i; do
            eval "[ -z "\${$i+x}" ]" && return 1
        done

        return 0
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
}
