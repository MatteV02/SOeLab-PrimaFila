#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
shift
#Inizializzo la variabile Files dove inserisco i file da ricercare
Files=
Tot=0
for F  
do
	Files="$Files $F"
	Tot=`expr $Tot + 1`
done
#esploro la directory
contatore=
FilesT=
for F in *
do
	#controllo se $F e un file 
	if test -f $F
	then
		#controllo se il file e uguale ad uno dei file da ricercare
		for N in $Files
		do
			#contorllo se il nome del file e uguale
			case $F in 
			$N)	
				contatore=`expr $contatore + 1`
				FilesT="$FilesT $F";;
			esac
		done
	fi
done
#controllo se ho trovato almeno meta dei file richiesti
if test $contatore -gt `expr $Tot / 2`
then
	echo file trovati nel dir `pwd`
	echo main $FilesT
fi
#cerco nuove directory da esplorare
for F in *
do
	#Se F e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
	if test -d $F -a -x $F
	then
		FCR.sh `pwd`/$F $Files
	fi
done