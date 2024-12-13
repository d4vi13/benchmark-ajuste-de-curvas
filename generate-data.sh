#!/bin/bash

base_dir="dados"
mkdir $base_dir


KPONTOS=(64 128 200 256 512 600 800 1024 2000 3000 4096 6000 7000 10000 50000 10000)
GRAUS=(10 1000)
METRICAS=("L3CACHE" "ENERGY" "FLOPS_DP")  # Substitua por sua métrica desejada

touch "$base_dir/v1-tempo.dat"
# Executa para cada combinação de Kpontos e GrauPol
for k in "${KPONTOS[@]}"; do
    for g in "${GRAUS[@]}"; do
        ./gera_entrada "$k" "$g" | ./ajustePol >> "$base_dir/v1-tempo.dat"
        for m in "${METRICAS[@]}"; do
            echo "Executando com Kpontos=$k e GrauPol=$g..."
            touch "$base_dir/v1-$g-$k-$m.dat"
            ./gera_entrada "$k" "$g" | likwid-perfctr -C 3 -g "$m" -m ./ajustePol > "$base_dir/v1-$g-$k-$m.dat"
        done
    done
done

echo "Execuções concluídas."
