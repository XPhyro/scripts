# shellcheck disable=SC2034

[ -z "${HEADER_SCRIPTS_SH_EXECINFO_+x}" ] && {
    HEADER_SCRIPTS_SH_EXECINFO_=

    std_execpath="$0"
    std_execname="${std_execpath##*/}"

    # also define as global variables
    execpath="$std_execpath"
    execname="$std_execname"
}
