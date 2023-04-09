#!/bin/sh

if test $# -eq 1
then 
case $1 in
*/*) echo $1;;
esac
fi
