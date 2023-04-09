#!/bin/sh
cd $1
count=0
files=
for i in *
do
	#controllo se un file
	if test -f $i
	then
		if test `wc -l <$i` -eq $2 
		then
			if `grep $4 $i >/dev/null 2>&1`
			then
				count=`expr $count + 1`
				files="$files $i"
			fi
		fi
	fi
done

if test $count -eq $2
then 	echo trovati file che conformano
	echo $files $3 $2
	echo `pwd`
fi

for i in *
do
	if test -d $i -a -x $i #controllo se directory ed attraversabile
	then
		17GennaioRic.sh `pwd`/$i $2 $3 $4 
	fi
done
