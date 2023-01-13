# shellcheck disable=SC2034

[ -z "${HEADER_SCRIPTS_PREFIX_+x}" ] && {
    HEADER_SCRIPTS_PREFIX_=

    std_prefix="${0%%/bin/"${0##*/}"}"
    std_binprefix="$std_prefix/bin"
    std_includeprefix="$std_prefix/include"
    std_manprefix="$std_prefix/share/man"
    std_dataprefix="$std_prefix/share/scripts"
}
