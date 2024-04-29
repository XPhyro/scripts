test_binary=expandpath
test_name="Arguments"
test_exit_code=0

set -- \
    "" \
    "~" \
    "~/" \
    "~/XXXXXX" \
    "~/XXXXXX/YYYYYY" \
    "~XXXXXX" \
    "~XXXXXX/" \
    "/root" \
    "/root/XXXXXX" \
    "XXXXXX" \
    "/XXXXXX/"

command_stdin() {
    :
}

expected_stdout() {
    printf "%s\n" \
        "" \
        ~ \
        ~/ \
        ~/XXXXXX \
        ~/XXXXXX/YYYYYY \
        "~XXXXXX" \
        "~XXXXXX/" \
        /root \
        /root/XXXXXX \
        XXXXXX \
        /XXXXXX/
}

expected_stderr() {
    :
}
