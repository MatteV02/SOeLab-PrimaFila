#!/bin/sh
cd $1
flag=0
for i in *
do
	if test -f $i -a -r $i -a $flag -eq 0
	then
		case $i in
		*.$2)
			if test $4 = A
			then
				flag=1
				echo `pwd` >>$3
			else
				echo file `pwd`/$i
				echo la prima riga
				head -n $3 <$i | tail -n 1
			fi;;
		esac
	else
		if test -d $i -a -x $i -a $4 = A
		then
			FCR.sh `pwd`/$i $2 $3 $4
		fi
	fi
done
