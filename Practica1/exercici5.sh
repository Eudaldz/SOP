#!/bin/bash
#falta testejar.

if [ $# -ne 3 ]
then 
    echo Nombre de parametres incorrecte
    exit 1 
else
    ext=$2
    dir=$1
    string=$3
    
    for file in $(ls $dir); do
        if [ -f "$dir/$file" ]; then
            if [[ $file == *".$ext" ]]; then
                for word in $(cat $dir/$file); do
                    if [ $word == $string ]; then
                        echo "$dir/$file"
                        break
                    fi
                done
            fi
        else
            if [ -d "$dir/$file" ]; then
                ./exercici5.sh $dir/$file $ext $string
            fi
        fi
    done
    exit 0
fi
