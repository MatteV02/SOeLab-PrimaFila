#!/bin/sh

#controllo se il numero di parametri uguale a 2
if test $# -ne 2
then
	echo Numero di parametri errato, inseriti $# devono essere 2
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
#inserisco il parametro nella variabile G
G=$1
#controllo se il parametro $2 sia un carattere
case $2 in 
?)
	echo parametro $2 corretto;;
*)
	echo parametro $2 errato
	exit -1;;
esac
Cx=$2

#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH
> /tmp/temp
FCR.sh $G $Cx /tmp/temp 

#controllo se le line contenute in `cat </tmp/temp` sono maggiori o uguali a NumeroLinee
if test `wc -l < /tmp/temp` -ge 2
then
	#Inizializzo la variabile files dove inserisco file che sono conformi alla specifia da FCR
	files=
	for i in `cat </tmp/temp`
	do
		files="$files $i"
	done
	echo main $files
fi

rm /tmp/temp