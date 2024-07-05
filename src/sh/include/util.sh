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

    std_ret_to_bool() {
        if [ "$?" -eq 0 ]; then
            printf 1
        else
            printf 0
        fi
    }

    std_invert_ret() {
        if [ "$?" -eq 0 ]; then
            return 1
        else
            return 0
        fi
    }
}
