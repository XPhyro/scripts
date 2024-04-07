test_binary=unexpandpath
test_name="Arguments"
test_exit_code=0

set -- \
    XXXXXX \
    XXXXXX/YYYYYY \
    ~root \
    ~root/XXXXXX \
    /home/XXXXXX \
    /home/XXXXXX/YYYYYY

test_stdin() {
    :
}

test_stdout() {
    printf "%s\n" \
        'XXXXXX' \
        'XXXXXX/YYYYYY' \
        '~root' \
        '~root/XXXXXX' \
        '/home/XXXXXX' \
        '/home/XXXXXX/YYYYYY'
}

test_stderr() {
    :
}
