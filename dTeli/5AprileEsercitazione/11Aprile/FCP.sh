#!/bin/sh

if test ! $# -ge 2
then
	echo numero parametri errati
	exit 1
fi

#controllo del primo parametro
case $1 in
*[!0-9] | 0) echo primo parametro errato
	 exit 2;;
esac

Y=$1
shift
G=

for i
do
	case $i in
	/*)	echo parametro corretto
		G="$G $i";;
	*)	echo parametro errato
		exit 2;;
	esac
done
#esporto numero di righe minimo
export Y

PATH=`pwd`:$PATH
export PATH

#genero un file temporaneo per gestire il conteggio e output finale
touch /tmp/Conta$$
for i in $G
do
	FCR.sh /tmp/Conta$$ $i
done

#stampo numero di file
echo Il numero di file creati: `wc -l /tmp/Conta$$`
for i in `cat /tmp/Conta$$`
do
	#controllo se il file vuoto e lo notifico
	echo nome del file:
	echo $i
	if test `wc -l <$i` -eq 0
	then
		echo il file e vuoto
	else
		cat $i
	fi
done



