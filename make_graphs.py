import pandas as pd
import csv
import re
import numpy as np
import matplotlib.pyplot as plt


basedir = "dados/parsed/"

funcoes = ["montaSl", "resolveSl"]
versoes = ["v1", "v2"]
graus = [10, 1000]
kpontos =  [64, 128, 200, 256, 512, 600, 800, 1024, 2000, 3000, 4096, 6000, 7000, 10000, 50000, 100000]
metricas = ["L3CACHE", "ENERGY", "FLOPS_DP"]
submetricas = ["L3 miss ratio","Energy [J]", "DP MFLOP/s"]
colors = ["b", "g", "r", "y"]

def find_tag_value(file_path, tag):
    with open(file_path, mode='r') as file:
        reader = csv.reader(file)
        
        occurrences = []
        
        for row in reader:
            if row and row[0].strip().startswith(tag):
                occurrences.append(row)
                if len(occurrences) == 2:
                    break
        
        return occurrences

def find_metric(table, metric):
    filtered_row = table[table[0].str.contains(metric, na=False)]

    if not filtered_row.empty and filtered_row.shape[1] > 1:
        return filtered_row.iloc[0, 2]  # Access the third column (index 2)
    else:
        return None


for v in versoes:
    for grau in graus:
        montaSl = {
                'L3CACHE' : [],
                'ENERGY': [],
                'FLOPS_DP' : []
                #'flops_avx' : []
                }

        resolveSl = {
                'L3CACHE' : [],
                'ENERGY': [],
                'FLOPS_DP' : []
                #'flops_avx' : []
                }


        for m in metricas:
            for pontos in kpontos:
               #rawdata = read_file("dados/"+v+'-'+str(grau)+'-'+str(pontos)+'-'+m+'.csv') 
               ###int(rawdata)
               #data = parse_data(rawdata)
              
               if m == "L3CACHE":
                    metric =  "L3 miss ratio"
               elif m == "ENERGY":
                    metric = "Energy [J]"
               else:
                    metric = "DP MFLOP/s"
                
               dados = find_tag_value("dados/"+v+'-'+str(grau)+'-'+str(pontos)+'-'+m+'.csv', metric) 
               print("dados/"+v+'-'+str(grau)+'-'+str(pontos)+'-'+m+'.csv') 
                
               print(montaSl)
               print(dados)
               montaSl[m].append(dados[0][1])
               resolveSl[m].append(dados[1][1])

        pd.DataFrame(montaSl).to_csv(basedir+v+"-"+str(grau)+"-"+"montaSl"+".csv",index=False)
        pd.DataFrame(resolveSl).to_csv(basedir+v+"-"+str(grau)+"-"+"resolveSl"+".csv",index=False)
        
#monta graficos
for a,m in enumerate(metricas):
    for j,funcao in enumerate(funcoes):
        for k,v in enumerate(versoes):
            for grau in graus:
                data = np.genfromtxt(basedir+v+"-"+str(grau)+"-"+"montaSl"+".csv", delimiter=',', skip_header=1)
                y_values = data[:, a]
                plt.plot(kpontos, y_values, marker='o', linestyle='-', color=colors[j+k], label=v+" "+str(grau))  # Ajuste a legenda conforme necessário
        plt.xlabel('numero de pontos')
        plt.ylabel(submetricas[a])
        plt.title(m + " " + funcao)
        plt.legend()
        plt.grid(True)
        plt.savefig(v+"-"+funcao+"-"+m)




        
