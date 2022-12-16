[ -z "${HEADER_SCRIPTS_UTILITY_+x}" ] && {
    HEADER_SCRIPTS_UTILITY_=

    # don't prepend with std_, these should be global variables
    execpath="${0}"
    execname="${execpath##*/}"
}
