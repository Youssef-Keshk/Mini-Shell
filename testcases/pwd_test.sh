#!/usr/bin/env bash
cd ..

strip_prompt() {
  sed -E 's/^myshell> ?//'
}

# Remove all COMMAND TABLE blocks
remove_command_table() {
  sed '/COMMAND TABLE/,/^$/d'
}

out="$( (echo 'pwd'; echo 'exit') | ./myshell | strip_prompt | remove_command_table )"

# Get last non-empty line
last_line="$(printf '%s\n' "$out" | awk 'NF' | tail -n1)"

if [[ -n "$last_line" ]]; then
  echo 2
  exit 0
else
  echo 0
  exit 1
fi
