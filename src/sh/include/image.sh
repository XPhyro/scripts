. std.sh

[ -z "${HEADER_SCRIPTS_SH_IMAGE_+x}" ] && {
    HEADER_SCRIPTS_SH_IMAGE_=

    std_get_image_aspect() {
        __image="$1"

        __aspect_expr="$(exiftool -s -s -s -ImageWidth -ImageHeight -- "$__image" | chomp | tr '\n' '/')"
        printf "scale=2; %s\n" "$__aspect_expr" | bc
    }
}
