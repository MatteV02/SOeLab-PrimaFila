#!/bin/sh

case $# in
1)
#	echo DEBUG-è stato passato un solo parametro
	;;
*)	echo Devi passare 1 solo parametro
	exit 1;;
esac

case $1 in
/*)
#	echo DEBUG-il parametro rappresenta un nome assoluto
	if test ! -d $1 -o ! -x $1
	then	echo Il parametro specificato non è una directory o non è traversabile
		exit 2
	fi;;
*)	echo Il parametro specificato non rappresenta un nome assoluto
	exit 3;;
esac

tempFile=/tmp/13apr12-$$
echo 1 > $tempFile

PATH=`pwd`:$PATH
export PATH

FCR.sh $1 1 $tempFile A

read livelliGerarchia < $tempFile
rm $tempFile

echo -n "Inserisci un livello della gerarchia tra 1 e $livelliGerarchia: "
read inputUtente
case $inputUtente in
*[!0-9]*)	echo Hai digitato un valore non numerico!
		exit 4;;
*)		if test $inputUtente -lt 1 -o $inputUtente -gt $livelliGerarchia
		then	echo Il valore digitato non è corretto
			exit 5
		fi;;
esac

FCR.sh $1 1 $inputUtente B
