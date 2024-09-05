test_binary=calctdiff
test_name="Date"
test_exit_code=0

set -- 2020-10-11 2024-09-06

expected_stdout() {
    printf "%s\n" "1426 days, 0 hours, 0 minutes, 0 seconds"
}
