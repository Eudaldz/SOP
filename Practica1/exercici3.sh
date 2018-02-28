#!/bin/bash

if [ $# -ne 2 ]
then 
    echo Nombre de parametres incorrecte
    exit 1
else
    for file in $(ls $1)
    do
        absfile=$1/$file
        if [ -f $absfile ]
        then
            for word in $(cat $absfile)
            do
                newword=${word#$2}
                if [ ${#newword} -ne ${#word} ]
                then
                    echo "$file : $word"
                fi
            done
        fi
    done
    exit 0
fi
