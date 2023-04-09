#!/bin/sh

if test $# -eq 0
then 	echo "Il numero di parametri passati non è corretto"
	exit 1
fi

# Viene passato almeno un parametro

altriParametri=	# Variabile per memorizzare tutti i parametri prima dell'ultimo
count=1	# Variabile per contare i parametri

for i
do	if test $count -eq $#
	then	# Ultimo parametro
		echo "L'ultimo primo parametro vale:" $i
	else	# parametri prima dell'ultimo
		altriParametri="$altriParametri $i"
	fi
	count=`expr $count + 1`
done
echo "Gli altri parametri valgono:" $altriParametri
