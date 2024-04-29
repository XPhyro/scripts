test_binary=fmapc
test_name="Binary"
test_exit_code=0

set -- -b

command_stdin() {
    seq 255 -1 0 | xargs -d '\n' -n 1 printf '%02x' | rev
}

expected_stdout() {
    seq 0 255 \
        | xargs -d '\n' -n 1 printf "%x\n" \
        | sed 's/^/\\x/' \
        | xargs -d '\n' -n 1 printf
}

expected_stderr() {
    :
}
