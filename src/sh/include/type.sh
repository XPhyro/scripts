. std.sh

[ -z "${HEADER_SCRIPTS_SH_TYPE_+x}" ] && {
    HEADER_SCRIPTS_SH_TYPE_=

    . io.sh

    std_is_digit() {
        for __i; do
            [ "$__i" -eq "$__i" ] 2> /dev/null || return 1
        done
        return 0
    }

    std_assert_digit() {
        for __j; do
            std_is_digit "$__j" || std_logerrq "expected digit, got '$__j'"
        done
    }

    std_is_number() {
        for __i; do
            printf "%s\n" "$__i" | grep -Exq '[-+]?([0-9]+|[0-9]+\.|\.[0-9]+|[0-9]+\.[0-9]+)' || return 1
        done
        return 0
    }

    std_assert_number() {
        for __j; do
            std_is_number "$__j" || std_logerrq "expected number, got '$__j'"
        done
    }

    std_is_unsigned_number() {
        for __i; do
            printf "%s\n" "$__i" | grep -Exq '([0-9]+|[0-9]+\.|\.[0-9]+|[0-9]+\.[0-9]+)' || return 1
        done
        return 0
    }

    std_assert_unsigned_number() {
        for __j; do
            std_is_unsigned_number "$__j" || std_logerrq "expected unsigned number, got '$__j'"
        done
    }

    std_is_signed_number() {
        for __i; do
            printf "%s\n" "$__i" | grep -Exq '[-+]([0-9]+|[0-9]+\.|\.[0-9]+|[0-9]+\.[0-9]+)' || return 1
        done
        return 0
    }

    std_assert_signed_number() {
        for __j; do
            std_is_signed_number "$__j" || std_logerrq "expected signed number, got '$__j'"
        done
    }
}
