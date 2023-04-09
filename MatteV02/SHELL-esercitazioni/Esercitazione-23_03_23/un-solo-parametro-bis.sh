#!/bin/sh
case $# in
1)	case $1 in
	/*) echo Il percorso è assoluto;;
	*/*) echo Il percorso è relativo;;
	*) 	echo Il percorso è relativo semplice
		if test -f $1
		then echo $1 è un file
		else echo $1 è una directory
		fi;;
	esac;;
*)	echo Il numero di parametri passati non è corretto;;
esac
