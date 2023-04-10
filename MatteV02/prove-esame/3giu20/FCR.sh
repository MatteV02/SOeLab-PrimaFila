#!/bin/sh

cd $1	# mi posiziono sulla directory da esplorare

C=$2	# nella variabile C memorizzo il carattere passato alla funzione principale

# scorro il contenuto della directory alla ricerca di sottodirectory con nome lungo 3 caratteri rispondenti alla specifica dell'esercizio
for i in *
do	if test -d $i	# se l'elemento attuale è una directory controllo se il suo nome corrisponde alla specifica dell'esercizio
	then	case $i in
		$C?$C)	# in questo caso l'elemento attuale corrisponde alla specifica
			echo `pwd`/$i >> $3	# memorizzo il percorso assoluto della directory sul file temporaneo
			;;
		*)	;;	# in caso contrario non faccio nulla
		esac
	fi
done

# scorro nuovamente il contenuto della directory alla ricerca di sottodirectory traversabili nelle quali lanciare il programma ricorsivo
for i in *
do	if test -d $i -a -x $i
	then	# l'elemento attuale è una directory traversabile quindi lancio al suo interno il programma ricorsivo
		$0 `pwd`/$i $2 $3
	fi
done
