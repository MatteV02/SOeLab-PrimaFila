#!/bin/sh
case $# in
0)  	ls -R | more;;

1)	echo SONO DIR.sh
	echo 'Il valore di $0 ===>' $0
	echo 'Il valore di $1 ===>' $1
	echo "DEBUG-Ora eseguo ls -l $1"
	if test -f $1
	then ls -l $1
	else 	if test -d $1
		then ls -ld $1
		fi
	fi;;
*) echo Il numero di parametri passati non è corretto;;
esac
