#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
B=$2
Files=
flag=0
flagD=0
#esploro la directory
for V in *
do
	#controllo se $V e un file leggibile
	if test -f $V -a -r $V
	then
		L=`wc -c <$V`
		#controllo se la dimensione in caratteri del file e multiplo di B
		if test `expr $L % $B` -eq 0
		then
			Files="$Files $V"
			flag=1
		fi
	fi
done
Ger=
#cerco nuove directory da esplorare
for G in *
do
	#Se F e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
	if test -d $G -a -x $G
	then
		#trovata una sottodirectory controllo se ho trovato un file che rispetta le caratteristiche richieste
		if test $flag -eq 1
		then
			Ger="$Ger $G"
			flagD=1
		fi
		FCR.sh `pwd`/$G $B $temp
	fi
done

#controllo se entrambe le richieste sono rispettate
if test $flagD -eq 0
then
	exit 0 
fi

#stampo le sottodirectory trovate
for G in $Ger
do
	echo trovata sotto-directory `pwd`/$G
done
for F in $Files
do
	echo $F
	> $F.Chiara
	main $F $B `wc -c <$F`
done