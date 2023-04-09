#!/bin/sh
#Soluzione della Prima Prova in itinere del 11 Aprile 2014
#versione che usa diversi file temporanei e usa FCR.sh per tutte le fasi

#controllo sul numero di parametri: deve essere maggiore o uguale a 2
case $# in
0|1)	echo Errore: numero parametri $# quindi pochi parametri. Usage is $0 dirass1 dirass2 ...
	exit 1;;
*) 	echo DEBUG-OK: da qui in poi proseguiamo con $# parametri ;;
esac
#in $* abbiamo solo i nomi delle gerarchie e quindi possiamo fare i controlli sui nomi assoluti e sulle directory in un for
for i 
do
	case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
	    echo $i non directory o non attraversabile
	    exit 2
	    fi;;
	*)  echo $i non nome assoluto; exit 3;;
	esac
done

#controlli sui parametri finiti possiamo passare a settare ed esportare la variabile PATH
PATH=`pwd`:$PATH
export PATH

#ora possiamo passare alle N fasi 
n=1 #serve per dare un nome diverso ai file temporanei e volendo per dire in che fase siamo
for i
do
	> /tmp/conta$$-$n #creiamo/azzeriamo il file temporaneo
	echo fase $n per la gerarchia $i
	#invochiamo il file comandi ricorsivo con la gerarchia e il file temporaneo
	FCR.sh $i /tmp/conta$$-$n   
	n=`expr $n + 1`
done

#terminate tutte le ricerche ricorsive cioe' le N fasi
n=1 #idem come prima
for i
do
	#N.B. Andiamo a contare le linee del file /tmp/conta$$-$n
	echo Il numero di file totali della gerarchia $i che soddisfano la specifica = `wc -l < /tmp/conta$$-$n` 
	for j in `cat  /tmp/conta$$-$n`
	do
		#Stampiamo (come richiesto dal testo) i nomi assoluti dei file trovati
		echo Trovato il file $j
		#chiediamo all'utente il numero X
		echo -n "Dammi il numero X "	#l'opzione -n serve per NON fare andare a capo il cursore!
		read X
		#Controllo X (con case): questo controllo non e' richiesto, ma e' opportuno inserirlo!
		case $X in
		*[!0-9]*) echo non numerico o non positivo
			  continue;; #usiamo continue e non exit in modo da saltare al prossimo file 
		*) ;;
		esac
		echo Ora mostriamo le sue prime $X linee o tutto il contenuto se piu\' corto:
		#selezioniamo direttamente le prime $X linee del file corrente
		head -$X < $j
	done 
	n=`expr $n + 1`
done 

#cancelliamo tutti i file temporanei
n=1 #idem come prima
for i
do
	rm /tmp/conta$$-$n
	n=`expr $n + 1`
done 
