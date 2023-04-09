#!/bin/sh

if test $# -eq 0
then 	echo DEBUG-il numero di parametri passati è sbagliato
	exit 1
fi


echo $*


for i
do
	echo $i
done
