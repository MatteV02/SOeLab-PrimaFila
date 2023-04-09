#!/bin/sh

cd $1

terzoParametro=

#echo DEBUG-$*

case $4 in
A)	terzoParametro=`cat < $3`
	if test $2 -gt $terzoParametro
	then	echo $2 > $3
	fi
	;;
*)	terzoParametro=$3
#	echo DEBUG-test $2 -eq $terzoParametro
	if test $2 -eq $terzoParametro
	then 	echo Stampo le informazioni relative a $1
		ls -l
	fi
	;;
esac

for i in *
do 	if test -d $i -a -x $i
        then    $0 $1/$i `expr $2 + 1` $3 $4
	fi
done
