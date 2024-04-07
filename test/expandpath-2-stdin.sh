test_binary=expandpath
test_name="Pipe"
test_exit_code=0

set --

test_stdin() {
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

test_stdout() {
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

test_stderr() {
    :
}
