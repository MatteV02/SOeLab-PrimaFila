#!/bin/sh

case $# in
2)	echo DEBUG-due parametri passati;;
*)	echo Il numero di parametri passati non è corretto.
	exit 1;;
esac

case $1 in
*/*)	echo DEBUG-il nome del primo file non è relativo semplice
	exit 2;;
*)	echo DEBUG-il nome del primo file è relativo semplice;;
esac

case $2 in
*/*)    echo DEBUG-il nome del secondo file non è relativo semplice
        exit 2;;
*)      echo DEBUG-il nome del secondo file è relativo semplice;;
esac

if test ! \( -f $1 -a -r $1 \)
then    echo DEBUG-il primo nome specificato non corrisponde a un file o non è leggibile
        exit 3;
else    echo DEBUG-il primo nome specificato corrisponde a un file leggibile
fi

if test ! \( -f $2 -a -w $2 \)
then    echo DEBUG-il secondo nome specificato non corrisponde a un file o non è scrivibile
        exit 4;
else    echo DEBUG-il secondo nome specificato corrisponde a un file scrivibile
fi

echo DEBUG-metto in append il primo file sul secondo
cat < $1 >> $2
