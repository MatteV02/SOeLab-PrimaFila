#!/bin/sh

G=$1	# memorizzo la gerarchia attuale
F=	# in questa variabile memorizzerò di volta in volta il nome del file che prendo in considerazione
X=$2	# in questa variabile memorizzo il valore numerico passato dall'utente alla funzione principale
count=1	# attraverso questo contatore noterò quali file si trovano in posizione dispari
directoryTrovata=false	# attraverso questa variabile saprò se ho trovato almeno un file corrispondente alla specifica in questa directory (e quindi se devo invocare la parte C)
files=	# questa variabile conterrà tutti i file corrispondenti alla specifica nella directory attuale

cd $G	# imposto la working directory sulla gerarchia attuale

for F in *
do	# se l'elemento attuale è un file in posizione dispari
	if test -f $F -a `expr $count % 2` -eq 1
	then	# controllo se la lunghezza del file corrisponde a X ed eventualmente stampo il percorso assoluto del file trovato
		if test `wc -l < $F` -eq $X
		then
			echo File corrispondente alla specifica \: `pwd`/$F
			directoryTrovata=true	# memorizzo che ho individuato una directory sulla quale eseguire la parte in C
			files="$files `pwd`/$F"	# memorizzo il file trovato nella lista dei file trovati
		fi
	fi
	count=`expr $count + 1`	# incremento la variabile count
done

if test $directoryTrovata = true
then	# se la directory risponde alle specifiche invoco la parte C
	echo Invoco la parte C con parametri $files
	echo 9set22 $files
fi

# scorro nuovamente la directory alla ricerca di directory nelle quali eseguire ricorsivamente la ricerca
for F in *
do	if test -d $F -a -x $F
	then	$0 `pwd`/$F $X
	fi
done
