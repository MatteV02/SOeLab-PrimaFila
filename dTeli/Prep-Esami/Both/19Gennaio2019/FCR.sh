#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
X=$3
C=$2
temp=$4
flag=0

#esploro la directory
for F in *
do
	#controllo se $F e un file leggibile
	if test -f $F -a -r $F
	then
		#controllo se i caratteri contenutenuti in $F sono esattamente $X
		NumeroCaratteri=`wc -c < $F`
		if test $NumeroCaratteri -eq $X
		then
			#controllo se sia multipo di $C
			if test `expr $NumeroCaratteri % $C` -eq 0
			then
				flag=1
				echo `pwd`/$F >>$temp 
			fi
		fi
	fi
done

#stampo se ho trovato dei file la directory corrente
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
		FCR.sh `pwd`/$F $C $X $temp
	fi
done