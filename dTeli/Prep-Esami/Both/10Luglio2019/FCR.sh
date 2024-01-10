#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
Cz=$2
temp=$3
flag=0
#esploro la directory
for F in *
do
	#controllo se $F e un file leggibile
	if test -f $F -a -r $F
	then
		#controllo se ci sono almeno due istanze di Cz
		if test `grep -c $Cz $F` -ge 2
		then
			flag=1
			echo `pwd`/$F >>$temp
		fi
	fi
done
#se ho trovato almeno un file stampo il direttori
if test $flag -eq 1
then
	echo trovati file in `pwd`
fi
#cerco nuove directory da esplorare
for F in *
do
	#Se F e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
	if test -d $F -a -x $F
	then
		FCR.sh `pwd`/$F $Cz $temp
	fi
done