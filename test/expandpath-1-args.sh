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

test_stdin() {
    :
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
