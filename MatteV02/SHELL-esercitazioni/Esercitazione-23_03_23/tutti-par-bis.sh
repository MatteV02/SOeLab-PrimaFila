#!/bin/sh

if test $# -eq 0
then 	echo DEBUG-il numero di parametri passati è sbagliato
	exit 1
fi


echo $*

counter=1 # Contatore per il ciclo successivo

for i
do
	echo Il parametro numero $counter passato vale\: $i
	counter=`expr $counter + 1`
done
