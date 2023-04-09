#!/bin/sh

if test ! $# -ge 1
then echo no param pal
fi
flag=0
for i
do 
	if test $flag -eq `expr $# - 1`
	then echo ultimo parametro $i
	else flag=`expr $flag + 1`
	fi
done
echo resto dei parametri
flag=0
for i
do
	if test $flag -eq `expr $# - 1`
	then 	exit 0
	else	echo $i
		flag=`expr $flag + 1`
	fi
done
