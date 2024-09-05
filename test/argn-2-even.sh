test_binary=argn
test_name="Even"
test_exit_code=0

set -- ::2

command_stdin() {
    seq 0 10
}

expected_stdout() {
    printf "%s\n" 0 2 4 6 8 10
}
