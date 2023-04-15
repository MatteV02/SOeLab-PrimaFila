#!/bin/sh

ger=$1	# all'interno di questa variabile memorizzo il nome della gerarchia da esplorare
Word1=$2	# all'interno di questa variabile memorizzo il valore della prima estensione
Word2=$3	# all'interno di questa variabile memorizzo il valore della seconda estensione
tempFile1=$4	# all'interno di questa variabile memorizzo il percorso del file temporaneo relativo a Word1
tempFile2=$5	# all'interno di questa variabile memorizzo il percorso del file temporaneo relativo all'estensione Word2

# echo DEBUG-GER\=$ger

cd $ger	# mi posiziono sulla gerarchia da esplorare

# scorro tutti gli elementi all'interno della gerarchia attuale che terminano con .Word1
for H in *.$Word1
do	if test -f $H -a -r $H	# controllo che l'elemento in questione sia effettivamente un file leggibile
	then	echo `pwd`/$H >> $tempFile1	# memorizzo il nome assoluto del file trovato sul file temporaneo che raccoglie i file con estensione Word1
	fi
done

# scorro tutti gli elementi all'interno della gerarchia attuale che terminano con .Word2
for H in *.$Word2
do      if test -f $H -a -r $H		# controllo che l'elemento in questione sia effettivamente un file leggibile
        then    echo `pwd`/$H >> $tempFile2		# memorizzo il nome assoluto del file trovato sul file temporaneo che raccoglie i file con estensione Word2
        fi
done

for i in *
do	if test -d $i -a -x $i
	then	$0 $ger/$i $Word1 $Word2 $tempFile1 $tempFile2	# chiamo ricorsivamente la funzione di ricerca in ogni sottodirectory traversabile della gerarchia attuale
	fi
done
