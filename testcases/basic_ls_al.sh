#!/usr/bin/env bash
cd ..

strip_prompt(){ sed -E 's/^myshell> ?//' ; }
remove_command_table(){
  sed -n '/COMMAND TABLE/{
    :a;N;/^\n/!ba;d
  }; p'
}

out="$( (echo 'ls -al'; echo 'exit') | ./myshell | strip_prompt | remove_command_table )"

if grep -q 'command.cc' <<<"$out" && \
   grep -q 'tokenizer.cc' <<<"$out" && \
   grep -q 'myshell' <<<"$out"; then
  echo 2
  exit 0
else
  echo 0
  exit 1
fi

