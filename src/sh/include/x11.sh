# shellcheck disable=SC2034

[ -z "${HEADER_SCRIPTS_SH_X11_+x}" ] && {
    HEADER_SCRIPTS_SH_X11_=

    . io.sh

    # `xdg-open` does a lot more than this if it cannot determine the keycode,
    # but we'll just assume this is enough.
    case "$XDG_CURRENT_DESKTOP" in
        # same names as `xdg-open`
        Cinnamon|X-Cinnamon) std_x11_desktop="cinnamon";;
        ENLIGHTENMENT) std_x11_desktop="enlightenment";;
        GNOME*) std_x11_desktop="gnome";; # matches GNOME, GNOME-Classic:GNOME, or GNOME-Flashback:GNOME
        KDE) std_x11_desktop="kde";;
        DEEPIN|Deepin|deepin) std_x11_desktop="deepin";;
        LXDE) std_x11_desktop="lxde";;
        LXQt) std_x11_desktop="lxqt";;
        MATE) std_x11_desktop="mate";;
        XFCE) std_x11_desktop="xfce";;
        X-Generic) std_x11_desktop="generic";;

        # following names are not used by `xdg-open` and are not standard
        awesome) std_x11_desktop="awesome";;
        bspwm) std_x11_desktop="bspwm";;
        dwm) std_x11_desktop="dwm";;

        *) std_x11_desktop="unknown";;
    esac

    std_x11_prop() {
        __id="$1"
        __expected_lc="${2:-0}"
        shift 2
        __out="$(
            {
                [ -n "$__id" ] && __std_printf "%s\0" -id "$__id"
                __std_printf "%s\0" "$@"
            } | xargs -0 xprop | tail -n +2
        )"
        if [ -z "$__out" ]; then
            __i=0
            while [ "$__i" -lt "$__expected_lc" ]; do
                __std_printf '""\n'
                : "$((__i += 1))"
            done
        else
            __std_printf "%s\n" "$__out"
        fi
    }

    std_x11_evalable_prop() {
        {
            std_x11_prop "$1" 2 '\n$0\n$1\n' WM_CLASS
            std_x11_prop "$1" 1 '\n$0\n' WM_NAME
        } | sed -n 's/'\''/'\'\"\'\"\''/g;s/^"\(.*\)"$/'\''\1'\''/;1s/^/CLASS=/;2s/^/INSTANCE=/;3s/<field not available>//;3s/^/NAME=/;1,3p'
    }

    std_x11_eval_prop() {
        eval "$(std_x11_evalable_prop "$1")"
    }

    std_x11_get_current_keymap() {
        xkb-switch # TODO: add fallbacks in case xkb-switch is not installed
    }
}
