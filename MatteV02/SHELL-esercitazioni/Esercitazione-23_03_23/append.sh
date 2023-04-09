#!/bin/sh

case $# in
1)	echo DEBUG-un parametro passato
	echo Scrivi in append sul file specificato:
	if test -f $1
	then	echo DEBUG-il file passato esiste
		echo Scrivi in append sul file specificato:
	else	echo DEBUG-il nome specificato non corrisponde a un file
		exit 2;
	fi
	cat >> $1;;
2)	echo DEBUG-due parametri passati
	if test ! -f $1
        then    echo DEBUG-il primo nome specificato non corrisponde a un file
                exit 2;
	else	echo DEBUG-il primo nome specificato corrisponde a un file
        fi
	if test ! -f $2
        then    echo DEBUG-il secondo nome specificato non corrisponde a un file
                exit 3;
        else    echo DEBUG-il secondo nome specificato corrisponde a un file
        fi
	echo Copia in append il primo file sul secondo.
	cat < $1 >> $2;;
*)	echo Il numero di parametri passati non è corretto.
	exit 1;;
esac
