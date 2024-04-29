test_binary=expandpath
test_name="Pipe"
test_exit_code=0

set --

command_stdin() {
    printf "%s\n" \
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
