#!/bin/sh

cd $1
for i in *
do
	if test -f $i
	then	echo [F] `pwd`/$i
	else	echo [D] `pwd`/$i
	fi
done
