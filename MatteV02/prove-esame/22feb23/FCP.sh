#!/bin/sh

# controllo il numero di parametri
if test $# -lt 3
then 	echo "Errore: devi passare almeno 3 parametri"
	exit 1
fi

X= # in questa variabile memorizzo il primo parametro

# controllo che il primo parametro sia un numero intero strettamente positivo minore di 500
case $1 in
*[!0-9]*)	echo "Errore: il primo parametro non è un numero o non è strettamente positivo"
		exit 2;;
*)		;;
esac
if test $1 -eq 0 -o $1 -ge 500
then	echo "Errore: il primo parametro è 0 o non strettamente minore di 500"
	exit 3
fi

# memorizzo il primo parametro e lo elimino dalla lista dei parametri
X=$1
shift

# Controllo che gli altri parametri passati rappresentino nomi assoluti di directory traversabili
for i
do	# controllo che il parametro attuale sia un percorso assoluto
	case $i in
	/*)	;;
	*)	echo "Errore: il parametro $i non rappresenta un percorso assoluto"
		exit 4;;
	esac

	# controllo che il parametro attuale sia una directory esistente e traversabile
	if test ! -d $i -o ! -x $i
	then	echo "Errore: il parametro $i non è directory o non è traversabile"
		exit 5
	fi
done

# Inizializzo un file temporaneo che memorizzerà le occorrenze trovate dalla funzione ricorsiva
tempFile=/tmp/22feb23-$$
> $tempFile

# Imposto il PATH in modo da poter eseguire correttamente la funzione ricorsiva
PATH=`pwd`:$PATH
export PATH

# invoco la funzione ricorsiva
for i
do	FCR.sh $i $X $tempFile
done

# controllo se sono stati trovati almeno 2 file dalla funzione ricorsiva
if test `wc -l < $tempFile` -ge 2
then	echo Invoco la parte C
	echo 22feb23 `cat < $tempFile`
fi
