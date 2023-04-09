#!/bin/sh
#controllo numero di  parametri
if test ! $# -eq 4
then	echo numero di parametri errato
	exit 1
fi
#controllo  primo parametro se percorso assoluto
case $1 in
/*) echo primo parametro corretto;;
*) echo non assoluto
	exit 2;;
esac

G=$1
#controllo secondo e terzo parametro se numero intero positivo
case $2 in
*![0-9]*) 	echo secondo parametro non numero
		exit 3;;
esac

case $3 in
*![0-9]*)  echo terzo non un numero
		exit 3;;
esac
echo parametri 2 3 corretti
N=$2
H=$3

#controllo ultimo se caratte unico
case $4 in
?) echo quarto parametro corretto;;
*) echo quarto non un carattere
	exit 4;;
esac
Cx=$4

#esporto la path con la dir corrente
PATH=`pwd`:$PATH
export PATH

#chiamo ricorsiva
17GennaioRic.sh $G $N $H $Cx
