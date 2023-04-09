#!/bin/sh

# Controllo il numero di parametri passati
case $# in
2) 	echo "DEBUG-il numero di parametri passati è corretto"
	;;
*)	echo "Devi passare 2 parametri"
	exit 1
	;;
esac

case $1 in	# Controllo che il primo parametro passato rappresenti un percorso assoluto
/*)	echo "DEBUG-il primo paramtro rappresenta un percorso assoluto"
	if test ! -d $1 -o ! -x $1	# Controllo che il primo prametro passato rappresenti una directory traversabile
	then	echo "Il primo parametro passato \"$1\" non rappresenta una directory traversabile"
		exit 2
	fi
	;;
*)	echo "Il primo parametro passato non rappresenta un percorso assoluto"
	exit 2
	;;
esac

case $2 in	# Controllo che il secondo parametro rappresenti un numero intero strettamento positivo.
*[!0-9]*)	echo "Il secondo parametro passato non rappresenta un numero intero strettamente positivo"
		exit 3
		;;
*)		if test $2 -eq 0
		then	echo "Il secondo parametro passato non rappresenta un numero intero strettamente positivo"
			exit 3
		fi
		;;
esac

PATH=`pwd`:$PATH	# Imposto il PATH in modo che FCP e FCR siano ricercabili
export PATH

FCR.sh $1 $2	# Lancio la funzione ricorsiva
