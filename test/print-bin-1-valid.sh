test_binary=print-bin
test_name="Valid"
test_exit_code=0

# print-bin supports 0-255, but, after 127, print-bin/shell encodings differ
eval "set -- $(seq 0 127 | tr '\n' ' ')"

test_stdin() {
    :
}

test_stdout() {
    seq 0 127 | xargs -d '\n' -n 1 printf "%x\n" | xargs -d '\n' -I CODE printf "\xCODE"
}

test_stderr() {
    :
}
