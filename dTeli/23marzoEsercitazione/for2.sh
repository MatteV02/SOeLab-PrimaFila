#!/bin/sh
#file comandi con primo esempio di for
for i in *	#per tutti i file/directory della directory corrente (ATTENZIONE NON QUELLI IL CUI NOME INIZIA PER .)
do
	case $i in 
	p*)
		if test -f $i
		then cat $i
		else echo $i directory
		fi
	esac
done
