#!/bin/sh

#controllo se il numero di parametri e maggiore o uguale a 3
if test $# -lt 3
then
	echo Numero di parametri errato, inseriti $# devono essere maggiori o uguali a 3
	exit -1
fi

#controllo se il parametro $1 e un singolo carattere
case $1 in 
?)
	echo parametro $1 corretto;;	
*)
	echo parametro $1 deve essere un singolo carattere
	exit 2;;
esac
Cz=$1
shift
#Inizializzo la variabile W dove inserisco i nomi assoluti di directory
W=
for G  
do
	#controllo che il parametro $G sia un percoso assoluto
	case $G in
	/*)	#controllo che il parametro $G sia una directory attraversabile
		if test -d $G -a -x $G
		then
			echo "parametro $G corretto"
		else
			echo "parametro $G non una directory o non attraversabile"
			exit 3
		fi;;
	*)	echo "parametro $G non assoluto"
		exit 3;;
	esac
	#inserisco il parametro nella variabile W
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

#Inizializzo la variabile Out dove inserisco i file conformi alle specifiche trovati da FCR
Out=
for G in `cat </tmp/temp`
do
	Out="$Out $G"
done
main $Out $Cz
rm /tmp/temp