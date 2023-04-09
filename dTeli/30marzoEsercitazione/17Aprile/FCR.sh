#!/bin/sh

cd $1
#esamino la directory corrente
for i in *
do

	#controllo se un file e leggibile
	if test -f $i -a -r $i
	then
		#controllo che il numero di rige che terminano con t siano conformi alla richiesta
		if test `grep -c t$ $i` -ge $2
		then
			echo `pwd`/$i >>$3
		fi
	else
		#controllo se directory e attraversabile
		if test -d $i -a -x $i
		then
			FCR.sh `pwd`/$i $2 $3
		fi
	fi

done

