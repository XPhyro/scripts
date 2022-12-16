[ -z "${HEADER_SCRIPTS_META_+x}" ] && {
    HEADER_SCRIPTS_META_=

    std_ensure_defined() {
        __ec=0
        for i; do
            command -v "$i" > /dev/null 2>&1 || eval "$i() :" || __ec=1
        done

        return "$__ec"
    }
}
