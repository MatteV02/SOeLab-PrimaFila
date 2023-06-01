#!/bin/sh

#controllo se il numero di parametri uguale a 2
if test $# -ne 2
then
	echo Numero di parametri errato, inseriti $# devono essere 2
	exit 1
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
		exit 3
	fi;;
esac
#inserisco il parametro nella variabile K
K=$2
#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH
> /tmp/temp

FCR.sh $G $K /tmp/temp

#ciclo che richiede per ogni sigolo file trovato dal FCR al utente un numero positivo tra 0 e K
formain=
for i in `cat </tmp/temp`
do
	input=
	echo inserire un numero tra 0 e $K
	read input
	#controllo se il parametro $input e un numero strettamente positivo
	case $input in
	*[!0-9]*)
		echo "il parametro $input deve essere un numero"
		exit 3;;
	*)
		if test $input -eq 0 -a $input -le $K
		then
			echo "il parametro $input deve essere strettamente positivo e minore di $K"
			exit 2
		fi;;
	esac
	#inserisco il parametro nella variabile answer
	answer=$input
	formain="$formain $i $answer"
done

main $formain

rm /tmp/temp