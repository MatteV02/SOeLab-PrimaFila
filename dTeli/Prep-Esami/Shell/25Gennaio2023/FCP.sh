#!/bin/sh
#controllo il numero di parametri
if test ! $# -eq 3
then
	echo numero di parametri errato
	exit 1
fi
#controllo primo parametro
case $1 in
/*)	if test ! -d $1 -a ! -x $1
	then
		echo directory non esistente o non attraverabile
		exit 2
	fi;;
*)	echo inserire path assoluto
	exit 2;;
esac
echo primo parametro corretto
G=$1
#preparo i parametri per il controllo sulle stringe
shift

for i
do	#controllo se le stringe contengono / che creano errori nei controllo successivi
	case $i in
	*/*) 	echo i parametri non devono contenere slash
		exit 3;;
	esac
done
#inserisco i parametri in variabili
E1=$1
E2=$2

#aggiorno la path
PATH=`pwd`:$PATH
export PATH

FCR.sh $G $E1 $E2
