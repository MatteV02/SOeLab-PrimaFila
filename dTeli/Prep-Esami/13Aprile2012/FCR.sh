#!/bin/sh
cd $1
#aggiungo al conteggio il nuovo livello raggiunto
conta=`expr $2 + 1`
#controllo in che fase sono se $# e pari a 2 siamo in fase A
#fase A
for i in *
do
	#controllo se $i e un dir ed attraversabile
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $conta $3 $4 $5
		ret=$?
		read prec <$3
		if test $ret -gt $prec
		then
			echo $ret >$3
		fi
	fi
done
#fase B
if test $4 = B
then
	#controllo se siamo nel livello richiesto e stampo il contenuto
	if test $conta -eq $5
	then
		echo visualizzo `pwd`
		ls -l
	fi

fi

exit $conta

