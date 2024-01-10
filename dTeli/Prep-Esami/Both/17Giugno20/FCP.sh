#!/bin/sh

#controllo se il numero di parametri e maggiore o uguale a 3
if test $# -lt 3
then
	echo Numero di parametri errato, inseriti $# devono essere maggiori o uguali a 3
	exit 1
fi

#Inizializzo la variabile Gers dove inserisco i nomi assoluti delle directory
Gers=
for i  
do
	#controllo se sono arrivato al ultimo parametro
	if test ! $# -eq 1
	then	
		#controllo che il parametro $i sia un percoso assoluto
		case $i in
		/*)	#controllo che il parametro $i sia una directory attraversabile
			if test -d $i -a -x $i
			then
				echo "parametro $i corretto"
				cont=`expr $cont + 1`
			else
				echo "parametro $i non una directory o non attraversabile"
				exit 2
			fi;;
		*)	echo "parametro $i non assoluto"
			exit 2;;
		esac
		#inserisco il parametro nella variabile G
		Gers="$Gers $i"
		shift
	fi
done

#controllo se il parametro $1 e un numero strettamente positivo
case $1 in
*[!0-9]*)
	echo "il parametro $1 deve essere un numero"
	exit 3;;
*)
	if test $1 -eq 0
	then
		echo "il parametro $1 deve essere strettamente positivo"
		exit 3
	fi;;
esac
#inserisco il parametro nella variabile B
B=$1

#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH

#esamino le Q fasi
for F in $Gers
do
	FCR.sh $F $B
done
