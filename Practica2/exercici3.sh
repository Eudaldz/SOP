#!/bin/bash

if [ $# -ne 2 ]
then
    exit 1
else
    dir1=$1
    dir2=$2
    for file in $(find $dir1 -type f)
    do
	file_c=$dir2${file#$dir1}#alternem directori        
	if ! [[ -f $file_c  ]]; then
        	echo "$file_c $file"
	fi
        
    done
    exit 0
    
fi
