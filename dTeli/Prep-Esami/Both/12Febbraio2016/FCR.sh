#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
temp=$3
flag=0
Cx=$2
#esploro la directory
for F in *
do
	#controllo se $F e un file leggibile
	if test -f $F -a -r $F
	then
		#controllo se esiste Cx nel file
		if grep $Cx < $F > /dev/null 2> /dev/null
		then
			echo file trovato $F
			echo $F >>$temp
		fi
	fi	
done
#controllo se e' trovato un file nel dir
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
		FCR.sh `pwd`/$F $Cx $temp
	fi
done