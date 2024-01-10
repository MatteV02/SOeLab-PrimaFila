#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
H=$2
K=$3
Files=
c=0
#esploro la directory
for F in *
do
	#controllo se $F e un file leggibile
	if test -f $F -a -r $F
	then
		Files="$Files $F"
		c=`expr $c + 1`
	fi
done
#controllo se il numero di file e maggiore e minore di H e K
if test $c -gt $H -a $c -lt $K
then
	echo dir `pwd`
	echo main $Files
fi
#cerco nuove directory da esplorare
for F in *
do
	#Se F e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
	if test -d $F -a -x $F
	then
		FCR.sh `pwd`/$F $H $K
	fi
done