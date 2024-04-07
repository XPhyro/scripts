test_binary=slash
test_name="Arguments"
test_exit_code=0

set -- \
    XXXXXX \
    /XXXXXX/YYYYYY \
    ////XXXXXX//// \
    "$HOME"///

test_stdin() {
    :
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
