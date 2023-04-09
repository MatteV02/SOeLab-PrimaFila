#!/bin/sh

#controllo sul numero di parametri: deve essere passato un solo parametro
#facciamo anche contestualmente il controllo se e' una directory traversabile
case $# in
1)	case $1 in
	/*) if test ! -d $1 -o ! -x $1
	    then
	    echo $1 non directory o non attraversabile
	    exit 1
	    fi;;
	*)  echo $1 non nome assoluto; exit 2;;
	esac;;
*) 	echo Errore: Usage is $0 dirass 
	exit 3;;
esac
PATH=`pwd`:$PATH
export PATH
conta=0  #valore che andremo a scrivere nel file temporaneo
fase=A

echo $conta > /tmp/tmpContaLivelli #scriviamo il valore di conta (cioe' 0) nel file temporaneo

#invochiamo per la fase A: bastano solo 3 parametri 
echo Stiamo iniziando la fase A
FCR.sh $1 $conta $fase    

read tot < /tmp/tmpContaLivelli
echo Il numero di livelli totali della gerarchia = $tot
#passiamo alla seconda fase
echo Stiamo iniziando la fase B
livello= #variabile per leggere il valore inserito dall'utente
#adesso chiediamo all'utente quale livello vuole visualizzare
echo -n "dammi il numero di livello pari che vuoi visualizzare (il numero deve essere compreso fra 1 e $tot " > /dev/tty
read livello #chiediamo all'utente il numero di livello
case $livello in
*[!0-9]*) echo NON hai fornito un numero o non positivo
	  #rimuoviamo il file temporaneo
          rm /tmp/tmpContaLivelli
          exit 4;;
*) #numero giusto;;
esac
if test $livello -ge 1 -a $livello -le $tot
then
	echo $livello compreso OK
	if test `expr $livello % 2` -eq 0
	then
		echo $livello pari
	else
		#questo sarebbe un errore per il testo considerato, ma lo teniamo buono per fare un numero maggiore di prove
		echo $livello dispari 
	fi
else
	echo $livello NON compreso
	#rimuoviamo il file temporaneo
	rm /tmp/tmpContaLivelli
	exit 5
fi
fase=B
#passiamo alla seconda chiamata 
echo $conta > /tmp/tmpContaLivelli #riscriviamo il valore di conta (cioe' 0) nel file temporaneo

FCR.sh $1 $conta $fase $livello #NOTA BENE: passiamo un parametro in piu'! 
#rimuoviamo il file temporaneo
rm /tmp/tmpContaLivelli
echo FINITO
