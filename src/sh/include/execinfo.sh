[ -z "${HEADER_SCRIPTS_EXECINFO_+x}" ] && {
    HEADER_SCRIPTS_EXECINFO_=

    # don't prepend with std_, these should be global variables
    execpath="${0}"
    execname="${execpath##*/}"
}
