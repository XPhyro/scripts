test_binary=calctdiff
test_name="Timestamp"
test_exit_code=0

set -- 2020-10-11 @1725575242

expected_stdout() {
    printf "%s\n" "1426 days, 1 hours, 27 minutes, 22 seconds"
}
