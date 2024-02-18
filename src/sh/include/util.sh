. std.sh

[ -z "${HEADER_SCRIPTS_SH_UTIL_+x}" ] && {
    HEADER_SCRIPTS_SH_UTIL_=

    std_cd() {
        cd "$@" || exit "${ec:-1}"
    }

    std_trap_once() {
        cmd="$1"
        shift
        trap "trap - $*; $cmd" "$@"
    }
}
