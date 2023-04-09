#!/bin/sh

cd $1
files=
counter=0

for i in *
do
	#controllo se $i file
	if test -f $i -a -r $i
	then
		#controllo se la lungezza del file rispetta le richieste
		#e controllo se il nome del file e lungo esattamente 2 caratteri
		if test ` wc -l <$i ` -eq $2
		then
			charOfFile=`echo $i | wc -m`
			if test `expr $charOfFile - 1` -eq 2
			then
				files="$files $i"
				counter=`expr $counter + 1`
			fi
		fi
	else
		if test -d $i -a -x $i
		then
			FCR.sh `pwd`/$i $2 $3
		fi
	fi
done
#controllo il numero di file trovati sono conforme
if test $counter -ge 2 -a $counter -lt $3
then
	echo `pwd`
	echo chiamata alla funzione C con params $files $2

fi
