[ -z "${HEADER_SCRIPTS_SH_SUDO_+x}" ] && {
    HEADER_SCRIPTS_SH_SUDO_=

    . meta.sh

    if [ "$(id -u)" = 0 ]; then
        std_sudo() {
            "$@"
        }
    elif std_has_command doas; then
        std_sudo() {
            doas -- "$@"
        }
    elif std_has_command sudo; then
        std_sudo() {
            sudo -- "$@"
        }
    else
        std_sudo() {
            "$@"
        }
    fi
}
