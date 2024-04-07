test_binary=fmapc
test_name="Binary 2"
test_exit_code=0

set -- -b

test_stdin() {
    printf 00102030405060708090a0b0c0d0e0f0
}

test_stdout() {
    printf "\0%s" '	
'
}

test_stderr() {
    :
}
