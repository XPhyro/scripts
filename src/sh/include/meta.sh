[ -z "${HEADER_SCRIPTS_SH_META_+x}" ] && {
    HEADER_SCRIPTS_SH_META_=

    std_is_set() {
        for __i; do
            eval '[ -z "${'"$__i"'+x}" ]' && return 1
        done

        return 0
    }

    std_ensure_defined() {
        __ec=0
        for __i; do
            command -v -- "$__i" > /dev/null 2>&1 || eval "$__i() :" || __ec=1
        done

        return "$__ec"
    }

    std_has_command() {
        for __i; do
            command -v -- "$__i" > /dev/null 2>&1 || return 1
        done

        return 0
    }

    std_is_function() {
        for __i; do
            [ "$(type "$__i" 2>&1)" = "$__i is a shell function" ] || return 1
        done

        return 0
    }

    std_try_choose_command() {
        for __i; do
            command -v -- "$__i" > /dev/null 2>&1 && {
                printf "%s" "$__i"
                return 0
            }
        done

        return 1
    }
}
