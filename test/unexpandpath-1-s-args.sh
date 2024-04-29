test_binary=unexpandpath
test_name="Short Arguments"
test_exit_code=0

set -- \
    -s \
    XXXXXX \
    XXXXXX/YYYYYY \
    ~ \
    ~/XXXXXX \
    ~/XXXXXX/ \
    ~/XXXXXX/YYYYYY \
    ~/XXXXXX//////YYYYYY \
    /home/XXXXXX \
    /home/XXXXXX/YYYYYY

command_stdin() {
    :
}

expected_stdout() {
    printf "%s\n" \
        'XXXXXX' \
        'XXXXXX/YYYYYY' \
        '~' \
        '~/XXXXXX' \
        '~/XXXXXX/' \
        '~/XXXXXX/YYYYYY' \
        '~/XXXXXX/YYYYYY' \
        '/home/XXXXXX' \
        '/home/XXXXXX/YYYYYY'
}

expected_stderr() {
    :
}
