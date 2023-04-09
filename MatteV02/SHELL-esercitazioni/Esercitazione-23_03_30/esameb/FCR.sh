#!/bin/sh

# $1 = directory su cui effettuare la ricerca
# $2 = numero linee file

# entro nella cartella nella quale eseguire la ricerca
cd $1

# Variabile che conterrà i file trovati dalla ricerca
fileTrovati=

# Scorro la directory corrente
for i in *
do	echo "DEBUG-analizzo il file `pwd`/$i"
	if	test -f $i -a -r $i	# controllo se il nome attuale rappresenta un file leggibile
	then	echo "DEBUG-$i rappresenta un file leggibile"
		numeroLinea=1		# questa variabile memorizza il numero della linea che vogliamo leggere del file in $i
		totaleLineeFile=`wc -l < $i`	# questa variabile memorizza il numero di linee presenti nel file in $i
		if test $totaleLineeFile -eq $2	# se il numero di linee del file è il valore cercato allora proseguo con il controllo delle specifiche
		then 	echo "DEBUG-il file `pwd`/$i ha $2 linee"
			fileCorretto=true	# se il file presenta una linea che non contiene caratteri numerici questa variabile è posta a "false"
			while	test $numeroLinea -le $totaleLineeFile	# scorro il file
			do	linea=`head -$numeroLinea < $i | tail -1`	# estraggo la $numeroLinea-esima linea del file
				echo "DEBUG-linea numero $numeroLinea estratta: $linea"
				# Se non trovo caratteri numerici nella linea impongo fileCorretto a "false" e termino l'esecuzione del while
				case $linea in
				*[0-9]*)	echo "DEBUG-carattere numerico trovato alla riga $numeroLinea"
						;;
				*)		echo "DEBUG-nessun carattere numerico trovato alla riga $numeroLinea"
						fileCorretto=false
						break
						;;
				esac
				numeroLinea=`expr $numeroLinea + 1`
			done
			if	test $fileCorretto = "true"	# entro in questo if solo se il file $i rappresenta la specifica
			then	echo "DEBUG-il file è corretto"
				fileTrovati="$fileTrovati $i"	# aggiungo il file $i alla lista dei risultati
			fi
		fi
	else	if 	test -d $i -a -x $i
		then	# eseguo la chiamata ricorsiva
			$0 `pwd`/$i $2
		fi
	fi
done


if	test ! -z $fileTrovati # Se la stringa fileTrovati non è vuota allora la directory corrisponde alle specifiche
then	echo "Una directory che corrisponde alle specifiche è: `pwd`"
	echo "I file che corrispondo alla specifica nella directory sono: $fileTrovati"
	echo "Vuoi eseguire la parte C su questa directory? (y/Y): "
	risposta=	# Questa variabile conterrà la risposta dell'utente rispetto all'esecuzione della parte C
	read risposta
	case $risposta in
	y | Y)	echo 15feb17 $fileTrovati $2
		;;
	*)	echo "DEBUG-Non eseguo la stampa"
		;;
	esac
fi
