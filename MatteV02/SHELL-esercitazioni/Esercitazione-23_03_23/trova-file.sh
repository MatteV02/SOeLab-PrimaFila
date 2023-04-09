#!/bin/sh
case $# in
1)	case $1 in
	*/*) 	echo il nome passato deve corrispondere ad un percorso relativo semplice
		exit 2;;
	*) 	ls | grep $1 > /dev/null
		if test $? -eq 0
		then 	echo il percorso assoluto del file specificato è $PWD/$1
		else	echo il file specificato non è presente nella directory corrente
			exit 3
		fi;;
	esac;;
*)	echo il numero di parametri passato non è corretto
	exit 1;;
esac
