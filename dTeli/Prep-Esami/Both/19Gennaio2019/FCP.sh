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
	if test `expr $1 % 2` -eq 1
	then
		echo "il parametro $1 deve essere pari"
		exit 2
	fi
	#controllo 0
	if test $1 -eq 0
	then
		echo "il parametro $1 deve essere strettamente positivo"
		exit 2
	fi;;
esac
#inserisco il parametro nella variabile C
X=$1
shift
#controllo se il parametro $2 e un numero strettamente positivo
case $1 in
*[!0-9]*)
	echo "il parametro $1 deve essere un numero"
	exit 3;;
*)
	if test `expr $1 % 2` -eq 0 -a ! $1 -eq 0
	then
		echo "il parametro $1 deve essere dispari"
		exit 3
	fi
	#controllo 0
	if test $1 -eq 0
	then
		echo "il parametro $1 deve essere strettamente positivo"
		exit 2
	fi;;
esac
#inserisco il parametro nella variabile X
C=$1
shift
#Inizializzo la variabile G dove inserisco percorsi assoluti
G=
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
			exit 3
		fi;;
	*)	echo "parametro $F non assoluto"
		exit 3;;
	esac
	#inserisco il parametro nella variabile G
	G="$G $F"
done

#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH	
> /tmp/nomiAssoluti
#for delle Q fasi
for Q in $G
do
	FCR.sh $Q $C $X /tmp/nomiAssoluti
done

echo "il numero di file trovati `wc -l </tmp/nomiAssoluti`"

#Inizializzo la variabile Out dove inserisco file trovati da FCR
Out=
for i in `cat </tmp/nomiAssoluti`
do
	Out="$Out $i"
done
echo
main $Out $C
rm /tmp/nomiAssoluti