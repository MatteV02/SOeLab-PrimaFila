#!/bin/sh
# Soluzione della prova d'esame del 3 Giugno 2020
#
# File ricorsivo. Uso: $0 dirass C filetemp
#file comandi ricorsivo che scrive il nome dei file trovati sul file
#il cui nome e' passato come terzo argomento
cd $1

#poiche' cerchiamo delle directory NON si puo' escludere che anche la radice della gerarchia soddisfi le specifiche indicate nel testo!
#quindi controlliamo $1 che la prima volta sara' il nome della gerarchia corrente
case $1 in
	*/$2?$2)  #se il nome rispetta la specifica, lo scriviamo sul file temporaneo: in questo caso la specifica richiedeva che il nome del file fosse di 3 caratteri e che i caratteri di posizione dispari (cioe' il primo e il terzo) fossero uguali al carattere C passato
	  pwd >> $3 ;; #NON SERVE SCRIVERE echo `pwd` >> $3 dato che il comando pwd gia' da solo riporta su standard output la dir corrente!
*) ;; 	  #altrimenti non facciamo nulla
esac

for i in *
do
        if test -d $i -a -x $i 
        then
                #invocazione ricorsiva passando sempre il nome assoluto della dir trovata
                $0 `pwd`/$i $2 $3
        fi
done
