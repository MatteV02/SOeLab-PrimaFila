#!/bin/sh

if test $# -lt 3
then	echo Errore, devi passare almeno 3 parametri
	exit 1
fi

count=1
stringa=
gerarchie=

for i
do	if test $count -eq $#
	then	stringa=$i
	else	case $i in
		/*)
			;;
		*)	echo Il parametro $i non rappresenta un percorso assoluto
			exit 2
			;;
		esac
		if test ! -d $i -o ! -x $i
		then	echo Il parametro $i non directory o non percorribile
			exit 3
		fi
		gerarchie="$gerarchie $i"
	fi
	count=`expr $count + 1`
done

tempFile=/tmp/5apr19-$$
> $tempFile

PATH=`pwd`:$PATH
export PATH

#echo DEBUG-$gerarchie $stringa

for i in $gerarchie
do	FCR.sh $i $stringa $tempFile
done

fileAttuale=

echo Sono stati trovati globalmente `wc -l < $tempFile` files
for i in `cat < $tempFile`
do	if test -f $i
	then	echo -n "Il file $i contiene "
		fileAttuale=$i
	else	echo "$i linee"
		varInput=
		echo -n "Vuoi ordinare questo file? (y/n)> "
		read varInput
		case $varInput in
		y)	echo Stampo il file $fileAttuale ordinato
			sort -f < $fileAttuale
			;;
		*)	echo Non stampo il file $fileAttuale ordinato
			;;
		esac
	fi
done

rm $tempFile
