#!/bin/sh

if test ! 1  -eq $#
then exit 1
fi

if test -f $1
then exit 0
fi

if test -d $1 
then ls -d ./$1 
fi
