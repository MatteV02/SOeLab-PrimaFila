#!/bin/sh
#controllo se il numero di parametri e maggiore o uguale a 2
if test $# -lt 2
then
	echo Numero di parametri errato, inseriti $# devono essere maggiori o uguali a 2
	exit 1
fi

#Inizializzo la variabile Ger dove inserisco i percorsi delle directory
Ger=
for G  
do
	Ger="$Ger $G" 
done
> /tmp/nomiAssoluti

#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH
#ciclo le Q fasi
for G in $Ger
do
	FCR.sh $G /tmp/nomiAssoluti
done
N=`wc -l </tmp/nomiAssoluti`
echo numero di file trovati $N
#Inizializzo la variabile Ger dove inserisco i file troviati dal FCR
Ger=
for i in `cat </tmp/nomiAssoluti`
do
	Ger="$Ger $i"
done

main $Ger

rm /tmp/nomiAssoluti