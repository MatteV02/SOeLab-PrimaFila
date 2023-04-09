#!/bin/sh

cd $1

fileTrovati=

for i in *
do	if	test -f $i -a -r $i
	then	if	test `wc -l < $i` -eq $2
		then	fileTrovati="$fileTrovati $i"
		fi
	fi
done

if	test ! -z "$fileTrovati"
then	echo "Una directory che rispetta le specifiche e' `pwd`"
	for i in $fileTrovati
	do	echo `pwd`/$i >> $3
	done
fi

for i in *
do	if      test -d $i -a -x $i
	then    $0 `pwd`/$i $2 $3
	fi
done
