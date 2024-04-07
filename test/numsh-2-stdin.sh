test_binary=numsh
test_name="Pipe"
test_exit_code=0

set -- \
    -m \
    -f \
    min \
    -f \
    max \
    -f \
    mean \
    -f \
    std

test_stdin() {
    printf "%s " \
        '640' \
        '56' \
        '0'
}

test_stdout() {
    printf "%s\n" \
        '0' \
        '640' \
        '232' \
        '354.446'
}

test_stderr() {
    :
}
