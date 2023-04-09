#!/bin/sh

cd $2

for i in *
do
	if test -f $i -a -r $i
	then
		linee=`wc -l <$i`
		if test $linee -ge $Y
		then
			if test ! $linee -ge 5
			then
				touch $i.NOquinta
				echo `pwd`/$i.NOquinta >>$1
			else
				touch $i.quinta
				echo file
				echo `head -n 5 <$i | tail -n 1 >$i.quinta`
				echo `pwd`/$i.quinta >>$1
			fi
		fi
	else
		if test -d $i -a -x $i
		then
			FCR.sh $1 `pwd`/$i
		fi
	fi
done


