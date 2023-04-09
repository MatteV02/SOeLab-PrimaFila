#!/bin/sh

if test ! $# -ge 3
then
	echo num param non corretto
	exit 1
fi

case $1 in
?)	echo parametro 1 corretto;;
*) 	echo parametro 1 deve essere un singolo carattere
	exit 1;;
esac

C=$1
shift
G=
for i
do
	case $i in
	/*)	echo parametro corretto
		G="$G $i";;
	*)	echo percorso non assoluto
		exit 2;;
	esac
done

PATH=`pwd`:$PATH
export PATH
touch /tmp/nomiAssoluti

for i in $G
do
	FCR.sh $i $C /tmp/nomiAssoluti
done

echo numero di directory trovate `wc -l </tmp/nomiAssoluti`
for i in `cat /tmp/nomiAssoluti`
do
	echo directory $i
	echo Elena vuoi visualizzare il contenuto della directory y/n
	read answer
	case $answer in
	y)	ls -a $i;;
	esac
done

rm /tmp/nomiAssoluti
