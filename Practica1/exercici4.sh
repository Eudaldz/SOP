if [ $# -ne 2 ]
then 
    echo Nombre de parametres incorrecte
    exit 1
else
    dir=$1
    vnum=$2
    
    mkdir $dir/petits
    mkdir $dir/grans
    
    for file in $(ls $dir)
    do 
        if [ -f "$dir/$file" ]
        then
            s=($(ls -l "$dir/$file"))
            if [ $echo${s[4]} -le $vnum ]
            then
                cp $dir/$file $dir/petits/
            else
                cp $dir/$file $dir/grans/
            fi
        fi
    done

    
    exit 0
fi
