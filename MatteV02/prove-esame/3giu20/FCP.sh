#!/bin/sh

if test $# -lt 3
then	echo "Errore: devi passare almeno 3 parametri"
	exit 1
fi

C= # in questa variabile memorizzo il primo parametro passato

case $1 in
?)	C=$1;;	# controllo che il primo parametro consista in un solo carattere, nel caso lo salvo nella variabile C
*)	echo "Errore: il primo parametro passato deve essere un carattere singolo!"
	exit 2;;
esac

shift	# elimino il primo parametro dalla lista, ora controllo che gli altri parametri rappresenti nomi assoluti di directory

for i
do	case $i in
	/*)	;;	# controllo che il parametro memorizzato in i rappresenti un percorso assoluto
	*)	echo "Errore: il parametro $i non rappresenta un nome assoluto!"
		exit 3;;
	esac

	if test ! -d $i -o ! -x $i	# controllo che il parametro memorizzato in i rappresenti una directory traversabile
	then	echo "Errore: il parametro $i non rappresenta una directory traversabile!"
		exit 4
	fi
done


tempFile=/tmp/nomiAssoluti	# all'interno di questa variabile memorizzo il nome assoluto del file temporaneo necessario per la parte ricorsiva dell'esercizio
> $tempFile			# inizializzo il file temporaneo

# imposto il PATH
PATH=`pwd`:$PATH
export PATH

for i
do	# invoco la funzione ricorsiva
	FCR.sh $i $C $tempFile
done

echo "Sono state trovate globalmente `wc -l < $tempFile` directory corrispondenti alla specifica"
echo "Listo le directory trovate"

answer=	# in questa variabile memorizzerò il valore inserito dall'utente
# scorro il contenuto del file temporaneo e per ogni directory consento la scelta dell'utente
for i in `cat < $tempFile`
do	echo $i
	echo -n "Elena, vuoi visualizzare il contenuto della directory $i compresi gli elementi nascosti? (y/n)> " > /dev/tty
	read answer

	# controllo l'input dell'utente
	case $answer in
	y)	# stampo il contenuto della directory compresi gli elementi nascosti
		echo "Mostro il contenuto della directory $i:"
		ls -A $i
		;;
	*)	;;	# in questo caso non faccio niente
	esac
done

rm $tempFile	# rimuovo il file temporaneo
