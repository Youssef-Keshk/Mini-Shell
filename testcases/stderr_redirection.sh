#!/usr/bin/env bash
cd ..

rm -f err.txt

( echo 'cat no_such_file 2> err.txt'; echo 'exit' ) | ./myshell >/dev/null 2>&1

if [[ -s err.txt ]]; then
  echo 2
  exit 0
else
  echo 0
  exit 1
fi
