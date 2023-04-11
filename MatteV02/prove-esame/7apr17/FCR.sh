#!/bin/sh
# Primo parametro = gerarchia da esplorare
# Secondo parametro = nome file da cercare
# Terzo parametro = file temporaneo sul quale scrivere i risultati

# mi posiziono sulla gerarchia da esplorare
cd $1

if test -f $2 -a -r $2	# controllo che sia presente il file e che sia leggibile
then	# ordino il file trovato e salvo il suo contenuto su un file di nome sorted in questa gerarchia
	sort -f < $2 > sorted

	# memorizzo il percorso del file creato sul file temporaneo
	echo `pwd`/sorted >> $3
fi

# scorro nuovamente il contenuto della gerarchia alla ricerca di sottodirectory traversabili da esplorare ricorsivamente
for i in *
do	if test -d $i -a -x $i
	then	$0 `pwd`/$i $2 $3
	fi
done
