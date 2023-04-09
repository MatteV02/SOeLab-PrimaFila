#!/bin/sh
#FCR.sh 
#file comandi ricorsivo che scrive il nome dei file creati sul file temporaneo 
#il cui nome e' passato come terzo argomento
NL= 	#variabile in cui salviamo il numero di linee del file corrente

cd $1 	#ci posizioniamo nella directory giusta

for i in *
do
	if test -f $i -a -r $ii #per ogni file leggibile
	then
		NL=`wc -l < $i`
		if test $NL -ge $2 #controlliamo la lunghezza in linee del file rispetto a Y
		then
			if test $NL -ge 5  #controlliamo la lunghezza in linee del file ora rispetto a 5!
			then
				head -5 $i | tail -1 > $i.quinta	#creiamo un file con il nome specificato
				echo `pwd`/$i.quinta >> $3 		#salviamo il suo nome nel file temporaneo
			else
				> $i.NOquinta	#il file non ha abbastanza linee!	#creiamo un file con il nome specificato
				echo `pwd`/$i.NOquinta >> $3				#salviamo il suo nome nel file temporaneo
			fi
		fi
	fi
done

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory 
		FCR12.sh `pwd`/$i $2 $3 
	fi
done
