#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
L=$2
temp=$3
shift
shift
shift
#Inizializzo la variabile Q dove inserisco i caratteri da controllare
Q=
for q 
do
	Q="$Q $q"
done

#esploro la directory
for F in *
do
	#controllo se $F e un file leggibile
	if test -f $F -a -r $F
	then
		#controllo se le line contenute in $F sono esattamente $L
		if test `wc -l < $F` -eq $L
		then
			flag=0
			#controllo se sono presenti ogni carattere cercato
			for c in $C
			do
				#controllo se il carattere $c esiste nel file
				if test ! `grep $c $F >/dev/null 2>&1`
				then
					flag=1
				fi
			done
			#controllo se ho trovato tutti i caratteri nel file
			if test $flag -eq 0
			then
				read tot <$temp
				tot=`expr $tot + 1`
				echo $tot >$temp
				echo file trovato `pwd`/$F
				main $F $L $Q
			fi
		fi
	fi
done

#cerco nuove directory da esplorare
for F in *
do
	#Se F e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
	if test -d $F -a -x $F
	then
		FCR.sh `pwd`/$F $L $temp $Q
	fi
done