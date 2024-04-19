test_binary=print-eof
test_name="Dummy Arguments"
test_exit_code=0

set -- 0 1 2 3 4 5 6 7 8 9

test_stdin() {
    :
}

test_stdout() {
    env printf "\x4"
}

test_stderr() {
    :
}
