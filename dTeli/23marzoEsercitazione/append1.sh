#!/bin/sh

case $# in
2) echo nice $1 $2
case $1 in 
./*)echo rel nice;;
*)echo not good 
exit 1;;
esac

case $2 in
./*)echo rel nice;;
*)echo not good
exit 1;;
esac

if test ! -f $1 -a ! -x $1
then echo not good1
exit 2
fi
if test -f $2
then echo veri nice
else echo not files
exit -2
fi

cat $2>>$1;;
*) echo not good 
exit 1;;
esac

