#!/bin/sh

G=$1	# Memorizzo in questa variabile la gerarchia da attraversare
X=$2	# Memorizzo in questa variabile il numero intero
F=	# Memorizzo in questa variabile volta per volta il nome dei file all'interno della gerarchia G

cd $G	# Mi posiziono sulla gerarchia corrente

for F in *
do	if test -f $F -a -r $F	# controllo se il nome attuale corrisponde a un file leggibile
	then	if test `wc -m < $F` -lt $X	# Se il numero di caratteri del file è strettamente minore di X allora procedo con la stampa del nome assoluto del file e la sua memorizzazione sul file temporaneo
		then 	echo "E' stato trovato il file `pwd`/$F"
			echo `pwd`/$F >> $3
		fi
	fi
done

for i in *
do	# scorro il contenuto della cartella alla ricerca di directory traversabili all'interno delle quali posso eseguire la chiamata ricorsiva
	if test -d $i -a -x $i
	then	$0 `pwd`/$i $X $3 	# invoco la funzione ricorsiva
	fi
done
