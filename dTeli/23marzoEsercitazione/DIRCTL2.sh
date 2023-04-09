#!/bin/sh

case $#  in

0) ls * ;;
bash: line 1: y: command not found
1)
if test -f $1 ;
then exit ./DIR.sh $1 ;
fi ;

if test -d $1 ;
then ls -d ./$1 ;
fi ;;
*) exit 1 ;;
esac
