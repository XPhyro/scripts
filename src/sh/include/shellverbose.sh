[ -z "${HEADER_SCRIPTS_SH_EVALVERBOSE_+x}" ] && {
    HEADER_SCRIPTS_SH_EVALVERBOSE_=

    [ -n "$SHELL_VERBOSE" ] && [ "$SHELL_VERBOSE" -gt 0 ] 2> /dev/null && {
        set -x
        SHELL_VERBOSE="$((SHELL_VERBOSE - 1))"
        export SHELL_VERBOSE
    }
    : # zero exit code
}
