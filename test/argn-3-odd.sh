test_binary=argn
test_name="Odd"
test_exit_code=0

set -- 1::2

command_stdin() {
    seq 0 10
}

expected_stdout() {
    printf "%s\n" 1 3 5 7 9
}
