#!/bin/sh
#AUTORE: Marco Michelini
#MODIFICHE: Prof. Letizia Leonardi
#script per la creazione guidata di una gerarchia
#sintassi: createhierarchy.sh <path>
#con <path> nome assoluto di directory traversabile

#controllo sul numero dei parametri
case $# in
1) ;;
*) echo "Errore: numero errato di parametri."
   echo "La sintassi del comando e': createhierarchy.sh <path>, con <path> nome assoluto di directory esistente e traversabile."
   exit 1;;
esac

#controllo sui parametri
case $1 in
/*) if test ! -d $1 -o ! -x $1
    then echo "Errore: directory inesistente o non traversabile."
    exit 3
    fi;;
*)  echo "Errore: sintassi errata."
    echo "La sintassi del comando e': createhierarchy.sh <path>, con <path> nome assoluto di directory esistente e traversabile."
    exit 2;;
esac

ans= #variabile usata per chiedere una conferma (y/n) all'utente
inv=true #variabile usata per determinare la validita' dell'input dell'utente (true => input non valido)
hrname= #nome relativo semplice della radice della gerarchia

#lettura del nome della radice
while test $inv = true
do echo "Inserisci il nome della radice della gerarchia:"
   read hrname
   hr=$1/$hrname #variabile contenente il nome assoluto della radice

   #controllo sul nome della radice
   case $hrname in
   */*) echo "Il nome di una directory non può contenere il carattere '/'!";; 
   *)   if test -d $hr
        then echo "Directory gia' presente in $1!\n"
        else inv=false
        fi;;
   esac

   #controllo su inv 
   if test $inv = false
   then mkdir $hr
   fi
done
inv=true #reset della variabile inv

#settaggio PATH
PATH=$PATH:`pwd`
export PATH

#creazione file temporaneo, in cui sarà salvato il nome assoluto di ogni file creato
#permette di chiedere all'utente se riempire i file creati dopo aver creato l'intera gerarchia
> /tmp/ftemp$$

createhierarchy-support.sh $hr /tmp/ftemp$$

#stampa della struttura della gerarchia creata
echo "\nLa tua nuova gerarchia:"
ls -RF $hr

#riempimento file
nf=`wc -l < /tmp/ftemp$$` #numero di file creati nella gerarchia
if test $nf -ne 0
then while test $inv = true
     do echo "\nVuoi inserire un contenuto nei file creati? (y/n)"
     read ans
	   
     #controllo risposta
     case $ans in
     y) inv=false
        for f in `cat /tmp/ftemp$$`
        do echo "\nInserisci il contenuto di $f:"
           cat > $f
           echo "File aggiornato!"
        done;;		   
     n) inv=false;;
     *) echo "Risposta non valida!";;
     esac 
     done
fi

rm /tmp/ftemp$$
