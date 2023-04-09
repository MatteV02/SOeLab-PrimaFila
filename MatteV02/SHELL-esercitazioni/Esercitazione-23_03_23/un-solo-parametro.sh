#!/bin/sh
case $# in
1)	case $1 in
	/*) echo Il percorso è assoluto;;
	*/*) echo Il percorso è relativo;;
	*) echo Il percorso è relativo semplice;;
	esac;;
*)	echo Il numero di parametri passati non è corretto;;
esac
