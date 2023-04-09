#!/bin/sh

if test ! $# -eq 3
then
	echo numero di parametri errato
	exit 1
fi

case $1 in
/*)	echo parametro 1 corretto;;
*)	echo parametro 1 deve essere un percorso assoluto
	exit 2;;
esac

G=$1
S=$2

case $3 in
[!0-9])	echo parametro 3 errato deve essere un numero
	exit 3;;
0)	echo parametro 3 deve essere un numero strettamente positiov
	exit 3;;
esac
echo parametro 3 corretto
N=$3
PATH=`pwd`:$PATH
export PATH

touch /tmp/nomiAssoluti$$

#fase A

FCR.sh $G $S /tmp/nomiAssoluti$$ A
totale=`wc -l </tmp/nomiAssoluti$$`
echo numero di dir trovati $totale
if test $totale -gt $N
then
	echo inserire un numero tra 1 e $N
	read X
	case $X in
	[!0-9])	echo inserimento errato deve essere un numero
		exit 3;;
	esac
	if test ! $X -ge 1 -a ! $X -le $N
	then
		echo il numero deve essere compreso tra 1 e $N
		exit 3
	fi
	G=`head -n $X </tmp/nomiAssoluti$$ | tail -n 1`
	FCR.sh $G $S $X B
fi

rm /tmp/nomiAssoluti$$
