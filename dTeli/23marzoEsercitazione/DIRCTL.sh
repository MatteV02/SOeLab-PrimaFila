#!/bin/sh

if test 1 -eq $#
then echo son dentro ; `pwd`/DIR.sh $1 
else exit 1
fi
