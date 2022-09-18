execname="${0##*/}"

std_logerrq() {
    for i; do
        printf "%s: %s\n" "$execname" "$i" >&2
    done
    exit "${ec:-1}"
}
