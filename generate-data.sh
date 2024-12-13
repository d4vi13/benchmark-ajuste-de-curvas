#!/bin/bash

mkdir $base_dir

tempo_line=$(grep "^#tempo#" "$input_file" | cut -d'#' -f3-)


KPONTOS=(64 128 200 256 512 600 800 1024 2000 3000 4096 6000 7000 10000 50000 10000)
GRAUS=(10 1000)
METRICAS=("L3CACHE" "ENERGY" "FLOPS_DP")  # Substitua por sua métrica desejada

# Executa para cada combinação de Kpontos e GrauPol
for k in "${KPONTOS[@]}"; do
    for g in "${GRAUS[@]}"; do
        ./gera_entrada "$k" "$g" | ./ajustePol >> "v1-tempo.dat"
        for m in "${METRICAS[@]}"; do
            echo "Executando com Kpontos=$k e GrauPol=$g..."
            ./gera_entrada "$k" "$g" | likwid-perfctr -C 3 -g "$m" -m ./ajustePol > "$base_dir/v1-$g-$k-$m.dat"
        done
    done
done

echo "Execuções concluídas."
