#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
Cz=$2
temp=$3
#esploro la directory
for F in *
do
	#controllo se $F e un file leggibile
	if test -f $F -a -r $F
	then
		#controllo se c'e almeno una ricorrenza di Cz nel file
		if `grep $Cz $F >/dev/null 2>&1`
		then
			echo trovato file `pwd`/$F
			echo `pwd`/$F >>$temp
		fi
		
	fi
done

#cerco nuove directory da esplorare
for F in *
do
	#Se F e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
	if test -d $F -a -x $F
	then
		FCR.sh `pwd`/$F $Cz $temp
	fi
done