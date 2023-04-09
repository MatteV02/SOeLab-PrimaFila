#!/bin/sh

if test ! $# -ge 1
then echo not enoght
ecxit 1
fi
e=0
for i 

do
e=`expr $e + 1`
echo  \$$e $i
done


