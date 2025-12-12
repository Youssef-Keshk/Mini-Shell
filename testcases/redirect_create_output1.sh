#!/usr/bin/env bash
cd ..
rm -f output1.txt

# We only need to check the file is created (content varies)
( echo 'ls -al > output1.txt'; echo 'exit' ) | ./myshell >/dev/null 2>&1

if [[ -f output1.txt ]]; then
  echo 2
  exit 0
else
  echo 0
  exit 1
fi
