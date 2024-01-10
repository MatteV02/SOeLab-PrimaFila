#!/bin/sh

#controllo se il parametro $1 e un numero strettamente positivo

#controllo se il numero di parametri e maggiore o uguale a 2
if test $# -lt 2
then
	echo Numero di parametri errato, inseriti $# devono essere maggiori o uguali a 2
	exit ExitValue
fi

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
#inserisco il parametro nella variabile L
L=$1

shift

#Inizializzo la variabile Q dove inserisco nomi assoluti di directory
Q=
for i  
do
	#controllo che il parametro $i sia un percoso assoluto
	case $i in
	/*)	#controllo che il parametro $i sia una directory attraversabile
		if test -d $i -a -x $i
		then
			echo "parametro $i corretto"
		else
			echo "parametro $i non una directory o non attraversabile"
			exit 3
		fi;;
	*)	echo "parametro $i non assoluto"
		exit 3;;
	esac
	#inserisco il parametro nella variabile Q
	Q="$Q $i"
done

#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH

> /tmp/nomiAssoluti$$

#ciclo le Q fasi
for G in $Q
do
	FCR.sh $G $L /tmp/nomiAssoluti$$

	N=`wc -l </tmp/nomiAssoluti$$`
	echo numero totale di file trovati in $G=$N
	#chiamo la parte c per ogni file trovato nella gerachia esplorata dal FCR
	for i in `cat </tmp/nomiAssoluti$$`
	do
		main $i $L
	done
done


rm /tmp/nomiAssoluti$$