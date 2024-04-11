test_binary=fmapc
test_name="Binary 2"
test_exit_code=0

set -- -b

test_stdin() {
    seq 255 -1 0 | xargs -d '\n' -n 1 printf '%02x' | rev
}

test_stdout() {
    seq 0 255 \
        | xargs -d '\n' -n 1 printf "%x\n" \
        | sed 's/^/\\x/' \
        | xargs -d '\n' -n 1 printf
}

test_stderr() {
    :
}
