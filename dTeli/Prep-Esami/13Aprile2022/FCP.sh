#!/bin/sh
#controllo il numero di parametri
if test ! $# -ge 3
then
	echo numero errato di parametri
	exit 1
fi
#controllo se primo parametro intero e positivo
case $1 in
[!0-9])	echo primo parametro non un numero
	exit 2;;
0)	echo primo parametro non puo essere 0;;
esac

X=$1

#preparo i parametri per il controllo delle gerarchie
shift
G=
for i
do
	case $i in
	/*)	#controllo se la dir assoluta esista ed attraversabile
		if test -d $i -a -x $i
		then
			G="$G $i"
		fi
	esac
done



#inserisco la dir corrente nel path
PATH=`pwd`:$PATH
export PATH

#variabile di conteggio per distiguere i file temporanei
conta=1
#inizio scorrimento fasi
for i in $G
do
	#genero file temporaneo per il conteggio e la memorizzazioni delle dir trovate
	touch /tmp/nomiAssoluti-$conta
	FCR.sh $i $X /tmp/nomiAssoluti-$conta
	echo per la gerarchia corrispondente si sono trovati `wc -l </tmp/nomiAssoluti-$conta`
	conta=`expr $conta + 1`
done
#resetto il contatore
conta=2

while test $conta -le $#
do
	for F1 in `cat </tmp/nomiAssoluti-1`
	do
		for F2 in `cat </tmp/nomiAssoluti-$conta` 
		do
			case `cat <$F2` in
			`cat <$F1`)	echo i file $F1 e $F2 sono uguali;;
			esac
		done
	done
	rm /tmp/nomiAssoluti-$conta
	conta=`expr $conta + 1`
done

rm /tmp/nomiAssoluti-1


