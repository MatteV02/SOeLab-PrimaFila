#!/bin/sh

if test $# -lt 4
then	echo "Errore: devi passare almeno 4 parametri"
	exit 1
fi

H=	# in questa variabile memorizzo il primo parametro
M=	# in questa variabile memorizzo il secondo parametro

case $1 in	# controllo il primo parametro
*[!0-9]*)	echo "Errore: il parametro $1 non è un numero strettamente positivo"
		exit 2;;
*)		# controllo che il parametro sia diverso da 0
		if test $1 -eq 0
		then	echo "Errore: il parametro $1 non è un numero strettamente positivo"
			exit 3
		fi
		H=$1	# assegno ad H il primo parametro
		;;
esac

case $2 in      # controllo il secondo parametro
*[!0-9]*)       echo "Errore: il parametro $2 non è un numero strettamente positivo"
                exit 2;;
*)              # controllo che il parametro sia diverso da 0
                if test $1 -eq 0
                then    echo "Errore: il parametro $2 non è un numero strettamente positivo"
                        exit 3
                fi
                M=$2    # assegno ad M il primo parametro
                ;;
esac

shift 2	# elimino i primi due parametri dalla lista dei parametri

for G
do	# controllo che gli altri parametri rappresentino nomi assoluti di directory
	case $G in
	/*)	if test ! -d $G -o ! -x $G
		then	echo "Errore: il parametro $G non è directory o non è traversabile"
			exit 4
		fi
		;;
	*)	echo "Errore: il parametro $G non è un nome assoluto di directory"
		exit 5;;
	esac
done

# imposto il PATH
PATH=`pwd`:$PATH
export PATH

# invoco la funzione ricorsiva su ognuna delle gerarchie passate come parametro
for G
do	echo invoco la funzione ricorsiva sulla directort $G
	FCR.sh $G $H $M
done
