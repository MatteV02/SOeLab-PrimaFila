#!/bin/sh
#Soluzione della Prima Prova in itinere del 11 Aprile 2018: turni 1 e 2
#versione che usa un file temporaneo e usa FCR.sh per tutte le fasi

#controllo sul numero dei parametri N >= 2 e quindi N+1 >=3
case $# in
0|1|2)	echo Errore: numero parametri is $# quindi pochi parametri. Usage is $0 Y dirass1 dirass2 ...
	exit 1;;
*) 	echo OK: da qui in poi proseguiamo con $# parametri ;;
esac

#Controllo primo parametro sia un numero (con case)
case $1 in
*[!0-9]*) echo Errore: $1 non numero
         exit 2;;
*) if test $1 -eq 0
   then
	echo $1 non diverso da 0
  	exit 3
   fi;;
esac

Y=$1 #salviamo il primo parametro
#quindi ora possiamo usare il comando shift
shift

#ora in $* abbiamo solo i nomi delle gerarchie e quindi possiamo fare i controlli sui nomi assoluti e sulle directory in un for
for i 
do
	case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
	    echo $i non directory o non traversabile
	    exit 4
	    fi;;
	*)  echo $i non nome assoluto; exit 5;;
	esac
done
#controlli sui parametri finiti possiamo passare alle N fasi
PATH=`pwd`:$PATH
export PATH
> /tmp/conta$$ #creiamo/azzeriamo il file temporaneo. NOTA BENE: SOLO 1 file temporaneo!

for i
do
	echo fase per $i
 	#invochiamo il file comandi ricorsivo con la gerarchia, il numero e il file temporaneo
	FCR12.sh $i $Y /tmp/conta$$   
done

#terminate tutte le ricerche ricorsive cioe' le N fasi
#N.B. Andiamo a contare le linee del file /tmp/conta$$
echo Il numero di file totali che sono stati creati sono = `wc -l < /tmp/conta$$` 
for i in `cat /tmp/conta$$`
do
        #Stampiamo (come richiesto dal testo) i nomi assoluti dei file creati
        echo Creato il file $i
        case $i in
        *NO*) echo il file originale non conteneva almeno 5 linee e quindi il file $i e\' vuoto ;;
        *)    echo il contenuto del file $i e\' il seguente\:
              cat < $i ;;
        esac
done 

#da ultimo eliminiamo il file temporaneo
rm /tmp/conta$$
