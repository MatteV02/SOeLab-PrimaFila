#!/bin/sh

cd $1	# mi posiziono sulla directory corrente

X=$2	# memorizzo in X il secondo parametro passato

# scorro il contenuto della directory corrente
for F in *
do	if test -f $F -a -r $F	# se F è il nome di un file leggibile
	then	if test `wc -l < $F` -eq $X	# confronto il numero di linee del file con la variabile X
		then	echo `pwd`/$F >> $3	# memorizzo il nome assoluto del file trovato sul file temporaneo passato come terzo parametro
		fi
	fi
done

# scorro nuovamente il contenuto della directory corrente alla ricerca di sottodirectory  traversabile nelle quali eseguire la chiamata ricorsiva
for i in *
do	if test -d $i -a -x $i
	then	$0 `pwd`/$i $X $3
	fi
done
