#!/bin/sh

cd $1

for i in *
do	if test -f $i -a -r $i
	then	righeFile=`wc -l < $i`
		if test $righeFile -ge $2
		then
			if test $righeFile -ge 5
			then	head -5 < $i | tail -1 > "$i.quinta"
				echo "`pwd`/$i.quinta" >> $3
			else	> "$i.NOquinta"
				echo "`pwd`/$i.NOquinta" >> $3
			fi
		fi
	fi
done

for i in *
do	if test -d $i -a -x $i
	then	$0 `pwd`/$i $2 $3
	fi
done
