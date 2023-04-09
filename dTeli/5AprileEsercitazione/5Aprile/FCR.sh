#!/bin/sh


cd $1

for i in *
do
	if test -f $i -a -r $i -a -w $i
	then
		if test $i = $2.txt
		then
			echo `wc -c  <$i` `pwd`/$i >$3
		fi
	else
		if test -d $i -a -x $i
		then
			FCR.sh `pwd`/$i $2 $3
		fi
	fi
done
