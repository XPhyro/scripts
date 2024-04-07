test_binary=fmapc
test_name="Binary 1"
test_exit_code=0

set -- -b

test_stdin() {
    printf 0123456789abcdef
}

test_stdout() {
    printf "%s" '2Tv˜ºÜş'
}

test_stderr() {
    :
}
