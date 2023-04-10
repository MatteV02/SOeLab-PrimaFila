#!/bin/sh

if test $# -lt 3
then 	echo "Errore: devi passare almeno 3 parametri"
	exit 1
fi

# controllo che il primo parametro corrisponda ad un nome relativo semplice
case $1 in
*/*)	echo "Errore: il primo parametro non rappresenta un nome relativo semplice"
	exit 2;;
*)	;;
esac

# assegno il primo parametro alla variabile F e lo elimino dalla lista dei parametri
F=$1
shift

# controllo che gli altri parametri rappresentino nomi assoluti di directory
for i
do	# controllo che il parametro sia un percorso assoluto
	case $i in
	/*)	;;
	*)	echo "Errore: il parametro $i non rappresenta un nome assoluto"
		exit 3;;
	esac

	# controllo che il paraemtro sia una directory traversabile
	if test ! -d $i -o ! -x $i
	then	echo "Errore: il parametro $i non è una directory traversabile"
		exit 4
	fi
done

tempFile=/tmp/7apr17-$$	# imposto questa variabile con il nome assoluto del file temporaneo che verrà utilizzato successivamente
> $tempFile	# inizializzo il file temporaneo

# imposto il PATH
PATH=`pwd`:$PATH
export PATH

# invoco la funzione ricorsiva su ogni gerarchia passata come parametro
for i
do	FCR.sh $i $F $tempFile
done

# conto i file creati dalla funzione ricorsiva
echo "Sono stati creati `wc -l < $tempFile` file sorted"
echo "Procedo a stampare il nome assoluto di ognuno dei file creati mostrandone la prima e l'ultima linea"
echo	# stampo una linea vuota

# scorro il contenuto del file temporaneo
for i in `cat < $tempFile`
do	echo "Nome assoluto del file = $i"

	# controllo se il file è vuoto o meno
	if test `wc -c < $i` -eq 0
	then	# il file è vuoto
		echo "Il file è vuoto"
	else	echo -n "La prima linea del file è: "	# se il file non è vuoto provvedo a stamparne prima e ultima riga
		head -1 < $i
		echo -n "L' ultima linea del file è: "
		tail -1 < $i
	fi
	echo	# stampo una linea vuota
done

rm $tempFile	# elimino il file temporaneo
