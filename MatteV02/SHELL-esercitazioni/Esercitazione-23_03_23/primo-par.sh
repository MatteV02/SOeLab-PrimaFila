#!/bin/sh

if test $# -eq 0
then 	echo "Il numero di parametri passati non è corretto"
	exit 1
fi

# Viene passato almeno un parametro

echo "Il primo parametro vale:" $1

shift

echo "Gli altri parametri valgono:" $*
