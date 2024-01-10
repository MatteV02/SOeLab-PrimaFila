#!/bin/sh

#controllo se il numero di parametri e maggiore o uguale a 4
if test $# -lt 4
then
	echo Numero di parametri errato, inseriti $# devono essere maggiori o uguali a 4
	exit -1
fi

#controllo che il parametro $1 sia in forma semplice
case $1 in
*/*)	echo "parametro $1 deve essere semplice"
	exit 1;;
esac
echo "parametro $1 corretto"
#inserisco il parametro nella variabile D
D=$1

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
#inserisco il parametro nella variabile Y
Y=$2

shift
shift

#Inizializzo la variabile W dove inserisco path assoluti dei direttori
W=
for G  
do
	#controllo che il parametro $G sia un percoso assoluto
	case $G in
	/*)	#controllo che il parametro $G sia una directory attraversabile
		if test -d $G -a -x $G
		then
			echo "parametro $G corretto"
		else
			echo "parametro $G non una directory o non attraversabile"
			exit 3
		fi;;
	*)	echo "parametro $G non assoluto"
		exit 3;;
	esac
	#inserisco il parametro nella variabile W
	W="$W $G"	
done

#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH

>/tmp/temp

#ciclo le W fasi

for G in $W
do
	FCR.sh $G $D $Y /tmp/temp 0
	echo "il numero di file trovati in $G e' $?"
	#Inizializzo la variabile Gers dove inserisco file trovati nella gerachia G dal FCR
	Files=
	for F in `cat </tmp/temp`
	do
		Files="$Files $F" 
	done
	main $Files $Y
	>/tmp/temp
done

rm /tmp/temp