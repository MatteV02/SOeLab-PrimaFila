#!/bin/sh

cd $1

#echo $*

for i in *
do	#echo test $i = "$2.txt" -a -w $i -a -r $i -a -f $i
	if test $i = "$2.txt" -a -w $i -a -r $i -a -f $i
	then	echo `pwd`/$i `wc -l < $i` >> $3
	fi
done

for i in *
do	if test -d $i -a -x $i
	then	$0 `pwd`/$i $2 $3
	fi
done
