#!/bin/sh
#AUTORE: Marco Michelini
#MODIFICHE: Prof. Letizia Leonardi
#script chiamato da createhierarchy.sh - non chiamare direttamente!
#sintassi: createhierarchy-support.sh <path> <file-temporaneo>

cd $1
echo "\nPercorso corrente:" $1

ans= #variabile usata per chiedere una conferma (y/n) all'utente
inv=true #variabile usata per determinare la validità dell'input dell'utente (true => input non valido)
flist= #variabile usata per memorizzare la lista dei file da creare in $1
dlist= #variabile usata per memorizzare la lista delle directory da creare in $1

#creazione file
while test $inv = true
do echo "\nInserire la lista dei file:"
   read flist

   #controllo sulla lista dei file
   if test "$flist"
   then case $flist in
        */*) echo "Il nome di un file non può contenere il carattere '/'!";; 
        *)   for f in $flist 
             do > $f
	     done
	     inv=false;;
        esac
   else inv=false
   fi
done
inv=true #reset della variabile inv

#salvataggio del nome assoluto dei file effettivamente creati
for f in *
do if test -f $f
   then echo `pwd`/$f >> $2
   fi
done

#creazione directory
while test $inv = true
do echo "\nInserire la lista delle sottodirectory:"
   read dlist

   #controllo sulla lista delle directory
   if test "$dlist"
   then case $dlist in
        */*) echo "Il nome di una directory non può contenere il carattere '/'!";; 
        *)   for d in $dlist 
             do if test -d $d -o -f $d
                then echo $d "non creata: nel percorso corrente esiste un file o una directory con lo stesso nome!"
                else mkdir $d
		fi
	     done
	     inv=false;;
        esac
   else inv=false
   fi
done

#passaggio alle sottodirectory eventualmente create
for d in *
do if test -d $d
   then inv=true
        while test $inv = true
        do echo "\nVuoi spostarti nella sottodirectory" $d"? (y/n)"
	   read ans
	   
	   #controllo risposta
	   case $ans in
	   y) inv=false	
	      createhierarchy-support.sh `pwd`/$d $2;;	   
	   n) inv=false;;
	   *) echo "Risposta non valida!";;
	   esac 
        done
   fi
done
