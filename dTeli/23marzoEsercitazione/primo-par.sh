#!/bin/sh

if test ! $# -ge 1
then echo no param pal
fi

echo primo param $1
echo resto dei parametri
flag=1
for i
do
	if test $flag -eq 1
	then 	flag=0
	else	echo $i
	fi
done

