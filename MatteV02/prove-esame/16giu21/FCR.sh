#!/bin/sh

G=$1	# memorizzo in G la gerarchia da esplorare
H=$2	# memorizzo in H il primo parametro passato alla funzione principale
M=$3	# memorizzo in M il secondo parametro passato alla funzione principale

cd $G	# mi posiziono all'interno della gerarchia da esplorare

echo H\=$H
echo M\=$M

cont=0	# in questa variabile conto i file che rispettano la specifica
files=	# in questa variabile memorizzo i file trovati

# scorro il contenuto della gerarchia da esplorare
for F in *
do	if test -f $F -a -r $F	# se il nome corrisponde ad un file leggibile
	then	case $F in	# controllo che il nome del file corrisponda a 2 caratteri
		??)	#echo file nome 2 caratteri $F
			if test `wc -l < $F` -eq $H	# controllo il numero di linee del file
			then	#echo incremento cont
				cont=`expr $cont + 1`	# incremento la variabile cont
			fi
			files="$files $F"		# memorizzo il file trovato sulla variabile files
			;;
		*)	;;
		esac
	fi
done

if test $cont -lt $M -a $cont -ge 2
then	pwd		# stampo il nome assoluto della directory che corrisponde alle specifiche
	echo Invoco la parte C
	echo 16giu21 $files
fi

for i in *
do	if test -d $i -a -x $i
	then	$0 `pwd`/$i $H $M	# chiamo ricorsivamente la funzione su ogni sottodirectory traversabile della gerarchia
	fi
done
