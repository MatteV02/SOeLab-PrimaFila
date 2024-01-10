#!/bin/sh

#controllo se il numero di parametri e maggiore o uguale a 3
if test $# -lt 3
then
	echo Numero di parametri errato, inseriti $# devono essere maggiori o uguali a 3
	exit 1
fi

#controllo se il parametro $1 e un signolo carattere
case $1 in 
?)
	echo parametro $1 corretto;;	
*)
	echo parametro $2 non corretto inserire singolo carattere
	exit 1;;
esac

Cz=$1
shift
#Inizializzo la variabile Cz dove inserisco i nomi assoluti dei direttori da esplorare
W=
for G  
do
	#controllo che il parametro $W sia un percoso assoluto
	case $G in
	/*)	#controllo che il parametro $G sia una directory attraversabile
		if test -d $G -a -x $G
		then
			echo "parametro $G corretto"
		else
			echo "parametro $G non una directory o non attraversabile"
			exit 2
		fi;;
	*)	echo "parametro $G non assoluto"
		exit 2;;
	esac
	#inserisco il parametro nella variabile G
	W="$W $G"
done

#aggiorno la PATH
PATH=`pwd`:$PATH
export PATH
> /tmp/temp

#ciclo le W fasi
for G in $W
do
	FCR.sh $G $Cz /tmp/temp
done
echo "il numero di file trovati e' `wc -l </tmp/temp`"
#Inizializzo la variabile Files dove inserisco i file trovati dal FCR
Files=
for F in `cat </tmp/temp`
do
	Files="$Files $F"
done
main $Cz $Files

rm /tmp/temp
