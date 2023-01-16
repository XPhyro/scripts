[ -z "${HEADER_SCRIPTS_SH_SUDO_+x}" ] && {
    HEADER_SCRIPTS_SH_SUDO_=

    . meta.sh

    if [ "$(id -u)" = 0 ]; then
        unset __sudo_cmd
    elif std_has_command doas; then
        __sudo_cmd="doas"
    elif std_has_command sudo; then
        __sudo_cmd="sudo"
    else
        unset __sudo_cmd
    fi

    std_sudo() {
        $__sudo_cmd -- "$@"
    }
}
