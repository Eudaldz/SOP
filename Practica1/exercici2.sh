if [ $# != 1 ]
then
    echo Nombre de parametres incorrecte
    exit 1
else
    input=$1
    cadena=""
    for (( i=${#input}-1 ; i>=0 ; i-- ))
    do
        cadena="$cadena${input:$i:1}"
    done
    echo $cadena
    exit 0
fi
