#!/bin/sh
#controllo numero parametri
if test ! $# -ge 4
then
	echo numero di parametri errato
	exit 1
fi
#controllo se il primo parametro e un numero
case $1 in
[!0-9])	echo non un numero positivo
	exit 2;;
0)	echo inserire numero strettamente positivo
	exit 2;;
esac
echo parametro 1 corretto
#lo memorizzo e elimino il parametro 1
W=$1

shift
#memorizzo la stringa e la elimino dai parametri
S=$1
shift

#controllo e memorizzo i percorsi assoluti delle gerarchie
G=
for i
do
	case $i in
	/*)	echo parametro corretto
		G="$G $i";;
	*)	echo parametro errato deve essere assoluto
		exit 3;;
	esac
done
#genero il file temporaneo per memorizzare il nome delle dir conformi alle richieste
touch /tmp/nomiAssoluti
#aggiorno il path
PATH=`pwd`:$PATH
export PATH

for i in $G
do	#chiamo la ricorsiva per le Q fasi
	FCR.sh $i $S /tmp/nomiAssoluti
done
#stampo e controllo il numero di directory trovate dal file ricorsivo nelle q fasi
totale=`wc -l </tmp/nomiAssoluti`
echo il numero di directory trovate sono $totale

if test $W -ge $totale
then
	#chiedo al utente un numero
	echo $USER scegliere un numero tra 1 e $W
	read X
	#controllo che sia un numero
	case $X in
	[!0-9])
		echo inserimento errato
		exit 3;;
	esac
	#controllo se il numero e valido
	if test ! $X -le $W -a ! $X -ge 1
	then
		echo inserire valore tra 1 e $W
		exit 3
	fi
	#stampo il nome assoluto della direcory al numero richiesto
	echo la directory $X
	echo `head -n $X </tmp/nomiAssoluti | tail -n 1`
fi
#elimino il file temporaneo non piu utilizzato
rm /tmp/nomiAssoluti


