#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
Y=$3
D=$2
temp=$4
conta=0
flag=0
#esploro la directory
for F in *
do
	#controllo se sono in una cartella con il nome D
	if test $5 -eq 1
	then
		#controllo se $F e un file leggibile
		if test -f $F -a -r $F
		then
			#controllo se le line contenute in $F sono esattamente $Y
			if test `wc -l < $F` -eq $Y
			then
				flag=1
				echo `pwd`/$F >>$temp
				conta=`expr $conta + 1`
				
			fi

		fi	
	
	fi
done
#controllo se nella directory sono stati trovati file che corrispondo alle richieste allora stampo la directory
if test $flag -eq 1
then
	echo trovati in `pwd`
fi

#cerco nuove directory da esplorare
for F in *
do
	#Se F e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
	if test -d $F -a -x $F
	then
		#controllo se il nome della directory e D
		if test "$F" = "$D"
		then
			FCR.sh `pwd`/$F $D $Y $temp 1 
		else
			FCR.sh `pwd`/$F $D $Y $temp 0
		fi
		conta=`expr $conta + $?`
		
	fi
done
echo $conta $1
exit $conta