#!/bin/sh
#controllo il numero di parametri
if test ! $# -ge 4
then
	echo numero di parametri errato
	exit 1
fi

#controllo i primi due parametri siano numeri strettamente positivi

case $1 in
[!0-9])	echo errore primo parametro deve essere un numero positivo
	exit 2;;
0)	#controllo per scrupolo che non sia 0
	echo non puo essere 0
	exit 2;;
esac

H=$1
#preparo i parametri per controllare il secondo parametro
shift

case $1 in
[!0-9])	echo errore primo parametro deve essere un numero positivo
	exit 2;;
0)	#controllo per scrupolo che non sia 0
	echo non puo essere 0
	exit 2;;
esac

M=$1

#preparo i parametri per controllare le Q gerarchie
shift

G=
for i
do
	case $i in
		#controllo per scrupolo che sia una gerachia attraversabile
	/*)	if test -d $i -a -x $i
		then
			G="$G $i"
		fi;;
	*)	echo parametro $i deve essere assoluto
		exit 3;;
	esac
done
#esporto la PATH
PATH=`pwd`:$PATH
export PATH

for i in $G
do
	#richiamo la ricorsiva per esplorare la gerarchia Q
	FCR.sh $i $M $H
done