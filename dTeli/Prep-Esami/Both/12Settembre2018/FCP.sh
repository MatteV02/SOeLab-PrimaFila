#!/bin/sh

#controllo se il numero di parametri e maggiore o uguale a 4
if test $# -lt 4
then
	echo Numero di parametri errato, inseriti $# devono essere maggiori o uguali a 4
	exit 1
fi

#controllo se il parametro $1 e un numero strettamente positivo
case $1 in
*[!0-9]*)
	echo "il parametro $1 deve essere un numero"
	exit 3;;
*)
	if test $1 -eq 0
	then
		echo "il parametro $1 deve essere strettamente positivo"
		exit 2
	fi;;
esac
#inserisco il parametro nella variabile H
H=$1
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

#controllo se H e minore di K
if test $H -ge $K
then
	echo $H deve essere strettamente inferiore $K
	exit 4
fi

shift
shift

#Inizializzo la variabile W dove inserisco nomi assoluti di directory
W=
for F  
do
	#controllo che il parametro $F sia un percoso assoluto
	case $F in
	/*)	#controllo che il parametro $F sia una directory attraversabile
		if test -d $F -a -x $F
		then
			echo "parametro $F corretto"
		else
			echo "parametro $F non una directory o non attraversabile"
			exit 5
		fi;;
	*)	echo "parametro $F non assoluto"
		exit 5;;
	esac
	#inserisco il parametro nella variabile W
	W="$W $F"
done

#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH

#ciclo le W fasi
for G in $W
do
	FCR.sh $G $H $K
done