#!/bin/sh

case $# in
4)	echo DEBUG-il numero di parametri passati è corretto
	;;
*)	echo DEBUG-il numero di parametri passati è sbagliato
	exit 1
	;;
esac

G= # gerarchia di directory
Cx= # carattere

case $1 in
/*)	echo DEBUG-il primo parametro è un percorso assoluto
	# Controllo $1 sia una directory traversabile
        if test ! -d $1 -o ! -x $1
        then    echo DEBUG-il primo parametro nonrappresenta una directory traversabile
                exit 3
        fi
        G=$1
	;;
*)	echo DEBUG il primo parametro non è un percorso assoluto
	exit 4
	;;
esac

case $4 in
?)	echo DEBUG-ultimo parametro è un singolo carattere
	Cx=$4
	;;
*)	echo l\'ultimo parametro non è un singolo carattere
	exit 3
	;;
esac

set $2 $3

for i
do	case $i in
	*[!0-9]*)	echo il parametro $i non è un numero
			exit 5
			;;
	*)	if test $i -le 0
		then	echo DEBUG-$i è non positivo
			exit 6
		fi
		;;
	esac
done

PATH=`pwd`:$PATH
export PATH

echo $*

FCR.sh $G $1 $2 $Cx

