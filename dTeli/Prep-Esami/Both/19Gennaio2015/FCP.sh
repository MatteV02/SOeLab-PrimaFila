#!/bin/sh

#controllo se il numero di parametri uguale a 4
if test $# -ne 4
then
	echo Numero di parametri errato, inseriti $# devono essere 4
	exit ExitValue
fi
#controllo che il parametro $1 sia un percoso assoluto
case $1 in
/*)	#controllo che il parametro $1 sia una directory attraversabile
	if test -d $1 -a -x $1
	then
		echo "parametro $1 corretto"
	else
		echo "parametro $1 non una directory o non attraversabile"
		exit 2
	fi;;
*)	echo "parametro $1 non assoluto"
	exit 2;;
esac
#inserisco il parametro nella variabile G1
G1=$1
#controllo che il parametro $2 sia un percoso assoluto
case $2 in
/*)	#controllo che il parametro $2 sia una directory attraversabile
	if test -d $2 -a -x $2
	then
		echo "parametro $2 corretto"
	else
		echo "parametro $2 non una directory o non attraversabile"
		exit 3
	fi;;
*)	echo "parametro $2 non assoluto"
	exit 3;;
esac
#inserisco il parametro nella variabile G2
G2=$2
#controllo il parametro $3 se e un singolo carattere
case $3 in 
?) 
	echo parametro corretto $3;;	
*) 
	echo parametro $3 errato
	exit 4;;
esac
C1=$3

#controllo il parametro $4 se e un singolo carattere
case $4 in 
?) 
	echo parametro corretto $4;;	
*) 
	echo parametro $4 errato
	exit 4;;
esac
C2=$4
#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH

> /tmp/tempC1
> /tmp/tempC2

FCR.sh $G1 $C1 /tmp/tempC1
FCR.sh $G2 $C2 /tmp/tempC2

#controllo se le line contenute in /tmp/tempC1 sono esattamente uguali a quelle /tmp/tempC2
if test `wc -l < /tmp/tempC1` -eq `wc -l < /tmp/tempC2`
then
	main `cat < /tmp/tempC1` `cat </tmp/tempC2` $C1 $C2
fi
rm /tmp/tempC1
rm /tmp/tempC2
