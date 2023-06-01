#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
K=$2
temp=$3
flag=0
#esploro la directory
for F in *
do
	#controllo se $F e un file leggibile
	if test -f $F -a -r $F
	then
		#controllo se le line contenute in $F sono esattamente $K
		if test `wc -l < $F` -eq $K
		then
			echo `pwd`/$F >>$temp
			flag=1
		fi
	fi
done
#Commento
if test $flag -eq 1
then
	echo trovati in `pwd`
fi
#cerco nuove directory da esplorare
for F in *
do
	#Se F e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
	if test -d $F -a -x $F
	then
		FCR.sh `pwd`/$F $K $temp
	fi
done