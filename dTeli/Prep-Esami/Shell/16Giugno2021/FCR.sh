#!/bin/sh

#cambio la cartella corrente esplorando la nuova cartella
cd $1
#esploro la directory corrente
#variabile per il conteggio dei file
cont=0
#variabile per contenere la lista dei file trovati
files=
for F in *
do
	#controllo se file e leggibile
	if test -f $F -a -r $F
	then	#controllo se il nome del file e di lungezza 2
		case $F in
		??)
			#esamino se le linee sono conformi alle richeste
			if test `wc -l <$F` -eq $3
			then
				#aumento il contatore e salvo il file trovato
				cont=`expr $cont + 1`
				files="$files $F"
			fi;;
		esac
	else
		#se trovo una cartella e attraversabile richiamo la ricosiva per esplorarla
		if test -d $F -a -x $F
		then
			FCR.sh `pwd`/$F $2 $3
		fi
	fi
done

if test $cont -lt $2 -a $cont -gt 2
then
	echo la directory `pwd`
	echo parte.c $files
fi
