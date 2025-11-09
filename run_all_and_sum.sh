#!/usr/bin/env bash
set -euo pipefail
cd "testcases"

total=0

# Correct test execution order — dependencies respected
tests=(
  basic_ls_al.sh                 # Should run first: validates shell runs and lists files
  echo_hello_os_lab.sh           # Independent test
  redirect_create_output1.sh     # Creates output1.txt (needed by next tests)
  cat_input_output1.sh           # Reads from output1.txt
  append_and_grep_line.sh        # Appends to output1.txt and reads back
  count_ls_wc.sh                 # Independent, but ls must work
  background_sleep2.sh           # Independent: tests background jobs
  cd_up_message.sh               # Independent: tests cd messaging
  stderr_redirection.sh
  pwd_test.sh
  log_file_linecount.sh
)

for t in "${tests[@]}"; do
  if [[ -x "$t" ]]; then
    score="$(bash "$t")"
    echo "$t -> $score"
    total=$(( total + score ))
  else
    echo "$t -> MISSING OR NOT EXECUTABLE"
  fi
done

echo "TOTAL=$total"
