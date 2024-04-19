test_binary=print-eof
test_name="No Arguments"
test_exit_code=0

set --

test_stdin() {
    :
}

test_stdout() {
    env printf "\x4"
}

test_stderr() {
    :
}
