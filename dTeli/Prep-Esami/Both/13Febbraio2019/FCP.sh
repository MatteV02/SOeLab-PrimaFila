#!/bin/sh
#controllo se il numero di parametri e maggiore o uguale a 3
if test $# -lt 3
then
	echo Numero di parametri errato, inseriti $# devono essere maggiori o uguali a 3
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
shift

#Inizializzo la variabile F dove inserisco nomi semplici di file inseriti
Files=
for F  
do
	#controllo che il parametro $F sia in forma semplice
	case $F in
	*/*)	echo "parametro $F deve essere semplice"
		exit ExitValue;;
	esac
	echo "parametro $F corretto"
	#inserisco il parametro nella variabile Files
	Files="$Files $F"
done

#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH

FCR.sh $G $Files