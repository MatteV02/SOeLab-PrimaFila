#!/bin/sh

case $# in
1) echo nice $1
if test -f $1
then echo veri nice
else echo not file 
exit -1
fi
cat >>$1;;
2) echo nice $1 $2
if test -f $1 -a -f $2
then echo veri nice
else echo not files
exit -2
fi
cat $2>>$1;;
*) echo not good 
exit 1;;
esac

