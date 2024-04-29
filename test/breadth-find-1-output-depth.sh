test_binary=breadth-find
test_name="Output Depth"
test_exit_code=0

set -- ../src

test_stdout() {
    awk -v c="/" '{print gsub(c, "")}' "$1" | sort -nC
}

expected_stderr() {
    :
}
