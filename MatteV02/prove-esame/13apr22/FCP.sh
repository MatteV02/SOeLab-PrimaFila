#!/bin/sh

# controllo il numero dei parametri
if test $# -lt 3
then 	echo "Errore: devi passare almeno 3 parametri"
	exit 1
fi

X=	# all'interno di questa variabile memorizzo il primo parametro

# controllo che il primo parametro rappresenti un numero intero strettamente positivo
case $1 in
*[!0-9]*)	echo "Errore: il primo parametor non rappresenta un numero intero strettamente positivo"
		exit 2;;
*)		if test $1 -eq 0
		then	echo "Errore: il primo parametor non rappresenta un numero intero strettamente positivo"
			exit 3
		fi
		# memorizzo il primo parametro all'interno della variabile X
		X=$1
		shift;; 	# elimino il primo parametro dalla lista dei parametri
esac

for G in
do	# controllo che ogni altro parametro rappresenti un nome assoluto di directory
	case $G in
	/*)	if test ! -d $G -o ! -x $G
		then	echo "Errore: il parametro $G non è directory o non è traversabile"
			exit 4
		fi;;
	*)	echo "Errore: il parametro $G non rappresenta un nome assoluto di directory"
		exit 5;;
	esac
done

# imposto il PATH
PATH=`pwd`:$PATH
export PATH

count=1	# in questa variabile memorizzo volta per volta il numero di chiamate fatte della funzione ricorsiva

for G
do	tempFile=/tmp/nomiAssoluti-$count	# all'interno di questa variabile memorizzo il nome assoluto del file temporaneo che userò per contare globalmente i file rispondenti alle specifiche dell'esercizio
	> $tempFile	# creo o azzero il file temporaneo
	# chiamo la funzione ricorsiva
	FCR.sh $G $X $tempFile

	# stampo il numero di file trovati in questa gerarchia
	echo "Nella gerarchia $G ho trovato `wc -l < $tempFile` file"
	count=`expr $count + 1`
done

# per ogni file nella prima gerarchia controllo se ci sono file nelle altre gerarchie aventi lo stesso contenuto

count=2	# ripristino count
while test $count -le $#	# scorro le gerarchie da 2 a $#
do	# scorro i file creati nella prima gerarchia
	for F1 in `cat < /tmp/nomiAssoluti-1`
	do	# scorro i file creati nella count-esima gerarchia
		for F in `cat < /tmp/nomiAssoluti-$count`
		do	# controllo che il contenuto dei file combaci
			if diff $F $F1 > /dev/null 2>&1
			then	echo "I file $F1 e $F sono uguali"
			fi
		done
	done
	count=`expr $count + 1`
done


count=1	# ripristino count
for G
do	rm /tmp/nomiAssoluti-$count	#elimino il file temporaneo
	count=`expr $count + 1`
done

