test_binary=slash
test_name="Pipe"
test_exit_code=0

set --

test_stdin() {
    printf "%s\n" \
        'XXXXXX' \
        '/XXXXXX/YYYYYY' \
        '////XXXXXX////' \
        "$HOME///"
}

test_stdout() {
    printf "%s\n" \
        'XXXXXX' \
        '/XXXXXX/YYYYYY' \
        '/XXXXXX' \
        "$HOME"
}

test_stderr() {
    :
}
