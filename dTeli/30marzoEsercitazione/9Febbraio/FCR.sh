#!/bin/sh

cd $1

for i in *
do
	if test -f $i -a -r $i
	then
		if test `wc -l <$i` -eq $2
		then
			echo file corretto `pwd`/$i
			echo `pwd`/$i >>$3
		fi
	else
		if test -d $i -a -x $i
		then
			FCR.sh `pwd`/$i $2 $3
		fi
	fi
done
