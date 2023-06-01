#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
temp=$2
#esploro la directory
for F in *
do
	#controllo se $F e un file leggibile
	if test -f $F -a -r $F
	then
		#controllo se le line contenute in $F sono diverse da 0
		if test `wc -l < $F` -ne 0
		then
			#controllo se tutti i caratteri sono albetici minuscoli
			
			if test `grep -c -v [a-z] $F` -eq 0 
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
		FCR.sh `pwd`/$F $temp
	fi
done