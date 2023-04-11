#!/bin/sh

# Controllo il numero dei parametri
case $# in
3)	;;	# numero dei parametri corretto
*)	echo "Errore, devi passare 3 parametri"
	exit 1
	;;
esac

G=	# in questa variabile memorizzerò il primo parametro

# controllo che il primo parametro rappresenti una nome assoluto di directory traversabile
case $1 in
/*)	if test ! -d $1 -o ! -x $1
	then	echo "Errore, il primo parametro non è directory o non è traversabile"
		exit 2
	fi
	G=$1	# memorizzo il primo parametro all'interno della variabile G
	shift	# elimino il primo parametro dalla lista dei parametri
	;;
*)	echo "Errore, il primo parametro non rappresenta un percorso assoluto"
	exit 3;;
esac

# controllo che i parametri successivi non contengano /
for i
do	case $i in
	*/*)	echo "Errore, il parametro $i contiene il carattere /"
		exit 4;;
	*)	;;	# la stringa ha un formato corretto
	esac
done

# imposto il PATH
PATH=`pwd`:$PATH
export PATH

# invoco la funzione ricorsiva
FCR.sh $G $1 $2
