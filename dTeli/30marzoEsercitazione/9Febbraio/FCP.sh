#!/bin/sh
#controllo numero di parametri
if test ! $# -eq 2
then 	echo numero di parametri non corretto
	exti 1
fi

#controllo parametro 1 se percorso assoluto
case $1 in
/*)	echo parametro 1 corretto;;
*)	echo parametro 1 non assoluto
	exit 2;;
esac
G=$1

#controllo parametro 2 numero strettamete positivo
case $2 in
*[!0-9]*)
	echo parametro 2 non numero
	exit 2;;
esac
echo parametro 2 corretto
K=$2

PATH=`pwd`:$PATH
export PATH

#genero file temporaneo per contenere i path dei file
touch /tmp/files$$

FCR.sh $G $K /tmp/files$$
invocationVar=
for i in `cat /tmp/files$$`
do
	echo inserire numero minore o uguale a $K
	read answer
	if test ! $answer -le $K
	then
		echo inserimento errato
		exit 3
	fi
	invocationVar="$invocationVar $i $answer"
done
echo invoco la parte C con $invocationVar
rm /tmp/files$$
