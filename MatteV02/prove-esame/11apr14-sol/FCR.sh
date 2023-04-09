#!/bin/sh
#FCR.sh 
#file comandi ricorsivo che scrive il nome dei file trovati sul file temporaneo
#il cui nome e' passato come secondo argomento

cd $1
#la variabile NR ci serve per il numero di linee
NR=
#la variabile NG ci serve per il numero di linee trovate dal grep
NG=

for i in *
do
	#controlliamo solo i nomi dei file leggibili come indicato nella specifica!
	if test -f $i -a -r $i
	then 	
		NR=`wc -l < $i`
		if test $NR -ne 0	#e' opportuno inserire il controllo che il file non sia vuoto!
		then
			NG=`grep '^a' $i | wc -l` #con il grep cerchiamo le linee che iniziano con il carattere richiesto (cioe' 'a'); contiamo quindi quante linee soddisfano questa specifica
			if test $NR -eq $NG #se il numero di linee del file e' uguale al numero di linee ricavate dal grep vuole dire che tutte le linee cominciano con il carattere richiesto!
			then
				#abbiamo trovato un file che soddisfa le specifiche e quindi inseriamo il suo nome assoluto nel file temporaneo
				echo `pwd`/$i >> $2
			fi
		fi
	fi
done

#ricorsione in tutta la gerarchia
for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva
		FCR.sh `pwd`/$i $2
	fi
done
