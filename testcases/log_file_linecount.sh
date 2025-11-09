#!/usr/bin/env bash
cd ..

# Clear log.txt at the start
: > log.txt

# Run 10 simple commands, each followed by newline to separate
(
  echo 'echo one'
  echo 'echo two'
  echo 'echo three'
  echo 'echo four'
  echo 'echo five'
  echo 'echo six'
  echo 'echo seven'
  echo 'echo eight'
  echo 'echo nine'
  echo 'echo ten'
  echo 'exit'
) | ./myshell >/dev/null 2>&1

# Count lines in log.txt
linecount=$(wc -l < log.txt || echo 0)

if [[ "$linecount" -eq 10 ]]; then
  echo 2
  exit 0
else
  echo 0
  exit 1
fi
