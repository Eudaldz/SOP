#!/bin/bash

if [ $# -ne 2 ]
then
    exit 1
else
    dir1=$1
    dir2=$2
    
    #find $dir1 -type f -exec echo $dir2${{}#$dir1} \;
    #find $dir1 -type f -exec file={} \; find $dir2${file#$dir1} \; if [ 1 -eq $? ] \; then echo {} \; fi \;
    for file in $(find $dir1 -type f)
    do
        find $dir2${file#$dir1}
        if [ 1 -eq $? ] 
        then 
            file
        fi
        
    done
    exit 0
    
fi
