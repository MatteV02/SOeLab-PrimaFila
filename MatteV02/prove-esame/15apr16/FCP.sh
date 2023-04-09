#!/bin/sh

if test $# -le 2
then	echo "Errore: devi inserire almeno 3 parametri"
	exit 1
fi

count=1
ultimoPar=
gerarchie=

for i
do	if test $count -eq $#
	then	case $i in
		*[!0-9]*)	echo Errore inserito un valore non numerico o non strettamente positivo in ultima posizione
				exit 2;;
		*)		if test $i -eq 0
				then	echo Errore inserito un valore intero non strettamente positivo
				fi
				;;
		esac
		ultimoPar=$i
	else	case $i in
		/*)	if test ! -d $i -o ! -x $i
			then 	echo Il parametro $i non rappresenta una directory traversabile
				exit 3
			fi;;
		*)	echo Il parametro $i non rappresenta un percorso assoluto
			exit 4;;
		esac
		gerarchie="$gerarchie $i"
	fi
	count=`expr $count + 1`
done

PATH=`pwd`:$PATH
export PATH

tempFile="/tmp/15apr16-$$"
> $tempFile

for i in $gerarchie
do
#	echo DEBUG-$tempFile
	FCR.sh $i $tempFile $ultimoPar
	count=`expr $count + 1`
done

echo "Il numero di directory trovate è `wc -l < $tempFile`"
for dir in `cat < $tempFile`
do	echo Esamino il direttorio $dir
	for file in $dir/*
	do	echo File\: $file
		echo -n "$ultimoPar Linea: "
		head -$ultimoPar < $file | tail -1
	done
done
count=`expr $count + 1`

rm $tempFile
