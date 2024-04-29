test_binary=sumsize
test_name="Pipe"
test_exit_code=0

set -- \
    -d \
    -f 6

command_stdin() {
    printf "%s\n" \
        '1MiB' \
        '1MB'
}

expected_stdout() {
    printf "%s\n" \
        '2.048576MB'
}

expected_stderr() {
    :
}
