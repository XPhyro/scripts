test_binary=slash
test_name="Arguments"
test_exit_code=0

set -- \
    XXXXXX \
    /XXXXXX/YYYYYY \
    ////XXXXXX//// \
    "$HOME"///

command_stdin() {
    :
}

expected_stdout() {
    printf "%s\n" \
        'XXXXXX' \
        '/XXXXXX/YYYYYY' \
        '/XXXXXX' \
        "$HOME"/
}

expected_stderr() {
    :
}
