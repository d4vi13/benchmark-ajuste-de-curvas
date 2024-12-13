#!/bin/bash


mkdir "dados/parsed"

version="v1"
KPONTOS=(64 128 200 256 512 600 800 1024 2000 3000 4096 6000 7000 10000 50000 10000)
GRAUS=(10 1000)
METRICAS=("L3CACHE" "ENERGY" "FLOPS_DP") 



extract(){  
    local file=$1
    local tag=$2
    local dest=$3
    cat $file | awk -v search=$tag '
    BEGIN { results = "" }
    {
        if ($file ~ search && $file ~ /\|/) {
            split($file, columns, "|")
                gsub(" ", "", columns[length(columns)])
                if (results == "") {
                    results = columns[length(columns)] "\n"
                } else {
                    results = results "," columns[length(columns)] "\n"
                }
        }
    }
    END {
        if (results != "") {
            print results >> dest
        } else {
            print "String não encontrada!" > "/dev/stderr"
                exit 1
        }
    }
    '
}



for g in "${GRAUS[@]}";do 
    for k in "${KPONTOS[@]}"; do
        for m in "${METRICAS[@]}"; do  
            if [$m = "L3CACHE"]; then 
                extract "dados/raw/$version-$g-$k-$m.dat"  "L3 miss ratio" "dados/parsed/$version-$g-$m.csv"                 
            elif [$m = "ENERGY"]; then
                extract "dados/raw/$version-$g-$k-$m.dat"  "Energy [J]" "dados/parsed/$version-$g-$m.csv"                 
            else
                 
                extract "dados/raw/$version-$g-$k-$m.dat" "DP MFLOP/s" "dados/parsed/$version-$g-$m.csv"                 
                extract "dados/raw/$version-$g-$k-$m.dat" "AVX DP MFLOP/s" "dados/parsed/$version-$g-$m.csv"                 
            fi
    
        done  
    done
done

