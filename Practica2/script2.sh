#!/bin/bash

if [ "$#" -eq 1 ]; then
	input=$(cat $1)
	num=5
elif [ "$#" -eq 2 ]; then
	input=$(cat $2)
	num=$1
else
	echo "Nombre de parametres il.legal"
	exit 1
fi

#seperar paraules amb 'new lines' sense commandes noves
textArray=($input)
inputWords=()
for word in "${textArray[@]}"
do
	inputWords+=(${word}'\n')
done

#len="${#inputWords[@]}"
#echo $len
#echo ${inputWords[0]}
#echo ${inputWords[1]}
#echo ${inputWords[$((len-1))]}
#echo ${inputWords[@]: -1}

sortedWords=$(echo -e "${inputWords[@]}"|sort)
#echo ORDENACIO
#echo -e "$sortedWords"

#comptar repeticions i eliminarles
token=''
tokenCount=0
wordsCount=()

for word in $sortedWords EOF
do
	if [ "$token" != "$word" ]
	then
		wordsCount+=(${token}' '${tokenCount}'\n')
		token=$word
		tokenCount=1
	else
		tokenCount=$(($tokenCount+1))
	fi
done

#echo -e "${wordsCount[@]}"

freqWords=$(echo -e "${wordsCount[@]}"|sort -nr -k 2,2)

echo "$freqWords"|head -n $num

exit 0
