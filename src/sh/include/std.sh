[ -z "${HEADER_SCRIPTS_SH_STD_+x}" ] && {
    HEADER_SCRIPTS_SH_STD_=

    if [ -n "$SHELL_VERBOSE" ] && [ "$SHELL_VERBOSE" -gt 0 ] 2> /dev/null; then
        set -x
        SHELL_VERBOSE="$((SHELL_VERBOSE - 1))"
        export SHELL_VERBOSE
    fi

    if [ -z "$STD_RUNTIME_DEPTH" ]; then
        STD_RUNTIME_DEPTH=1
    elif [ "$STD_RUNTIME_DEPTH" -eq "$STD_RUNTIME_DEPTH" ]; then
        : "$((STD_RUNTIME_DEPTH += 1))"
    else
        printf '$STD_RUNTIME_PATH (%s) is not empty and not a valid integer. Do not edit it, or make sure it is valid. Resetting it to 1.\n' "$STD_RUNTIME_DEPTH" >&2
        STD_RUNTIME_DEPTH=1
    fi
}
