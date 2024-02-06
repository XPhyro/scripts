[ -z "${HEADER_SCRIPTS_SH_TYPE_+x}" ] && {
    HEADER_SCRIPTS_SH_TYPE_=

    std_is_digit() {
        for __i; do
            [ "$__i" -eq "$__i" ] 2> /dev/null || return 1
        done
        return 0
    }
}
