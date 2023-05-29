#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
L=$2
temp=$3
Out=
#esploro la directory
for i in *
do
	#controllo se $i e un file leggibile
	if test -f $i -a -r $i
	then
		#controllo se le line contenute in L sono maggiori o uguali a L
		if test `wc -l < $i` -ge $L -a `wc -l < $i` -lt 255 
		then
			echo "trovato `pwd`/$i"
			echo `pwd`/$i `wc -l <$i` >>$temp
		fi	
	fi
done

#cerco nuove directory da esplorare

for i in *
do
	#Se i e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
	if test -d $i -a -x $i
	then
		FCR.sh "`pwd`/$i" $L $temp
	fi
done