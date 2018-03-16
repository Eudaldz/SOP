#!/bin/bash

if [ $# -ne 0 ]
then
    echo nombre de paràmetres incorrecte!
    exit 1
else

    user="$(whoami)"
    if [ ${#user} -le 8 ]
    then
        ps aux | grep $user | sort -fk3 -r | head -n 5 | awk '{print $2"\t"$3"\t"$11}'
    else
        ps aux | grep ${user:0:7}+ | sort -fk3 -r | head -n 5 | awk '{print $2"\t"$3"\t"$11}'
    fi
    exit 0
    
fi
