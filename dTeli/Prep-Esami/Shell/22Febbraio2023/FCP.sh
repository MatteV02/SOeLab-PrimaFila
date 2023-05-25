#!/bin/sh

if test ! $# -ge 3
then
	echo numero di parametri errato
	exit 1
fi

#controllo del primo parametro sia un numero
case $1 in
[!0-9])	echo non un numero
	exit 2;;
esac
#controllo se il numero e tra 1 e 500
if test ! $1 -gt 0 -a ! $1 -lt $1
then
	echo inserire un numero da 1 a 500
	exit 2
fi

X=$1
#preparo i parametri per il controllo delle gerarchie che seguiono il primo parametro
shift
#scorro le gerarchie
G=
for i
do
	case $i in
	/*)	G="$G $i";;
	*)	echo parametro $i errato deve essere un percorso assoluto
		exit 2;;
	esac
done


PATH=`pwd`:$PATH
export PATH
> /tmp/nomiAssoluti$$

#inizio esplorazione delle q fasi
for i in $G
do
	FCR.sh $i $X  /tmp/nomiAssoluti$$
done

if test `wc -l </tmp/nomiAssoluti$$` -ge 2
then
	F=
	for i in `cat /tmp/nomiAssoluti$$`
	do
		F="$F $i"
	done
	echo parte.c $F
fi
rm /tmp/nomiAssoluti$$
