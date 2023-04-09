#!/bin/sh

countFiles=0		# Conta i file
countDirectories=0	# Conta le directory

cd $1

for i in *
do
if test -f $i
then	echo "DEBUG-"$i è un file
	countFiles=`expr $countFiles + 1`
	echo "[F]" `pwd`/$i
else	echo "DEBUG-"$i è una directory
	countDirectories=`expr $countDirectories + 1`
        echo "[D]" $PWD/$i
fi
done

echo ---
echo Numero dei file processati = $countFiles
echo ---
echo Numero delle directory processate = $countDirectories
