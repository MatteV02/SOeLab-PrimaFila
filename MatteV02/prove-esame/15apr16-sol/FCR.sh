#!/bin/sh
#FCR.sh 
#file comandi ricorsivo che scrive il nome delle directory trovate sul file temporaneo 
#il cui nome e' passato come terzo argomento

cd $1
#la variabile NR ci serve per il numero di linee dei file; in questo caso la inizializziamo per fare un ulteriore controllo come spiegato in seguito
NR=0
#la variabile DIR ci serve per capire se ci sono solo file
DIR=false
#la variabile trovato ci serve per capire se tutti i file rispettano la specifica
trovato=true

for i in *
do
	#controlliamo solo i nomi dei file (se inseriamo anche il controllo se leggibili per cautelarci dato che dopo usiamo il comando wc bisogna commentarlo!)
	if test -f $i 
	then 	
		#calcoliamo il numero di linee 
		NR=`wc -l < $i`
		#controlliamo se il numero delle linee NON e' strettamente maggiore di X
		if test $NR -le $2
			then
			#abbiamo trovato un file che NON soddisfa le specifiche e quindi mettiamo a false trovato
			trovato=false
		fi
	else
		if test -d $i
		then
		#abbiamo trovato una directory e quindi dobbiamo mettere a true DIR
		DIR=true
		fi
	fi
done
#se i due booleani sono rimasti ai valori iniziali allora abbiamo trovato una directory giusta. Nota bene: l'ultimo controllo garantisce che si sia trovato almeno un file dato che una directory vuota non rispetta le specifiche! 
if test $DIR = false -a $trovato = true -a $NR -ne 0 
then
	pwd >> $3 #salviamo il nome della directory corrente (che soddisfa le specifiche) nel file temporaneo
	#NOTA BENE: in questo caso basta usare il comando pwd e quindi e' ritenuto sbagliato scrivere echo `pwd`
fi

#ricorsione in tutta la gerarchia
for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory 
		FCR.sh `pwd`/$i $2 $3 
	fi
done
