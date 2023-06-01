#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 
X=$2
Files=
flag=0
#esploro la directory
for F in *
do
	#controllo se $F e un file leggibile
	if test -f $F -a -r $F
	then
		echo sono qui `wc -l < $F`
		#controllo se le line contenute in $F sono strettamente maggiori di $X
		if test `wc -l < $F` -gt $X
		then
			echo $X $F
			head -$X $F >$F.testa
			Files="$Files $F.testa"
			flag=1
		fi
	
	fi
done

if test $flag -eq 1
then
	echo trovati file nella cartella `pwd`
	main $Files
fi

#cerco nuove directory da esplorare
for F in *
do
	#Se F e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
	if test -d $F -a -x $F
	then
		FCR.sh `pwd`/$F $X
	fi
done