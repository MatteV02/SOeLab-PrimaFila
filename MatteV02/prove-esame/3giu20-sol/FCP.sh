#!/bin/sh
# Soluzione della prova d'esame del 3 Giugno 2020
#
# File comandi. Uso: $0 C dirass1 dirass2 ...
#

# controllo sul numero dei parametri
case $# in
0|1|2)    echo "Errore nel numero dei parametri $#"
          exit 1;;
*)        echo DEBUG-"Numero parametri corretti $#";;
esac

#controllo su primo parametro
case $1 in
?) ;;
*)      echo $1 non singolo carattere
        exit 2;;
esac

#salviamo il primo parametro: il nome della variabile da usare era specificato nel testo
C=$1

#ora possiamo usare il comando shift 
shift

#ora in $* abbiamo solo i nomi delle gerarchie e quindi possiamo fare i controlli sui nomi assoluti e sulle directory in un for
for i
do
        case $i in
        /*) if test ! -d $i -o ! -x $i
            then
            echo $i non directory
            exit 3
            fi;;
        *)  echo $i non nome assoluto; exit 4;;
        esac
done

#controlli sui parametri finiti possiamo passare alle N fasi

PATH=`pwd`:$PATH
export PATH

#azzeriamo il file temporaneo: usiamo un solo file temporaneo: il nome del file temporaneo era specificato nel testo
> /tmp/nomiAssoluti

for i
do
        echo fase per $i
        #invochiamo il file comandi ricorsivo con la gerarchia corrente, il carattere e il file temporaneo
        FCR.sh $i $C /tmp/nomiAssoluti	#N.B. poiche' il nome del file temporaneo era fisso si poteva anche non passarlo, ma si e' deciso di passarlo per uniformita' per i casi in cui bisogna per forza passarlo 
done

echo Il numero di direttori trovati nella gerarchia = `wc -l < /tmp/nomiAssoluti`
#N.B. Andiamo a contare le linee del file temporaneo 
#In questo caso possiamo recuperare anche i nomi (assoluti) delle directory trovate 
for i in `cat /tmp/nomiAssoluti`
do
        echo Il nome assoluto di una delle directory trovate: $i
        echo -n "Elena vuoi visualizzarne il contenuto, compresi gli elementi nascosti (Si/si/Yes/yes)? " #il nome dell'UTENTE da usare era specificato nel testo
        read answer	#il nome della variabile da usare era specificato nel testo
        case $answer in
                s* | S* | Y* | y*) ls -a $i;;
                *) ;; #non facciamo nulla!
        esac
done

#cancelliamo il file temporaneo
rm /tmp/nomiAssoluti

