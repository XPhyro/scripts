[ -z "${HEADER_SCRIPTS_SH_CONFIG_+x}" ] && {
    HEADER_SCRIPTS_SH_CONFIG_=

    . execinfo.sh
    . io.sh

    std_xdg_config_home="${XDG_CONFIG_HOME:-"$HOME/.config"}"
    std_global_config_home="$std_xdg_config_home/scripts"
    # shellcheck disable=SC2154
    std_config_home="$std_global_config_home/$std_execname"

    [ -d "$std_xdg_config_home" ] || mkdir -- "$std_xdg_config_home" \
        || std_logerr "config.sh: system does not obey XDG Base Directory Specification"
    [ -d "$std_global_config_home" ] || mkdir -- "$std_global_config_home" \
        || std_logerr "config.sh: cannot set up global config directory"
    # assumption: if the script includes this header, it wants/needs a config sub-directory
    [ -d "$std_config_home" ] || mkdir -- "$std_config_home" \
        || std_logerr "config.sh: cannot set up script config directory"
}
