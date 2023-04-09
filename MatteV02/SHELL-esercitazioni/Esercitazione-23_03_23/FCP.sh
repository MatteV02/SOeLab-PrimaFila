#!/bin/sh

case $# in
1) 	echo DEBUG-il numero di parametri passato è corretto
	;;
*)	echo DEBUG-il numero di parametri passato non è corretto
	exit 1
	;;
esac

case $1 in
/*)	echo DEBUG-il parametro passato rappresenta un percorso assoluto
	if test -d $1 -a -x $1
	then	echo il parametro passato rappresenta una directory traversabile
	else	echo DEBUG-il parametro passato non rappresenta una directory traversabile
		exit 3
	fi
	;;
*)	echo DEBUG-il parametro passato non rappresenta un percorso assoluto
	exit 2
	;;
esac

echo DEBUG-invoco la file-dir.sh sulla directory passata
PATH=`pwd`:$PATH
export PATH
file-dir.sh $1
