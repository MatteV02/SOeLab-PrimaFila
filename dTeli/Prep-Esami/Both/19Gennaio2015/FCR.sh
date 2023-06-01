#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
C=$2
temp=$3
#esploro la directory
for F in *
do
	#esiste una istanza del carattere C nel file
	if `grep $C <$F >/dev/null 2>&1`
	then
		echo `pwd`/$F >>$temp
	fi
done

#cerco nuove directory da esplorare
for F in *
do
	#Se F e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
	if test -d $F -a -x $F
	then
		FCR.sh `pwd`/$F $C $temp
	fi
done