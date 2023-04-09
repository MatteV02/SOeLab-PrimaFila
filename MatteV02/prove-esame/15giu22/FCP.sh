#!/bin/sh

if test $# -lt 3
then	echo "Errore: devi passare almeno 3 parametri"
	exit 1
fi

G=

for i
do	case $i in
	/*)
		;;
	*)	echo "Errore: Il parametro $i deve rappresentare un percorso assoluto"
		exit 2;;
	esac
	if test ! -d $i -o ! -x $i
	then	echo "Errore: il parametro $i non directory o non percorribile"
		exit 3
	fi
	G="$G $i"
done


tempFile=/tmp/nomiAssoluti
> $tempFile

PATH=`pwd`:$PATH
export PATH

for i in $G
do
	FCR.sh $i $tempFile
done

N=`wc -l < $tempFile`

echo Sono stati trovati $N file corrispondenti alla specifica

listaFile=
for i in `cat < $tempFile`
do	listaFile="$listaFile $i"
done

echo Invoco la parte C
echo 15giu22 $listaFile

rm $tempFile
