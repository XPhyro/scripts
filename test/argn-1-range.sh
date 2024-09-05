test_binary=argn
test_name="Range"
test_exit_code=0

set -- 1:3

command_stdin() {
    seq 10
}

expected_stdout() {
    printf "%s\n" 2 3
}
