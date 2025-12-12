#!/usr/bin/env bash
cd ..

if [[ ! -f output1.txt ]]; then
  ( echo 'ls -al > output1.txt'; echo 'exit' ) | ./myshell >/dev/null 2>&1
fi

strip_prompt(){ sed -E 's/^myshell> ?//' ; }
remove_command_table(){
  sed -n '/COMMAND TABLE/{
    :a;N;/^\n/!ba;d
  }; p'
}

out="$( (echo 'cat < output1.txt'; echo 'exit') | ./myshell | strip_prompt | remove_command_table )"

if grep -q '^total ' <<<"$out" && grep -q 'command.cc' <<<"$out" && grep -q 'tokenizer.cc' <<<"$out"; then
  echo 2
  exit 0
else
  echo 0
  exit 1
fi
