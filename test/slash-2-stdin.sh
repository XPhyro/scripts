test_binary=slash
test_name="Pipe"
test_exit_code=0

set --

command_stdin() {
    printf "%s\n" \
        'XXXXXX' \
        '/XXXXXX/YYYYYY' \
        '////XXXXXX////' \
        "$HOME///"
}

expected_stdout() {
    printf "%s\n" \
        'XXXXXX' \
        '/XXXXXX/YYYYYY' \
        '/XXXXXX' \
        "$HOME"
}

expected_stderr() {
    :
}
