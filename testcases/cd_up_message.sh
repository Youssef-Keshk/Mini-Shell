#!/usr/bin/env bash
cd ..

strip_prompt() {
  sed -E 's/^myshell> ?//'
}

remove_command_table() {
  sed '/COMMAND TABLE/,/^$/d'
}

out="$( (echo 'cd ..'; echo 'exit') | ./myshell | strip_prompt | remove_command_table )"

msg1="Changing to directory '..'"
msg2_prefix="You are now in "

if grep -Fxq "$msg1" <<< "$out" && grep -q "^$msg2_prefix" <<< "$out"; then
  echo 2
  exit 0
else
  echo 0
  exit 1
fi
