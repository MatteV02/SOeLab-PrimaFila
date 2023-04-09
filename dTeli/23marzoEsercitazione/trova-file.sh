#!/bin/sh

if test ! $# -eq 1
then exit 1
fi
if test $1 = ./test
then echo aaaaaaaa
fi

case $1 in
./*) echo nice ;;
*) echo no ; exit 2;; 
esac
read a1
for i in *
do 
if test -f $i -a $i = $a1
then echo il file esiste
exit 0
fi
done  
