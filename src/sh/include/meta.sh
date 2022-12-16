[ -z "${HEADER_SCRIPTS_META_+x}" ] && {
    HEADER_SCRIPTS_META_=

    std_is_set() {
        for __i; do
            eval '[ -z "${'"$__i"'+x}" ]' && return 1
        done

        return 0
    }

    std_ensure_defined() {
        __ec=0
        for __i; do
            command -v "$__i" > /dev/null 2>&1 || eval "$__i() :" || __ec=1
        done

        return "$__ec"
    }
}
