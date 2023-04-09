#!/bin/sh
#cambio directory nella directory da esaminare
cd $1
#genero una variabile che segnala se un file che rispetta le richieste e stato trovato
flag=0
#scorro la dir corrente
for F in *
do
	#controllo se file e che non sia gia stato trovato un file che rispetta le richieste
	if test -f $F -a $flag -eq 0
	then
		#controllo che il nome del file ripsetti le richieste
		case $F in
		*.$2)	flag=1
			echo `pwd` >>$3;;
		esac
	else
		#controllo se dir ed attraversabile
		if test -d $F -a -x $F
		then	#richiamo la funzione ricorsiva per esplorare la dir trovata
			FCR.sh `pwd`/$F $2 $3
		fi
	fi
done
