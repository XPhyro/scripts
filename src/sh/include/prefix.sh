# shellcheck disable=SC2034

[ -z "${HEADER_SCRIPTS_PREFIX_+x}" ] && {
    HEADER_SCRIPTS_PREFIX_=

    . execinfo.sh
    . io.sh

    case "$std_execpath" in
        /*) std_can_query_prefix=1;;
        *) std_can_query_prefix=0;;
    esac

    std_prefix="${std_execpath%%/bin/"${std_execpath##*/}"}"
    std_binprefix="$std_prefix/bin"
    std_includeprefix="$std_prefix/include"
    std_manprefix="$std_prefix/share/man"
    std_dataprefix="$std_prefix/share/scripts"

    std_assert_can_query_prefix() {
        [ "$std_can_query_prefix" -eq 0 ] && std_logferrq 'cannot query prefix. $0 is %s.\n' "$0"
    }
}
