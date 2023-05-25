#!/bin/sh
#file ricorsivo

#cambio di Dir nella dir da esplorare
cd $1

#ciclo di esplorazione dei contenuti della dir
for File in *
do
	#controllo se File e un file leggibile
	if test -f $File -a -r $File
	then	#controllo che termini o per .$Str2 o per .$Str1
		case $File in
		*.$2) 	#inserisco il file trovato nel file temporaneo apposito
			echo $File >>$4;;
		*.$3) 	#inserisco il file trovato nel file temporaneo appposito
			echo $File >>$5;;
		esac
	else
		#se non e un file controllo sia una dir attraversabile
		if test  -d $File -a -x $File
		then
			#esploro la dir trovata
			FCR.sh `pwd`/$File $2 $3 $4 $5
		fi
	fi
done
