#!/bin/sh

G=$1	# memorizzo all'interno di questa variabile la directory da analizzare
S=$2	# memorizzo all'interno di questa variabile la stringa dell'estensione da cercare
tempFile=$3	# memorizzo all'interno di questa variabile il file temporaneo sul quale scrivere
directoryTrovata=false

cd $G

echo DEBUG-$G \: *.$S

for F in *.$S
do	if test -f $F
	then	echo DEBUG-directory trovata
		directoryTrovata=true
		break
	fi
done

echo DEBUG-$directoryTrovata
if test $directoryTrovata = true
then	# se l'espansione di *.$S non è nulla allora la directory corrente rispetta le specifiche
	echo `pwd` >> $tempFile	# memorizzo la directory corrente su file
fi

for i in *
do	# eseguo la chiamata ricorsiva sulle sottodirectory traversabili
	if test -d $i -a -x $i
	then	$0 `pwd`/$i $S $tempFile
	fi
done
