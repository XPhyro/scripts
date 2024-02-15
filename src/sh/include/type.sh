. std.sh

[ -z "${HEADER_SCRIPTS_SH_TYPE_+x}" ] && {
    HEADER_SCRIPTS_SH_TYPE_=

    std_is_digit() {
        for __i; do
            [ "$__i" -eq "$__i" ] 2> /dev/null || return 1
        done
        return 0
    }

    std_is_number() {
        for __i; do
            printf "%s\n" "$__i" | grep -Exq '[-+]?([0-9]+|[0-9]+\.|\.[0-9]+|[0-9]+\.[0-9]+)' || return 1
        done
        return 0
    }

    std_is_unsigned_number() {
        for __i; do
            printf "%s\n" "$__i" | grep -Exq '([0-9]+|[0-9]+\.|\.[0-9]+|[0-9]+\.[0-9]+)' || return 1
        done
        return 0
    }

    std_is_signed_number() {
        for __i; do
            printf "%s\n" "$__i" | grep -Exq '[-+]([0-9]+|[0-9]+\.|\.[0-9]+|[0-9]+\.[0-9]+)' || return 1
        done
        return 0
    }
}
