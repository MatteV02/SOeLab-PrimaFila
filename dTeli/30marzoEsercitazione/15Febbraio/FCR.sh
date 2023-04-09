#!/bin/sh
cd $1
files=
for i in *
do
	#controllo se $i sia un file
	if test -f $i -a -r $i
	then
		#conto le righe del file trovato
		linecount=`wc -l <$i`
		#controllo se le linee rispettano i parametri
		if test $linecount -eq $2
		then
			#controllo se ogni linea ha un carattere numerico
			if test `grep -c [0-9] $i` -eq $linecount
			then	files="$files $i"
			fi
		fi
	else
		#se directory richiamo la ricorsiva esplorandola
		if test -d $i
		then
			FCR.sh `pwd`/$i $2
		fi
	fi
done
#controllo se sono stati trovati dei file
if test ! -z $files
then
	#stampo i dati richiesti
	echo `pwd`
	echo $files
	#richiedo al utente se si vuole richiamare la parte in C
	echo "avviare il processo C? (y/n)"
	read answer
	if test $answer=y
	then	echo richiamo file c con $files $2
	fi
fi
