#!/bin/sh
#controllo parametri
if test ! $# -ge 3
then
	echo numero di parametri errato
	exit 1
fi
#creo una variabile contatore per tenere traccia di quanti parametri ho controllato
conta=1
G=
#scorro i valori dei parametri
for i
do
	if test $conta -eq $#
	then
		#controllo se ultimo parametro e un numero strettamente positivo
		case $i in
		[!0-9])	echo parametro finale errato inserire un numero
			exit 2;;
			#controllo se il numero inserito e zero
		0)	echo inserire un numero strettamente positivo
			exit 2;;
		esac
		X=$i
	else
		#controllo se il parametro e un nome assoluto di dir
		case $i in
		/*)	echo parametro corretto
			G="$G $i";;
		*)	echo parametro errato
			exit 3;;
		esac
		conta=`expr $conta + 1`
	fi
done
#genero un file temporaneo per contenere i dir trovati che soddisfano le richieste
touch /tmp/nomiAssoluti$$

#aggiorno la path
PATH=`pwd`:$PATH
export PATH

for i in $G
do	#richiamo il ricorsivo per le N fasi
	FCR.sh $i $X /tmp/nomiAssoluti$$
done

echo il numero di direttori trovati `wc -l </tmp/nomiAssoluti$$`
#scorro il file temporaneo
for i in `cat /tmp/nomiAssoluti$$`
do
	echo $i
	#cambio directory
	cd $i
	#esamino la directory in cerca dei file trovati
	for i in *
	do
		if test -f $i
		then
			#ne stampo il nome assoluto e il contenuto della X-esima righa
			echo il file `pwd`/$i
			echo contenuto della righa $X
			tail -n $X <$i | head -n 1
		fi
	done
done

rm /tmp/nomiAssoluti$$
