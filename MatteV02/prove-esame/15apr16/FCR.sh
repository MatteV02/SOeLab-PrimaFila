#!/bin/sh

#echo DEBUG-$*

cd $1

numeroFile=`ls -l | wc -l`
numeroFile=`expr $numeroFile - 1`

contaFile=0
nomeFile=

for i in *
do	if test -f $i
	then 	contaFile=`expr $contaFile + 1`
		nomeFile=$i
	fi
done

if test $contaFile -eq 1
then	if test `wc -l < $nomeFile` -ge $3
	then	echo `pwd` >> $2
	fi
fi

for i in *
do	if test -d $i -a -x $i
	then	$0 `pwd`/$i $2 $3
	fi
done

