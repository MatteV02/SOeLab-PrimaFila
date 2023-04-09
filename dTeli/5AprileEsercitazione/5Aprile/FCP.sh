#/bin/sh
G=
con=1
if test ! $# -ge 3
then
	echo troppi pochi params
	exit 1
fi
for i
do
	case $i in
	/*)	echo parametro corretto
		G="$G $i"
		con=`expr $con + 1`;;
	*)
		if test $# -eq $con
		then
			S=$i
		else
			echo Dir non assoluta
			exit 2
		fi ;;
	esac
done

touch /tmp/conta$$
touch /tmp/lung$$

PATH=`pwd`:$PATH
export PATH

for i in $G
do
	FCR.sh $i $S /tmp/conta$$
done
flag=0
echo numero totale di file trovati `wc -l </tmp/conta$$`
for i in `cat /tmp/conta$$`
do
	if test $flag -eq 0
	then
		echo numero di caratteri $i
		flag=1
		continue
	else
		echo $i
		flag=0
	fi
	echo si vuole oridinare il file y/n
	read answer
	case $answer in
	y)	sort -f $i ;;
	n)	echo non ordino il file ;;
	esac
done


rm /tmp/conta$$
rm /tmp/lung$$





