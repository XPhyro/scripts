[ -z "${HEADER_SCRIPTS_SH_SUDO_+x}" ] && {
    HEADER_SCRIPTS_SH_SUDO_=

    . meta.sh

    if [ "$(id -u)" = 0 ]; then
        __sudo_func() {
            "$@"
        }
    elif std_has_command doas; then
        __sudo_func() {
            doas -- "$@"
        }
    elif std_has_command sudo; then
        __sudo_func() {
            sudo -- "$@"
        }
    else
        __sudo_func() {
            "$@"
        }
    fi

    std_sudo() {
        __sudo_func "$@"
    }
}
