# shellcheck disable=SC2034

[ -z "${HEADER_SCRIPTS_SH_COLOR_+x}" ] && {
    HEADER_SCRIPTS_SH_COLOR_=

    std_clr() {
        printf "$2$1$std_nc"
    }

    std_clrl() {
        std_clr "$1\n" "$2"
    }

    std_black='\033[0;30m'
    std_red='\033[0;31m'
    std_green='\033[0;32m'
    std_orange='\033[0;33m'
    std_blue='\033[0;34m'
    std_purple='\033[0;35m'
    std_cyan='\033[0;36m'
    std_lgray='\033[0;37m'
    std_dgray='\033[1;30m'
    std_lred='\033[1;31m'
    std_lgreen='\033[1;32m'
    std_yellow='\033[1;33m'
    std_lblue='\033[1;34m'
    std_lpurple='\033[1;35m'
    std_lcyan='\033[1;36m'
    std_white='\033[1;37m'
    std_nc='\033[0m'

    std_ws="    "
    std_wss="$WS$WS"
    std_wsss="$WSS$WS"
    std_wssss="$WSSS$WS"
}
