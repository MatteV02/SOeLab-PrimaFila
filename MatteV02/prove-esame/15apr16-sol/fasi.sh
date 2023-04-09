#!/bin/sh
#Soluzione della Prima Prova in itinere del 15 Aprile 2016
#versione che usa un file temporaneo e usa FCR.sh per tutte le fasi

#controllo sul numero di parametri: deve essere maggiore o uguale a 3 
case $# in
0|1|2)	echo Errore: numero parametri is $# quindi pochi parametri. Usage is $0 numero dirass1 dirass2 ...
	exit 1;;
*) 	echo DEBUG-OK: da qui in poi proseguiamo con $# parametri ;;
esac
#dobbiamo isolare l'ultimo parametro e intanto facciamo i controlli
num=1 	#la variabile num ci serve per capire quando abbiamo trovato l'ultimo parametro
params=	#la variabile params ci serve per accumulare i parametri a parte l'ultimo
#in $* abbiamo i nomi delle gerarchie e il numero intero 
for i 
do
	if test $num -ne $# #ci serve per non considerare l'ultimo parametro che e' il numero
	then
		#soliti controlli su nome assoluto e directory traversabile
		case $i in
		/*) 	if test ! -d $i -o ! -x $i
	    		then
	    		echo $i non directory o non attraversabile
	    		exit 2
	    		fi;;
		*)  	echo $i non nome assoluto; exit 3;;
		esac
		params="$params $i" #se i controlli sono andati bene memorizziamo il nome nella lista params
	else
	#abbiamo individuato l'ultimo parametro e quindi facciamo il solito controllo su numerico e strettamente positivo
		#Controllo ultimo  parametro (con expr)
		expr $i + 0  > /dev/null 2>&1
		N1=$?
		if test $N1 -ne 2 -a $N1 -ne 3
		then #echo numerico $i siamo sicuri che numerico
     			if test $i -le 0
     			then echo $i non strettamente positivo
          		exit 4
     			fi
		else
  			echo $i non numerico
  			exit 5
		fi
		X=$i #se i controlli sono andati bene salviamo l'ultimo parametro
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
	#invochiamo il file comandi ricorsivo con la gerarchia, il numero e il file temporaneo
	FCR.sh $i $X /tmp/conta$$   
done

#terminate tutte le ricerche ricorsive cioe' le N fasi
#N.B. Andiamo a contare le linee del file /tmp/conta$$
echo Il numero di directory totali che soddisfano la specifica = `wc -l < /tmp/conta$$` 
for i in `cat /tmp/conta$$`	#nel file temporaneo abbiamo i nomi assoluti delle directory trovate
do
	#Stampiamo (come richiesto dal testo) i nomi assoluti delle directory trovate
	echo "Trovato la directory $i; contiene i seguenti file:"
	cd $i	#ci spostiamo nella directory corrente del for
	for file in * #siamo sicuri che sono solo file (controllo fatto da FCR.sh)
	do
		echo "file: `pwd`/$file"
		echo "la cui linea $X-esima a partire dalla fine e' la seguente:"
		#selezioniamo direttamente la $X-esima linea del file corrente a partire dalla fine
		tail -$X $file | head -1
	done
done 
#da ultimo eliminiamo il file temporaneo
rm /tmp/conta$$

