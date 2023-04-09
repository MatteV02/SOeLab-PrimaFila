#!/bin/sh
#file comandi con primo esempio di for
for i in `ls -a`	#per tutti i file/directory della directory corrente (ATTENZIONE NON QUELLI IL CUI NOME INIZIA PER .)
do
	
	case $i in
	.*)	 echo File $i;;
	esac
done
