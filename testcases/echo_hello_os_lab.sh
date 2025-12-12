#!/usr/bin/env bash
cd ..

strip_prompt(){ sed -E 's/^myshell> ?//' ; }
remove_command_table(){
  sed -n '/COMMAND TABLE/{
    :a;N;/^\n/!ba;d
  }; p'
}
out="$( (echo 'echo "Hello OS Lab"'; echo 'exit') | ./myshell | strip_prompt | remove_command_table )"
last="$(printf '%s\n' "$out" | tail -n1)"

if [[ "$last" == '"Hello OS Lab"' ]]; then
  echo 2
  exit 0
else
  echo 0
  exit 1
fi
