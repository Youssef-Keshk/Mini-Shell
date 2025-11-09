#!/usr/bin/env bash
cd ..

# Ensure file exists, then append
( echo 'echo "second line" >> output1.txt'; echo 'exit' ) | ./myshell >/dev/null 2>&1

strip_prompt(){ sed -E 's/^myshell> ?//' ; }
remove_command_table(){
  sed -n '/COMMAND TABLE/{
    :a;N;/^\n/!ba;d
  }; p'
}
out="$( (echo 'cat output1.txt | grep line'; echo 'exit') | ./myshell | strip_prompt | remove_command_table )"
last="$(printf '%s\n' "$out" | tail -n1)"

if [[ "$last" == '"second line"' ]]; then
  echo 2
  exit 0
else
  echo 0
  exit 1
fi
