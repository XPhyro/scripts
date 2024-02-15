. std.sh

[ -z "${HEADER_SCRIPTS_SH_UTIL_+x}" ] && {
    HEADER_SCRIPTS_SH_UTIL_=

    std_cd() {
        cd "$@" || exit "${ec:-1}"
    }
}
