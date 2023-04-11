#!/bin/sh
#FCR.sh 
#file comandi ricorsivo che scrive il nome dei file creati sul file temporaneo 
#il cui nome e' passato come terzo argomento

cd $1 #ci posizioniamo nella directory giusta

if test -f $2 -a -r $2	#verifichiamo se nella dir corrente esiste un file leggibile con il nome richiesto
then
	sort -f $2 > sorted  #usiamo l'opzione -f per fare l'ordinamento senza tenere in conto maiuscole/minuscole (per fare l'ordinamento senza linee doppie si doveva usare l'opzione -u)
	#NOTA BENE: la ridirezione in uscita e' necessaria per salvare il risultato del comando sort!
	echo `pwd`/sorted >> $3  #salviamo il nome assoluto del file creato nel file temporaneo
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

