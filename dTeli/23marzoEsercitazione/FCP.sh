#!/bin/sh

if test ! $# -eq 1
then 	echo no good 
	exit 1
fi
case $1 in
/*)
	if test ! -d $1 -a ! -x $1
	then	echo not good
	exit 2	
	fi;;
esac

./file-dir.sh $1

