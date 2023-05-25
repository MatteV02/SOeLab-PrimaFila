#!/bin/sh
#combio la directory alla directory che voglio esaminare in questa ricorsione
cd $1
#creo una variabilile flag che mi segnala se un file non rispetta le condizioni o si trova una directory
flag=0
for i in *
do
	#controllo se un file ed leggibile
	if test -f $i -a -r $i
	then
		#controllo che rispetti le condzioni date
		if test ! `wc -l <$i` -gt $2
		then
			#aggiorno il flag marchiando la dir per essere scartata
			flag=1
		fi
	fi
	#controllo se directory
	if test -d $i
	then	#aggiorno il flag marchiando la directory per essere scartata
		flag=1
		#controllo se il la dir e comuque traversabile
		if test -x $i
		then	#richiamo la ricorsiva per esplorare la nuova dir
			FCR.sh `pwd`/$i $2 $3
		fi
	fi
done
#scarto la dir se il flag e stato attivato cioe ad 1
if test $flag -eq 1
then
	exit
fi
#se il flag e a 0 aggiungo la dir al file temporaneo
echo `pwd` >>$3

