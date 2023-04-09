#!/bin/sh

if test $# -lt 2
then	echo "Devi passare almeno 2 parametri"
	exit 1
fi

for i
do	case $i in
	/*)	echo DEBUG-il parametro $i è un percorso assoluto
		;;
	*)	echo Il parametro $i non è un percorso assoluto
		exit 2
		;;
	esac

	if test ! -d $i -o ! -x $i
	then	echo Il parametro $i non è una directory o non è traversabile
		exit 3
	fi
done

PATH=`pwd`:$PATH
export PATH

count=1

for i
do	echo DEBUG-analizzo la gerarchia $i
	tempFile=/tmp/11apr14$$-$count
	count=`expr $count + 1`
	> $tempFile
	FCR.sh $i $tempFile
done

count=1

for n
do	tempFile=/tmp/11apr14$$-$count
	count=`expr $count + 1`
	echo La gerarchia $n contiene `wc -l < $tempFile` file
	for i in `cat $tempFile`
	do	echo Un file che rispetta le specifiche è $i
		echo -n "Quante righe di questo file vuoi visualizzare? "
		leggiDaTastiera=
		read leggiDaTastiera
		case $leggiDaTastiera in
		*[!0-9]*)	echo "Devi inserire un numero intero!"
				exit 4;;
		*)		echo "Stampo $leggiDaTastiera linee dal file:"
				head -$leggiDaTastiera < $i;;
		esac
	done
	rm $tempFile
done
