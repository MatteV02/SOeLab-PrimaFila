#!/bin/sh

# controllo che il numero di parametri passati sia almeno 4
if test $# -lt 4
then	echo "Errore: devi passare almeno 4 parametri"
	exit 1
fi

# controllo che il primo parametro sia una stringa che non contiene caratteri slash
case $1 in
*/*)	echo "Errore: il primo parametro contiene il carattere /"
	exit 4;;
*)	;;	# altrimenti continuo il programma
esac

# controllo che il secondo parametro sia una stringa che non contiene caratteri slash
case $2 in
*/*)    echo "Errore: il secondo parametro contiene il carattere /"
        exit 5;;
*)      ;;      # altrimenti continuo il programma
esac

Word1=$1	# assegno il primo parametro a questa variabile
Word2=$2	# assegno il secondo parametro a questa variabile

shift 2		# elimino i primi 2 parametri dalla lista dei parametri

# controllo che gli altri parametri passati siano nomi assoluti di directory traversabili
for ger
do	case $ger in
	/*)	if test ! -d $ger -o ! -x $ger
		then	echo "Errore: il parametro $ger non rappresenta una directory traversabile"
			exit 2
		fi;;
	*)	echo "Errore: il parametro $ger non è un nome assoluto"
		exit 3;;
	esac
done

tempFile1=/tmp/Tempfile-1-$$	# memorizzo in questa variabile il percorso del file toemporaneo relativo ai file con estensione Word1
tempFile2=/tmp/Tempfile-2-$$	# memorizzo in questa variabile il percorso del file toemporaneo relativo ai file con estensione Word2
> $tempFile1	# inizializzo i file temporanei
> $tempFile2

# imposto il PATH
PATH=`pwd`:$PATH
export PATH

# eseguo per ogni gerarchia passata come parametro la ricerca ricorsiva
for ger
do	FCR.sh $ger $Word1 $Word2 $tempFile1 $tempFile2
done

Sum1=`wc -l < $tempFile1`	# immagazzino in questa variabile il numerp di file con estensione Word1
Sum2=`wc -l < $tempFile2`	# immagazzino in questa variabile il numerp di file con estensione Word2

echo "Sono stati trovati $Sum1 file con estensione .$Word1"
echo "Sono stati trovati $Sum2 file con estensione .$Word2"

if test $Sum1 -le $Sum2 -a $Sum1 -gt 0	# controllo che sum1 <= Sum2 e che sia stato trovato almeno un file con estensione Word1
then	Q=	# in questa variabile memorizzo l'input dell'utente
	echo "Matteo, inserisci un numero intero compreso tra 1 e $Sum1 > "
	read Q

	# controllo che Q sia un numero intero
	case $Q in
	*[!0-9]*)	echo "Errore, il valore inserito non è numerico o non è compreso tra 1 e $Sum1"
			exit 6;;
	*)		# controllo che il numero sia compreso tra 1 e Sum1
			if test $Q -lt 1 -o $Q -gt $Sum1
			then	echo "Errore, il valore inserito non è compreso tra 1 e $Sum1"
				exit 7
			fi;;
	esac

	# stampo i file in posizione Q-esima all'interno dei file
	echo "Riporto i nomi assoluti dei file in posizione $Q:"
	echo -n ".$Word1: "
	head -$Q < $tempFile1 | tail -1
	echo -n ".$Word2: "
	head -$Q < $tempFile2 | tail -1
fi

# elimino i file temporanei
rm $tempFile1
rm $tempFile2
