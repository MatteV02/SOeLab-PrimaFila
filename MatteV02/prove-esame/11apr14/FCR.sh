#!/bin/sh

cd $1

files=
countFiles=0

for i in *
do	if test -f $i -a -r $i
	then	righeFile=`wc -l < $i`
		righeFileA=`grep '^a' < $i | wc -l`
		if test $righeFile -eq $righeFileA
		then	files="$files `pwd`/$i"
			countFiles=`expr $countFiles + 1`
		fi
	fi
done

if test ! -z $files
then	for i in $files
	do	echo $i >> $2
	done
fi

for i in *
do	if test -d $i -a -x $i
	then	$0 `pwd`/$i $2
	fi
done
