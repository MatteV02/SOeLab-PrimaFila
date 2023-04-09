#!/bin/sh
#FCR.sh
#file comandi ricorsivo che scrive il nome dei file creati sul file temporaneo
#il cui nome e' passato come terzo argomento
NC=     #variabile in cui salviamo il numero di caratteri del file corrente

cd $1 	#ci posizioniamo nella directory giusta

if test -f $2.txt -a -r $2.txt -a -w $2.txt 		#se esiste un file leggibile e scrivibile con il nome specificato
then
	NC=`wc -c < $2.txt`
     	echo $NC `pwd`/$2.txt >> $3 #NOTA BENE: su una stessa linea sono inseriti la sua lunghezza in caratteri e nome assoluto file trovato: per il funzionamento del cat usato nel for del file principale e' assolutamente indifferente; l'unica cosa che cambia che facendo in questo modo NON serve dividere per due il conteggio
fi

for i in *
do
        if test -d $i -a -x $i
        then
                #chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio
                FCR.sh `pwd`/$i $2 $3
        fi
done
