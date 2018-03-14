#!/bin/bash

if [ $# -ne 2 ]
then 
    echo Nombre de parametres incorrecte
    exit 1
else
    ls -l $(find $1 -name "*.$2") | sort -nk5 | awk '{print $9}'
    exit 0
fi
