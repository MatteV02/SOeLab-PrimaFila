#!/bin/sh
#Soluzione della Prima Prova in itinere del 5 Aprile 2019 - testo turni 5 e 6
#versione che usa un file temporaneo e usa FCR.sh per tutte le fasi

#controllo sul numero dei parametri N >= 2 e quindi N+1 >=3
case $# in
0|1|2)  echo Errore: numero parametri is $# quindi pochi parametri. Usage is $0 nomedir dirass1 dirass2 ...
        exit 1;;
*)      echo DEBUG-OK: da qui in poi proseguiamo con $# parametri ;;
esac

#dobbiamo isolare l'ultimo parametro e intanto facciamo i controlli
num=1 	#la variabile num ci serve per capire quando abbiamo trovato l'ultimo parametro
params=	#la variabile params ci serve per accumulare i parametri a parte l'ultimo
#in $* abbiamo i nomi delle gerarchie e la stringa
for i
do
	if test $num -ne $# #ci serve per non considerare l'ultimo parametro che e' la stringa
	then
		#soliti controlli su nome assoluto e directory traversabile
		case $i in
		/*) 	if test ! -d $i -o ! -x $i
	    		then
	    		echo $i non directory o non traversabile
	    		exit 2
	    		fi;;
		*)  echo $i non nome assoluto; exit 3;;
		esac
		params="$params $i"
	else
		#abbiamo individuato l'ultimo parametro e quindi ha senso controllare sia una stringa che non contiene il carattere '/' visto l'uso che poi se ne deve fare (anche se non era richiesto)
		S=$i #salviamo l'ultimo parametro
		case $S in
		*/*) 	echo Errore: $S contiene il carattere '\'
    			exit 4;;
		*) ;;
		esac
	fi
	num=`expr $num + 1` #incrementiamo il contatore del ciclo sui parametri
done

#controlli sui parametri finiti possiamo passare alle N fasi
PATH=`pwd`:$PATH
export PATH
> /tmp/conta$$ #creiamo/azzeriamo il file temporaneo. NOTA BENE: SOLO 1 file temporaneo!

for i in $params
do
        echo fase per $i
        #invochiamo il file comandi ricorsivo con la gerarchia, la stringa e il file temporaneo
        FCR.sh $i $S /tmp/conta$$
done

#terminate tutte le ricerche ricorsive cioe' le N fasi
#N.B. Andiamo a contare le linee del file /tmp/conta$$
echo Il numero di file totali che sono stati trovati sono = `wc -l < /tmp/conta$$`
c=0	#variabile che serve nel for successivo per capire se l'elemento corrente del for e' una lumghezza o il nome assoluto di un file 
for i in `cat /tmp/conta$$`
do
        #Stampiamo (come richiesto dal testo) la lunghezza in caratteri e i nomi assoluti dei file trovati 
 	c=`expr $c + 1`
	if test `expr $c % 2` -eq 1
	then 
		echo elemento dispari, quindi lunghezza in caratteri del file = $i
       	else 
		echo elemento pari, quindi nome assoluto del file $i
		echo -n "dimmi se vuoi ordinare il file (Si, si, Yes, yes) "
		read RISPOSTA
		#controlliamo che cosa vuole fare l'utente, nel modo piu' generale possibile
		case $RISPOSTA in
		S* | s* | Y* | y*) 
			#poiche' alcuni testi richiedevano l'ordinamento alfabetico senza distinguere maiuscole e minuscole e altri l'ordinamento inverso, si sono riportate entrambe le versioni!
			echo il file ordinato senza distinguere maiuscole e minuscole
			sort -f $i
			echo il file ordinato in ordine alfabetico inverso 
			sort -r $i ;;
		*) echo deciso di non ordinare il file $i ;;
      		esac		
	fi
done
#da ultimo eliminiamo il file temporaneo
rm /tmp/conta$$
