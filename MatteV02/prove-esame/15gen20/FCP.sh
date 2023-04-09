#!/bin/sh

if test $# -lt 3
then	echo Errore\: devi passare almeno 3 parametri
	exit 1
fi

H=

case $1 in
*[!0-9]*) 	if test $1 -eq 0
		then	echo "Devi passare un numero intero strettamente postivo come primo parametro"
			exit 2
		fi
		H=$1
*
