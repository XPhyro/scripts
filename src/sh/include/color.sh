[ -z "${HEADER_SCRIPTS_COLOR_+x}" ] && {
    HEADER_SCRIPTS_COLOR_=

    clr() {
        printf "$2$1$NC"
    }

    clrl() {
        clr "$1\n" "$2"
    }

    BLACK='\033[0;30m'
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    ORANGE='\033[0;33m'
    BLUE='\033[0;34m'
    PURPLE='\033[0;35m'
    CYAN='\033[0;36m'
    LGRAY='\033[0;37m'
    DGRAY='\033[1;30m'
    LRED='\033[1;31m'
    LGREEN='\033[1;32m'
    YELLOW='\033[1;33m'
    LBLUE='\033[1;34m'
    LPURPLE='\033[1;35m'
    LCYAN='\033[1;36m'
    WHITE='\033[1;37m'
    NC='\033[0m'

    WS="    "
    WSS="$WS$WS"
    WSSS="$WSS$WS"
    WSSSS="$WSSS$WS"
}
