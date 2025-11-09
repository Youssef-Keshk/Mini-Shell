#!/usr/bin/env bash
cd ..

strip_prompt(){ sed -E 's/^myshell> ?//' ; }
remove_command_table(){
  sed -n '/COMMAND TABLE/{
    :a;N;/^\n/!ba;d
  }; p'
}

out="$( (echo 'ls -1 | wc -l'; echo 'exit') | ./myshell | strip_prompt | remove_command_table )"
num="$(printf '%s\n' "$out" | tail -n1 | tr -d '\r')"

# Accept any positive integer (directory contents can change)
if [[ "$num" =~ ^[0-9]+$ ]] && (( num > 0 )); then
  echo 2
  exit 0
else
  echo 0
  exit 1
fi
