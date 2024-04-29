test_binary=print-eof
test_name="No Arguments"
test_exit_code=0

set --

command_stdin() {
    :
}

expected_stdout() {
    env printf "\x4"
}

expected_stderr() {
    :
}
