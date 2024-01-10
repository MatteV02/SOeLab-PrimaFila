#!/bin/sh

#controllo che il parametro $1 sia un percoso assoluto
case $1 in
/*)	#controllo che il parametro $1 sia una directory attraversabile
	if test -d $1 -a -x $1
	then
		echo "parametro $1 corretto"
	else
		echo "parametro $1 non una directory o non attraversabile"
		exit 1
	fi;;
*)	echo "parametro $1 non assoluto"
	exit 1;;
esac
#inserisco il parametro nella variabile G
G=$1
#controllo se il parametro $2 e un numero strettamente positivo
case $2 in
*[!0-9]*)
	echo "il parametro $2 deve essere un numero"
	exit 3;;
*)
	if test $2 -eq 0
	then
		echo "il parametro $2 deve essere strettamente positivo"
		exit 2
	fi;;
esac
#inserisco il parametro nella variabile L
L=$2

shift 
shift
#Inizializzo la variabile Q dove inserisco caratteri
Q=
for q  
do
	#controllo se i parametri rimasti sono singoli caratteri se si li inserisco nella lista Q
	case $q in 
	?)	
		echo parametro corretto;;
	*)
		echo parametro $q deve essere un singolo carattere
		exit 3;;
	esac
	Q="$Q $q"
done

> /tmp/nomiAssoluti

#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH

FCR.sh $G $L /tmp/nomiAssoluti $Q 

N=`cat </tmp/nomiAssoluti`
echo "il numero di file trovati e' $N"
rm  /tmp/nomiAssoluti