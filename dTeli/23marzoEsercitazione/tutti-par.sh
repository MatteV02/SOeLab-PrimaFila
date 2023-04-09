#!/bin/sh

if test ! $# -ge 1
then echo not enoght
exit 1
fi

for i 
do j=$j$i
echo $i 
done
echo $j

