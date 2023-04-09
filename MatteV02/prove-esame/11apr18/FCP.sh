#!/bin/sh

if test $# -lt 3
then	echo Errore, devi passare almeno 3 parametri
	exit 1
fi

Y=

case $1 in
*[!0-9]*)	echo "Il primo parametro è non numerico o non strettamente positivo"
		exit 2;;
*)		if test $1 -eq 0
		then	echo "Il primo parametro è non strettamente positivo"
			exit 3
		fi
		Y=$1
		shift
		;;
esac

for i
do	case $i in
	/*)
		;;
	*)	echo "Il parametro $i non rappresenta un percorso assoluto"
		exit 4;;
	esac

	if test ! -d $i -o ! -x $i
	then	echo "Il parametro $i non è directory o non è percorribile"
		exit 5
	fi
done

tempFile=/tmp/11apr18-$$
> $tempFile

PATH=`pwd`:$PATH
export PATH

for i
do	FCR.sh $i $Y $tempFile
#	echo Finora sono stati creati `wc -l < $tempFile` file
done

echo Sono stati creati globalmente `wc -l < $tempFile` files

for i in `cat < $tempFile`
do	echo Ho creato il file $i
	echo -n "Il contenuto del file è: "
	cat < $i
	echo
done

rm $tempFile
