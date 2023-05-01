#!/bin/sh

case $# in
2) echo nice $1 $2
case $1 in 
./*)echo rel nice;;
*)echo not good 
exit 1;;
esac

case $2 in
./*)echo rel nice;;
*)echo not good
exit 1;;
esac

if test ! -f $1 -a ! -x $1
then echo not good1
exit 2
fi
if test -f $2
then echo veri nice
else echo not files
exit -2
fi

cat $2>>$1;;
*) echo not good 
exit 1;;
esac

#!/bin/sh

#cambio la dir corrente alla directory da esplorare
cd $1 

#esploro la directory
for F in *
do
    
done

#cerco nuove directory da esplorare
for F in *
do
    #Se F e una directory attraversabile allora richiamo la funzione ricorsiva ed la esploro
    if test -d F -a -x F
    then
        FCR.sh `pwd`/F Parametri aggiuntivi
    fi
done