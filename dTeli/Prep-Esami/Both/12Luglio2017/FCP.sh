#!/bin/sh

#controllo se il numero di parametri e' strettamente maggiore di 2
if test $# -le 2
then
	echo Numero di parametri errato, inseriti $# devono essere strettamente maggiori di 2
	exit 1
fi

#controllo se il parametro $1 e un numero strettamente positivo
case $1 in
*[!0-9]*)
	echo "il parametro $1 deve essere un numero"
	exit 3;;
*)
	if test $1 -eq 0 -o $1 -gt 100
	then
		echo "il parametro $1 deve essere strettamente positivo"
		exit 2
	fi;;
esac
#inserisco il parametro nella variabile L
L=$1
shift
#controlla il resto dei parametri inseriti se sono file assoluti
#Inizializzo la variabile G dove inserisco noi assoluti dei file
G=
for i
do
	#controllo che il parametro i sia un percoso assoluto
	case $i in
	/*)	#controllo che il parametro i sia una directory attraversabile
		if test -d $i -a -x $i
		then
			echo "parametro $i corretto"
			G="$G $i"
		else
			echo "parametro $i non una directory o non attraversabile"
			exit 3
		fi;;
	*)	echo "parametro $i non assoluto"
		exit 3;;
	esac
done

#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH	

#creo file temporanio
> /tmp/filetemp

#ciclo le W fasi
for W in $G
do
	FCR.sh $W $L /tmp/filetemp
done

#Inizializzo la variabile Out dove inserisco file trovati dal FCR
Out=
for i in `cat </tmp/filetemp`
do
	Out="$Out $i"	
done
echo 
main $Out
rm /tmp/filetemp