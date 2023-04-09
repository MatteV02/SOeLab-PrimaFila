#!/bin/sh

# Controllo che vengano passati almeno 4 parametri
if test $# -lt 4
then	echo "Errore: devi passare almeno 4 parametri"
	exit 1
fi

W= 	# in questa variabile memorizzo il primo parametro
S=	# in questa variabile memorizzo il secondo parametro

# Controllo che il primo paramtro sia un numero intero strettamente positivo e lo memorizzo nella variabile W
case $1 in
*[!0-9]*)	echo "Errore: Il primo parametro non è un numero o non è un numero strettamente positivo!"
		exit 2;;
*)		if test $1 -eq 0
		then 	echo "Errore: Il primo parametro non è un numero intero strettamente positivo"
			exit 3
		fi
		W=$1;;
esac


# Controllo che il secondo parametro sia una stringa che rappresenti un nome senza simboli
case $2 in
*[!0-9a-zA-Z]*)	echo "Errore: il secondo parametro non può rappresentare un nome di estensione"
		exit 4;;
*)		# memorizzo il secondo parametro
		S=$2;;
esac

shift 2	# elimino dalla lista dei parametri i parametri che mi sono già salvato da parte

for G
do	# Controllo che i parametri rimanenti rappresentino nomi assoluti di directory traversabili
	case $G in
	/*)	;;
	*)	echo "Errore: il parametro $i non rappresenta un nome assoluto"
		exit 5;;
	esac
	if test ! -d $G -o ! -x $G
	then 	echo "Errore: $i non directory o non traversabile"
		exit 6
	fi
done

tempFile=/tmp/nomiAssoluti	# memorizzo all'interno di questa variabile il file temporaneo
> $tempFile	# Inizializzo il file temporaneo

# imposto il PATH
PATH=`pwd`:$PATH
export PATH

for G
do	# scorro la lista delle gerarchie e lancio la funzione ricorsiva
	FCR.sh $G $S $tempFile
done

dirTrovate=`wc -l < $tempFile`	# Memorizzo all'interno di questa variabile il numero di directory trovate globalmente da parte della funzione ricorsiva
echo Sono state trovate globalmente $dirTrovate directory

if test $dirTrovate -ge $W
then	echo -n "Utente `whoami`, inserisci un numero compreso tra 1 e $W: " > /dev/tty
	X=	# in questa variabile memorizzo la scelta dell'utente
	read X

	# controllo che il valore inserito sia un numero intero compreso tra 1 e W
	case $X in
	*[!0-9]*)	echo "Errore: hai inserito un valore sbagliato" > /dev/tty
			exit 7;;
	*)		if test $X -ge 1 -a $X -le $W
			then	# seleziono la directory corretta e la stampo su schermo
				directorySelezionata=	# in questa variabile memorizzo la directory selezionata
				count=1	# in questa variabile conto quante directory percorro
				for i in `cat < $tempFile`
				do	if test $count -eq $X
					then	# questa è la directory da memorizzare
						directorySelezionata=$i
					fi
					count=`expr $count + 1`	# aggiorno il contatore
				done
				echo Stampo la directory selezionata $directorySelezionata
			fi;;
	esac
fi

rm $tempFile
