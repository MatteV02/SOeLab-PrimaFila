#!/bin/sh

cd $1
cont1=0
cont2=0
contl1=
contl2=
for F in *
do
	#controllo se file e leggibile
	if test -f $F -a -r $F
	then
		#controllo se termina con E1 o E2
		case $F in
		*.$2)	#se il file termian con E1 lo aggiungo alla lista
			contl1="$contl1 $F"
			cont1=`expr 1 + $cont1`;;
		*.$3)	#controllo se il file che termina con E2 sia anche scrivibile
			if test -w $F
			then	#se soddisfa tutti i requisiti per i file E2 allora lo inserisco nella lista
				contl2="$contl2 $F"
				cont2=`expr 1 + $cont2`
			fi;;
		esac

	else
		#controllo se directory e attravesabile
		if test -d $F -a -x $F
		then
			#esploro nuova directory
			FCR.sh `pwd`/$F $2 $3
		fi
	fi
done
#controllo se sono stati trvati almeno una istanza di un file E2 e E1
if test $cont2 -gt 0 -a $cont1 -gt 0
then
	echo directory attuale `pwd`
	#per ogni file .E2 trovato si richiama la funzione c
	for i in $contl2
	do
		echo chiamo la funzione c con $i e $contl1
	done
fi
