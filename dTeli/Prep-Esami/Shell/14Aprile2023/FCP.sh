#!/bin/sh

#controllo il numero di parametri
if test ! $# -ge 4
then
	echo numero parametri errato
	exit 1
fi

#controllo per scrupolo se ci sono / nelle stringe
case $1 in
*/*)	echo striga 1  non valida
	exit 2;;
*)	echo parametro 1 corretto
esac

#inserisco il primo parametro nella variabile
Str1=$1

#preparo i parametri per il controllo della seconda stringa
shift

#controllo per scrupolo se ci sono / nelle stringe
case $1 in
*/*)	echo striga 2 non valida
	exit 2;;
*)	echo parametro 2 corretto
esac

#inserisco nella variabile il secondo parametro
Str2=$1

#preparo i parametri per il controllo delle directory
shift

#controllo per scrupolo se le due stringe sono uguali
if test $Str2 = $Str1
then
	echo le stringe sono uguali
	exit 3
fi
#creo una variabile per inserire le X gerachie
X=
#ciclo i parametri rimanenti controllando se siano percorsi assoluti di dir
for i
do
	#controllo se il parametro e un percorso assoluto
	case $i in
	/*)	#controllo se il parametro sia una directory attraversabile
		if test -d $i -a -x $i
		then
			X="$X $i"
			echo parametro corretto
		fi;;
	*)	echo inserire percorso assoluto
		exit 3;;
	esac
done

#aggiorno il path
PATH=`pwd`:$PATH
export PATH

#creo i file temporanei per la memorizzazione dei file
>/tmp/AbsoluteNames$Str1
>/tmp/AbsoluteNames$Str2

#creo delle variabili per contenere il nomi dei file
file1=/tmp/AbsoluteNames$Str1
file2=/tmp/AbsoluteNames$Str2

#inizio a esporare le gerarchie
for D in $X
do	#inizio le X fasi
	FCR.sh $D $Str1 $Str2 $file1 $file2
done
#al termine delle X fasi

#genero i file con il numero di file trovati
tot1=`wc -l <$file1`
tot2=`wc -l <$file2`

#stampo i dati richiesti
echo "numero di file con estensione .$Str1 $tot1"
echo "numero di file con estensione .$Str2 $tot2"

#controllo il se tot1 e maggiore uguale di tot2
if test $tot1 -le $tot2
then
	echo Daniele inserire un numero fra 1 e $tot1
	read H
	#controllo per scrupolo se esso e un numero
	case $H in
	[!0-9]) echo non un numero
		#rimuovo i file temporanei creati in prec
		rm $file1
		rm $file2
		exit 4;;
	esac
	#controllo se il numero inserito rispetta le specifiche 1>H>tot1
	if test ! $H -ge 1 -a ! $H -le $tot1
	then
		echo numero errato
		#rimuovo i file temporanei creati in prec
		rm $file1
		rm $file2
		exit 4
	fi
	#stampo i file richiesti
	echo "il file .$Str1 e `head -$H <$file1 | tail -1` il file .$Str2 `head -$H <$file2 | tail -1`"
fi
#rimuovo i file temporanei creati in prec
rm $file1
rm $file2
