#!/bin/sh

#controllo numero di parametri
if test ! $# -eq 1
then	echo numero di params errato
	exit 1
fi

#controllo del parametro sia una dir assoluta
case $1 in
/*)	echo parametro corretto;;
*)	echo parametro errato not dirass;;
esac

G=$1

#variabile per il conteggio
conta=0
#genero file temporaneo per il conteggio dei livelli inizializzandolo al valore di conta
echo $conta >/tmp/conta$$

PATH=`pwd`:$PATH
export PATH

#inizio la fase A

FCR.sh $G $conta /tmp/conta$$ A
#inserisco il numero di livelli in una variabile
read ris </tmp/conta$$
echo $conta >/tmp/conta$$

#inizio la fase B
echo inserire il livello da 1 a $ris
read answer
#controllo che inserimeto sia un numero
case $answer in
[!0-9])	echo risposta errata inserire un numero
	exit 2;;
esac

#controllo se la risposta sia nei parametri richiesti
if test $answer -ge 1 -a $answer -le $ris
then
	if test `expr $answer % 2` -eq 0
	then
		#avvio del FCR in fase B con un parametro in piu
		FCR.sh $G $conta /tmp/conta$$ B $answer
	else
		echo non pari
	fi
else
	echo numero errato
fi

rm /tmp/conta$$
