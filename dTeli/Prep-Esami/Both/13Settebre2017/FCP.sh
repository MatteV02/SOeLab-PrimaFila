#!/bin/sh


#controllo se il numero di parametri e maggiore o uguale a 2
if test $# -lt 2
then
	echo Numero di parametri errato, inseriti $# devono essere maggiori o uguali a 2
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
#inserisco il parametro nella variabile X
X=$1
shift

#Inizializzo la variabile F dove inserisco i path assoluti passati
F=
W=
for F  
do
	#controllo che il parametro $i sia un percoso assoluto
	case $F in
	/*)	#controllo che il parametro $i sia una directory attraversabile
		if test -d $F -a -x $F
		then
			echo "parametro $F corretto"
		else
			echo "parametro $F non una directory o non attraversabile"
			exit 3
		fi;;
	*)	echo "parametro $F non assoluto"
		exit 3;;
	esac
	#inserisco il parametro nella variabile G
	W="$W $F"
done

#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH

#ciclo le W fasi
for G in $W
do
	FCR.sh $G $X
done