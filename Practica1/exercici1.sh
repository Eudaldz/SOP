#!/bin/bash

if [ $# -ne 1 ]
then 
    echo Nombre de parametres incorrecte
    exit 1
else
    result=0
    for file in $(ls $1)
    do 
        if [ -f $file ]
        then
            s=($(ls -l $file))
            result=$(expr $echo${s[4]} + $result)
        fi
    done
    echo $result
fi
