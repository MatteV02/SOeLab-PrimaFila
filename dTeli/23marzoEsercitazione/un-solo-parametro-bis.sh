#!/bin/sh

if test $# -eq 1
then 
case $1 in
/*) echo test $1; if test -f $1;
then echo $1 file;
else echo $1 directory;
fi;;
esac
else echo ex exit 1
fi
