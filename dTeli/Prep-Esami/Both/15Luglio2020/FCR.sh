#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
L=$2
temp=$3

#esploro la directory
for F in *
do
	#controllo se $F e un file leggibile
	if test -f $F -a -r $F
	then
		#controllo se le line contenute in $F sono esattamente $L
		if test `wc -l < $F` -eq $L
		then
			#controllo che la media della lunghezza e minore di 10
			N=`wc -c <$F`
			if test `expr $N / $L` -gt 10
			then
				echo `pwd`/$F >>$temp
			fi
		fi
	fi
done

#cerco nuove directory da esplorare
for F in *
do
	#Se F e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
	if test -d $F -a -x $F
	then
		FCR.sh `pwd`/$F $L $temp
	fi
done