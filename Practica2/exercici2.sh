#!/bin/bash

: ' Fem servir una funció auxiliar per a calcular
    el tamany de un directori que li passem per 
    paràmetre. 
'
function sizedir {
    result=0
    for file in $(find $1 -maxdepth 1 -type f)
    do 
        result=$(($result+$(ls -l "$file"| awk '{print $5}')))
    done
    echo $result $1
}

if [ $# -ne 1 ]
then 
    echo Nombre de parametres incorrecte
    exit 1
else

    for dir in $(find . -type d)
    do
        sizedir $dir
    done

    exit 0
fi

