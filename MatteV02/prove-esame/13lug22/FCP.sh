#!/bin/sh

# controllo che vengano passati almeno 4 parametri
if test $# -lt 4
then	echo "Errore: devi passare almeno 4 parametri"
	exit 1
fi


# controllo che il primo parametro rappresenti un nome assoluto di directory traversabile
case $1 in
/*)	;;
*)	echo "Errore: il primo parametro non è un persorso assoluto"
	exit 2;;
esac

if test ! -d $1 -a ! -x $1
then	echo "Errore: il primo parametro non rappresenta una directory traversabile"
	exit 3
fi

G=$1	# memorizzo il primo parametro all'interno della variabile G


# controllo che il secondo parametro rappresenti un numero interno strettamente positivo
case $2 in
*[!0-9]*)	echo "Errore: il secondo parametro non rappresenta un numero o non è un numero intero strettamente positivo"
		exit 4;;
*)		if test $2 -eq 0
		then 	echo "Errore: il secondo parametro dev'essere un numero intero strettamente positivo!"
			exit 5
		fi;;
esac

L=$2	# memorizzo il secondo parametro all'interno della variabile L
shift 2	# elimino i primi 2 parametri dalla lista dei parametri

# controllo che i successivi parametri siano singoli caratteri
for i
do	case $i in
	?)	;;
	*)	echo "Errore: il parametro $i non è un singolo carattere"
		exit 6;;
	esac
done


tempFile=/tmp/nomiAssoluti	# memorizzo in questa variabile il nome del file temporaneo, nel quale memorizzerò i file trovati
> $tempFile	# inizializzo il file temporaneo

# imposto il PATH
PATH=`pwd`:$PATH
export PATH

# invoco la funzione ricorsiva
FCR.sh $G $L $tempFile $*

echo	# stampo una linea vuota
echo Sono stati trovati in totale `wc -l < $tempFile` files
echo Eseguo la parte C
echo	# stampo una linea vuota

# invoco la parte in C per ogni file trovato
for F in `cat < $tempFile`
do	echo 13lug22 $F $L $*
done


rm $tempFile	# rimuovo il file temporaneo
