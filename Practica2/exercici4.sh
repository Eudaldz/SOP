#!/bin/bash

if [ $# -ne 3 ]
then
	echo "nombre paramatres incorrecte"    
	exit 1
else
    	ext=$1
	c1=$2
	c2=$3
	contar_paraula="echo {} \$(grep -o $c1 {} | wc -l); sed -i -- 's/$c1/$c2/g' {}" #codi a executar en el find
	find . -name "*.$ext" -type f -exec bash -c "$contar_paraula" \;	
    	
	exit 0
    
fi
