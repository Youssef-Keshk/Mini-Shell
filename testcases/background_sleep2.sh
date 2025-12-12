#!/usr/bin/env bash
cd ..

strip_prompt(){ sed -E 's/^myshell> ?//' ; }

extract_background(){
  awk '
    BEGIN { in_table=0; colline=0; }
    /COMMAND TABLE/ { in_table=1; next }
    in_table && /Background/ { colline=1; next }
    in_table && colline && /^[ -]+$/ { next }
    in_table && colline && NF { print $NF; exit }
  '
}

out="$( (echo 'sleep 1 &'; echo 'exit') | ./myshell | strip_prompt )"
bg="$(printf '%s\n' "$out" | extract_background | tr -d '\r')"

if [[ "$bg" == "YES" ]]; then
  echo 2
  exit 0
else
  echo 0
  exit 1
fi
