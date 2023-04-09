#!/bin/sh
#file comandi ricorsivo che torna il numero totale di livelli 
#contati fino a quel punto

cd $1
#il primo livello verra' contato come livello 1
conta=`expr $2 + 1`  #nota bene questa variabile conta NON e' quella dello script principale, ma e' una variabile conta LOCALE a questo script
for i in *
do
	if test -d $i -a -x $i
	then 	
		FCR.sh `pwd`/$i $conta $3 $4
		ret=$?
                read prec < /tmp/tmpContaLivelli #leggiamo dal file temporaneo il valore del numero di livello
		if test $ret -gt $prec		 #se il valore calcolato in ricorsione e' maggiore, allora
		then
		echo $ret > /tmp/tmpContaLivelli #aggiorniamo il numero di livello raggiunto
		fi
	fi
done
if test $3 = B	#solo se siamo nella fase B bisogna fare questa parte di codice, mentre quella precedente e' comune ad entrambe le fasi!
then
	if test $4 -eq $conta #se nella seconda fase sono in una directory che corrisponde al livello cercato, dobbiamo stampare le info richieste
	then
		echo  Adesso visualizzo il contenuto della directory `pwd` che fa parte del livello $4
		ls -l #se dovessimo visualizzare tutte le info inclusi i file nascosti sarebbe ls -la oppure inclusi i file nascosti a parte . e .. sarebbe ls -lA
	fi 
fi 
exit $conta 	#N.B. codice comune a tutte le fasi!
