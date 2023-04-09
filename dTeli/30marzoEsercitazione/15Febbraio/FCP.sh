#!/bin/sh

#controllo numero parametri ==2
if test ! $# -eq 2
then 	echo no enought params
	exit 1
fi

#controllo se 1 parmametro path assoluto
case $1 in
/*) 	echo parametro 1 corretto;;
*)	echo parametro 1 non assoluto
	exit 2;;
esac

G=$1

#controllo se 2 parametro numero positivo
case $2 in
*[!0-9]*)
	echo parametro 2 non numero pos
	exit 2;;
esac

X=$2

PATH=`pwd`:$PATH
export PATH

FCR.sh $G $X
