[ -z "${HEADER_SCRIPTS_SH_SUDO_+x}" ] && {
    HEADER_SCRIPTS_SH_SUDO_=

    . io.sh
    . meta.sh

    if [ "$(id -u)" = 0 ]; then
        std_is_root() {
            return 0
        }
    else
        std_is_root() {
            return 1
        }
    fi

    if std_has_command doas; then
        std_sudo() {
            doas "$@"
        }
    elif std_has_command sudo; then
        std_sudo() {
            sudo "$@"
        }
    else
        std_logerrq "no sudo-like command found"
        exit 1
    fi

    std_as_root() {
        if std_is_root; then
            "$@"
        else
            std_sudo -- "$@"
        fi
    }

    unset __std_parent_user
    # Get the first non-$USER user in parent process tree.
    # Cannot track grandparent process tree if double-forked.
    std_get_parent_user() {
        std_is_set __std_parent_user && {
            printf "%s" "$__std_parent_user"
            return 0
        }

        std_is_set SUDO_USER && {
            __std_parent_user="$SUDO_USER"
            printf "%s" "$__std_parent_user"
            return 0
        }

        ppid="$PPID"
        self="$(id -un)"
        __parent_user="$(
            while ppid="$(ps -o ppid= -p "$ppid" | tr -d '[:space:]')"; do
                owner="$(ps -o user= -p "$ppid")" || {
                    printf "%s" "$self"
                    break
                }
                [ "$owner" != "$self" ] && {
                    printf "%s" "$owner"
                    break
                }
            done
        )"

        [ -z "$__parent_user" ] && {
            unset __parent_user
            return 1
        }

        __std_parent_user="$__parent_user"
        unset __parent_user
        printf "%s" "$__std_parent_user"
        return 0
    }

    std_get_parent_user_id() {
        id -u "$(std_get_parent_user)"
    }

    std_get_parent_displays() {
        w | awk -vuser="$(std_get_parent_user)" '{
                if ($1 == user && $7 == "xinit") {
                    for (i = 8; i < NF; i++) {
                        if ($i ~ /:[0-9]+/)
                            print $i
                    }
                }
            }'
    }

    std_get_first_parent_display() {
        if std_is_set SUDO_DISPLAY; then
            printf "%s" "$SUDO_DISPLAY"
        else
            std_get_parent_displays | head -n 1
        fi
    }

    std_get_parent_dbus_address() {
        if std_is_set SUDO_DBUS_SESSION_BUS_ADDRESS; then
            printf "%s" "$SUDO_DBUS_SESSION_BUS_ADDRESS"
        else
            printf "%s" "unix:path=/run/user/$(std_get_parent_user_id)/bus"
        fi
    }

    std_sudo_env() {
        std_sudo -u "$(std_get_parent_user)" -H \
            DISPLAY=":0" \
            DBUS_SESSION_BUS_ADDRESS="$(std_get_parent_dbus_address)" \
            -- "$@"
    }
}
