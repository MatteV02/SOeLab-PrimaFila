#!/bin/sh

# controllo che vengano passati almeno 3 parametri
if test $# -lt 3
then	echo "Errore: devi passare almeno 3 parametri"
	exit 1
fi

# controllo che il primo parametro e lo memorizzo sulla variabile di seguito
X=
case $1 in
*[!0-9]*)	echo "Errore: il primo parametro è non numerico o non strettamente positivo"	# se il primo parametro contiene caratteri non numerici allora esso è non numerico o non strettamente positivo
		exit 2;;
*)		if test ! `expr $1 % 2` -eq 1
		then 	echo "Errore: il primo parametro non è strettamente positivo o non è dispari"	# se il primo parametro è divisibile per 2 allora non è dispari o è 0
			exit 3
		fi
		X=$1
		shift;;	# compio lo shift della lista dei paramtri in modo che ora contenga potenzialmente sole gerarchie di directory
esac

# controllo che gli altri parametri oltre al primo siano nomi assoluti di directory
for i
do	case $i in
	/*)	# in questo caso il parametro rappresenta un percorso assoluto
		if test ! -d $i -o ! -x $i	# se il parametro non è directory o non è traversabile esco con errore
		then	echo "Errore: Il parametro $i non è directory o non è traversabile"
			exit 4
		fi;;
	*)	# in questo caso il primo parametro non rappresenta un percorso assoluto
		echo "Errore: il parametro $i non rappresenta un percorso assoluto"
		exit 5;;
	esac
done

G=	# Questa variabile memorizzerà la gerarchia che viene presa in considerazione

# imposto il PATH in modo da eseguire correttamente la funzione ricorsiva
PATH=`pwd`:$PATH
export PATH

for G
do	# invoco la funzione ricorsiva
	FCR.sh $G $X
done
