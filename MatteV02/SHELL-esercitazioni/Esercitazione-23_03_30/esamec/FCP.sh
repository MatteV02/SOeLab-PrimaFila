#!/bin/sh

case $# in
2)	echo "DEBUG-il numero di parametri passati è corretto"
	;;
*)	echo "Devi passare 2 parametri"
	exit 1;;
esac

case $1 in
/*)	echo "DEBUG-il primo parametro passato rappresenta un percorso assoluto"
	if	test ! -d $1 -o ! -x $1
	then	"Il primo parametro \"$1\" non rappresenta una directory traversabile"
		exit 2
	fi
	echo "DEBUG-il primo parametro rappresenta una directory traversabile"
	;;
*)	echo "Il primo parametro \"$1\" non rappresenta un percorso assoluto"
	exit 3;;
esac

expr $2 + 0 > /dev/null 2>&1
returnExpr=$?
if	test $returnExpr -ne 0
then	echo "Il secondo parametro \"$2\" è un valore non numerico"
	exit 4
fi
if 	test $2 -le 0
then	echo "Il secondo parametro \"$2\" è un valore non strettamente positivo"
	exit 5
fi
echo "DEBUG-Il secondo parametro è un valore strettamente positivo"

tempFile="/tmp/9feb15$$"
cat < /dev/null > $tempFile

PATH=`pwd`:$PATH
export PATH

echo DEBUG-$1 $2 $tempFile

FCR.sh $1 $2 $tempFile

parameterList=
inputVar=
tempFileContent=`cat $tempFile`
rm $tempFile

for i in $tempFileContent
do	echo "File: $i; Inserisci un numero intero strettamente positivo e minore di $2 > "
	read inputVar
	expr $inputVar + 0 > /dev/null 2>&1
	returnExpr=$?
	if      test $returnExpr -ne 0
	then    echo "Il valore inserito non è un numero"
	        exit 6
	fi
	if 	test $inputVar -le 0 -o $inputVar -gt $2
	then    echo "Il numero inserito non è strettamente positivo o è maggiore di $2"
	        exit 7
	fi
	parameterList="$parameterList $i $inputVar"
done

echo "9feb15 $parameterList"
