[ -z "${HEADER_SCRIPTS_UTIL_+x}" ] && {
    HEADER_SCRIPTS_UTIL_=

    std_cd() {
        cd "$@" || exit 1
    }
}
