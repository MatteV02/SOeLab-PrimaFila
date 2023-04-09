#!/bin/sh

cd $1

NumFile=0
files=

# Conto il numero di file
for i in *
do 	if test -f $i
	then 	if test `wc -l < $i` -eq $3
		then
			if grep $4 < $i > /dev/null
			then	NumFile=`expr $NumFile + 1`
				files="$files $i"
			fi
		fi
	fi
done

if test $NumFile -eq $2
then	echo `pwd`
	echo 17Gen18 $files $4 $3
fi

for i in *
do	if test -d $i -a -x $i
	then	FCR.sh `pwd`/$i $2 $3 $4
	fi
done
