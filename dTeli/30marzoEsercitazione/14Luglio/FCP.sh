#!/bin/sh
#contorllo numero parametri
if test ! $# -ge 4
then 	echo numero parametri non corretto
	exit 1
fi
counter=1
for i
do
	if test $counter -ge `expr $# - 1`
	then
		case $i in
		*[!0-9]*)
			echo parametro errato
			exit 3;;
		esac
		echo parametro corretto
	else
		case $i in
		/*)	echo parametro corretto;;
		*)	echo parametro errato
			exit 2;;
		esac
	fi
	counter=`expr $counter + 1`
done

counter=0
Q=
for i
do
	counter=`expr $counter + 1`

	if test $counter -eq `expr $# - 1`
	then
		H=$i
		continue
	fi

	if test $counter -eq $#
	then
		M=$i
		continue
	fi

	Q="$Q $i"
done

PATH=`pwd`:$PATH
export PATH

for i in $Q
do
	FCR.sh $i $M $H
done
