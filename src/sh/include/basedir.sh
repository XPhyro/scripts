. std.sh

# shellcheck disable=SC2154

[ -z "${HEADER_SCRIPTS_SH_CONFIG_+x}" ] && {
    HEADER_SCRIPTS_SH_CONFIG_=

    . execinfo.sh
    . io.sh

    std_xdg_config_home="${XDG_CONFIG_HOME:-"$HOME/.config"}"
    std_xdg_cache_home="${XDG_CACHE_HOME:-"$HOME/.cache"}"

    std_global_config_home="$std_xdg_config_home/scripts"
    std_global_cache_home="$std_xdg_cache_home/scripts"

    std_config_home="$std_global_config_home/$std_execname"
    std_cache_home="$std_global_cache_home/$std_execname"

    { [ -d "$std_xdg_config_home" ] || mkdir -- "$std_xdg_config_home"; } \
        && { [ -d "$std_xdg_cache_home" ] || mkdir -- "$std_xdg_cache_home"; } \
        || std_logerr "basedir.sh: system does not conform to XDG Base Directory Specification"

    { [ -d "$std_global_config_home" ] || mkdir -- "$std_global_config_home"; } \
        && { [ -d "$std_global_cache_home" ] || mkdir -- "$std_global_cache_home"; } \
        || std_logerr "basedir.sh: cannot set up global directories"

    { [ -d "$std_config_home" ] || mkdir -- "$std_config_home"; } \
        && { [ -d "$std_cache_home" ] || mkdir -- "$std_cache_home"; } \
        || std_logerr "basedir.sh: cannot set up script directories"
}
