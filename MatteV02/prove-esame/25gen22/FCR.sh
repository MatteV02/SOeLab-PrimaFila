#!/bin/sh

G=$1	# memorizzo all'interno di G il nome della directory da esplorare

cd $G	# mi posiziono sulla cartella da esplorare

E1=$2	# memorizzo all'interno di E1 la prima estensione passata
E2=$3   # memorizzo all'interno di E2 la seconda estensione passata

cont1=0	# numero di occorrenze all'interno della directory di file che terminano con la prima estensione
cont2=0 # numero di occorrenze all'interno della directory di file che terminano con la prima estensione

# esploro la directory alla ricerca di file che contengono le estensioni E1 o E2
for F in *
do	case $F in
	*.$E1)
		# controllo che il file sia leggibile
		if test -f $F -a -r $F
		then	cont1=`expr $cont1 + 1`	# aggiorno la variabile cont1
		fi
		;;
	*.$E2)	if test -f $F -a -r $F -a -x	# controllo che il file sia leggibile e scrivibile
		then	cont2=`expr $cont1 + 1`	# aggiorno la variabile cont2
		fi
		;;
	*)	;;	# non faccio nulla
	esac
done

# controllo se ho trovato almeno un file con estensione .E1 e un file con estensione .E2
if test $cont1 -gt 0 -a $cont2 -gt 0
then	# stampo il nome assoluto della directory attuale
	pwd

	# scorro tutti i file che hanno estensione E2
	for F in *.$E2
	do
		# invoco la parte in C
		echo 25gen23 *.$E1 $F
	done
fi

# scorro la directory corrente alla ricerca di sottodirectory traversabili nelle quali eseguire la ricerca ricorsiva
for i in *
do	if test -d $i -a -x $i
	then	$0 $G/$i $E1 $E2
	fi
done
