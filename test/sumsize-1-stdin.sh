test_binary=sumsize
test_name="Pipe"
test_exit_code=0

set -- \
    -d \
    -f 6

test_stdin() {
    printf "%s\n" \
        '1MiB' \
        '1MB'
}

test_stdout() {
    printf "%s\n" \
        '2.048576MB'
}

test_stderr() {
    :
}
