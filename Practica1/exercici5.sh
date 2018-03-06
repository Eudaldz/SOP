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
            if [ $file == *".ext" ]; then
                for word in &(cat $file)
                    if [ $word == $cadena  ]; then
                        echo "$dir/$file"
                        break
            fi
        else
            if [ -d "$dir/$file" ]
        fi
    done
    exit 0
fi
