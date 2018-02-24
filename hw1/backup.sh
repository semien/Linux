#!/bin/bash

DIRNAME="$1"
ARCHNAME="$2"
shift
shift

mkdir $DIRNAME 2>/dev/null

for extension in "$@"
do
  FILES=$(find $HOME -type f -name "*.$extension")
  LenExt=${#extension}
  for file in $FILES
  do
    LName=$(echo $file | grep -o "[^/]*$")
    LenLName=${#LName}
    Name=${LName:0:$(($LenLName-$LenExt-1))} # name without extension
    num=1
    while true
    do
      if [ -f "$DIRNAME/$Name($num).$extension" ]
      then
        num=$(($num+1))
      else
        cp 2>/dev/null $file "$DIRNAME/$Name($num).$extension"
        break
      fi
    done
  done
done

tar -cf $ARCHNAME $DIRNAME

echo "done"
