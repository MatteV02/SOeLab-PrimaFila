#!/bin/sh
#controllo numero di parametri >=2
if test ! $# -ge 2
then	echo numero parametri errato
	exit 1
fi
#controll primo parametro numero positivo 
case $1 in
*[!0-9]*)
	echo primo parametro non numero
	exit 2;;
esac
echo primo parametro corretto
X=$1
#esamino tutti gli altri elementi usando un for ed inserisco gli elementi nella lista G
#elimino il parametro gia esaminato da $* con shift
shift
G=
for i
do
	case $i in
	/*)	echo parametro corretto
		G="$G $i";;
	*)	echo parametro errato
		exit 2;;
	esac
done

PATH=`pwd`:$PATH
export PATH

#richiamo la funzione ricorsiva per tutte le fasi
#genero un file temporaneo che tiene traccia di tutti i file che rispettano i parametri
touch /tmp/files$$

for i in $G
do
	FCR.sh $i $X /tmp/files$$
done

#stampo il numero di file trovati
echo `wc -l </tmp/files$$` file trovati

for i in `cat /tmp/files$$`
do
	echo $i
	echo numero riga da visualizzare minore di $X
	read answer
	case $answer in
	[!0-9])
		echo inserimento non numero
		exit 3;;
	esac
	if test ! $answer -lt $X
	then
		echo inserimento errato
		exit 3
	fi
	echo riga richiesta
	echo `head -n $answer $i | tail -n 1`
done

rm /tmp/files$$
