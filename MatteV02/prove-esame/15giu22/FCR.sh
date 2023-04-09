#!/bin/sh

cd $1

F=

for F in *
do	if test -f $F -a -r $F
	then	if test `wc -c < $F` -ne 0
		then	case `cat < $F` in
			*[!a-z]*)	;;
			*)		echo Trovato il file `pwd`/$F
					echo `pwd`/$F >> $2;;
			esac
		fi
	fi
done

for i in *
do	if test -d $i -a -x $i
	then	$0 `pwd`/$i $2
	fi
done
