#!/bin/sh
# Primo parametro = gerarchia da esplorare
# Secondo parametro = lunghezza del file da cercare
# Terzo parametro = file temporaneo su cui salvare i risultati
# successivi parametri = caratteri da cercare

# Queste variabili seguono la nomenclatura della funzione principale
G=$1
L=$2
tempFile=$3

shift 3	# elimino i primi tre parametri dalla lista dei parametri

# mi posiziono sulla gerarchia da esplorare
cd $G

# scorro il contenuto della gerarchia
for i in *
do	if test -f $i -a -r $i	# se l'elemento attuale è un file
	then	if test `wc -l < $i` -eq $L	# se il numero di linee di questo file corrisponde al parametro passato dall'utente
		then	fileCorretto=true	# uso questa variabile booleana per capire se il file con il quale ho a che fare contiene tutti i caratteri da cercare o meno
			for C
			do	case `cat < $i` in	# controllo se un carattere passato dall'utente è presente all'interno del contenuto del file, se non è presente vmodifico il valore della variabile booleana
				*$C*)	;;
				*)	fileCorretto=false
					break;;		# se non trovo il carattere esco direttamente dal ciclo for
				esac
			done

			if test $fileCorretto = true	# se fileCorretto contiene true allora il file rispetta le specifiche dell'esercizio: stampo il suo percorso assoluto e memorizzo il suo contenuto sul file temporaneo
			then	echo File trovato\: `pwd`/$i
				echo `pwd`/$i >> $tempFile
			fi
		fi
	fi
done

# scorro nuovamente il contenuto della gerarchia alla ricerca di directory traversabili nelle quali eseguire la ricerca ricorsivamente
for i in *
do	if test -d $i -a -x $i
	then	$0 $G/$i $L $tempFile $*
	fi
done
