#!/bin/sh
#Soluzione della Prova del 9 Febbraio 2015
#Si deve usare un file temporaneo 

#controllo sul numero dei parametri: devono essere esattamente 2
case $# in
0|1)	echo Errore: numero parametri is $# quindi pochi parametri. Usage is $0 dirass numerostrettpos 
	exit 1;;
2) 	echo OK: da qui in poi proseguiamo con $# parametri ;;
*)	echo Errore: numero parametri is $# quindi troppi parametri. Usage is $0 dirass numerostrettpos
	exit 1;;
esac

#controlliamo la gerarchia $1
case $1 in
	/*) if test ! -d $1 -o ! -x $1
	    then
	    echo $1 non directory o non traversabile
	    exit 2
	    fi;;
	*)  echo $1 non nome assoluto; exit 3;;
esac

#Controllo $2 (fatto con case) 
case $2 in
	*[!0-9]*) echo $2 non numerico o non positivo
			exit 4;;
	*) if test $2 -eq 0
	   then echo ERRORE: secondo parametro $2 uguale a zero
		exit 5
	   fi ;;
esac

#controlli sui parametri finiti possiamo passare a settare il path e poi lo esportiamo
PATH=`pwd`:$PATH
export PATH
> /tmp/tmp$$ #creiamo il file temporaneo che passeremo poi come ultimo parametro al file comandi ricorsivo
FCR.sh $1 $2 /tmp/tmp$$   

parms= #variabile in cui accumuliamo file trovati e numeri chiesti all'utente

for i in `cat /tmp/tmp$$`
do
	params="$params $i"
 	#Il programma, per ognuno dei file, deve richiedere all'utente un numero X intero strettamente positivo e minore di $2
	echo -n "Dammi un numero intero strettamente positivo e minore o uguale a $2 per il file $i: "
	read X
	#Controllo X (sempre con case!)
	case $X in
	*[!0-9]*) echo non numerico o non positivo
		  rm /tmp/tmp$$ #poiche' stiamo uscendo a causa di un errore, cancelliamo il file temporaneo!
                  exit 5;;
	*) if test $X -eq 0
	   then echo ERRORE: dato inserito $X uguale a zero
		rm /tmp/tmp$$ #poiche' stiamo uscendo a causa di un errore, cancelliamo il file temporaneo!
		exit 5
	   fi ;;
	esac
	if test $X -gt $2 
	then
		echo Numero fornito $X non minore di $2
		rm /tmp/tmp$$ #poiche' stiamo uscendo a causa di un errore, cancelliamo il file temporaneo!
		exit 6
	fi
 	params="$params $X"
done
#cancelliamo il file temporaneo dato che non ne abbiamo piu' bisogno
rm /tmp/tmp$$
#Infine, si deve invocare la parte in C passando come parametri gli N nomi assoluti dei file selezionati F0, F1, ..., FN-1 intervallati dai numeri positivi chiesti all'utente  
echo Sto per chiamare la parte C a cui si passano i parametri $params
9Feb15 $params
