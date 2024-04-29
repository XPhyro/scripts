test_binary=unexpandpath
test_name="Short Pipe"
test_exit_code=0

set -- \
    -s

command_stdin() {
    printf "%s\n" \
        'XXXXXX' \
        'XXXXXX/YYYYYY' \
        '~' \
        '~/XXXXXX' \
        '~/XXXXXX/' \
        '~root' \
        '~root/XXXXXX' \
        '~/XXXXXX/YYYYYY' \
        '~/XXXXXX//////YYYYYY' \
        '/home/XXXXXX' \
        '/home/XXXXXX/YYYYYY'
}

expected_stdout() {
    printf "%s\n" \
        'XXXXXX' \
        'XXXXXX/YYYYYY' \
        '~' \
        '~/XXXXXX' \
        '~/XXXXXX/' \
        '~root' \
        '~root/XXXXXX' \
        '~/XXXXXX/YYYYYY' \
        '~/XXXXXX/YYYYYY' \
        '/home/XXXXXX' \
        '/home/XXXXXX/YYYYYY'
}

expected_stderr() {
    :
}
