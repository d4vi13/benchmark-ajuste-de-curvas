import pandas as pd

basedir = "dados/parsed/"
funcoes = ["montaSl", "resolveSl"]
versoes = ["v1", "v2"]
graus = [10, 1000]
kpontos =  [64, 128, 200, 256, 512, 600, 800, 1024, 2000, 3000, 4096, 6000, 7000, 10000, 50000, 100000]
metricas = ["L3CACHE", "ENERGY", "FLOPS_DP"]

montaSl = {
        'pontos': [],
        'L3CACHE' : [],
        'ENERGY': [],
        'FLOPS_DP' : [],
        'flops_avx' : []
        }

resolveSl = {
        'pontos': [],
        'L3CACHE' : [],
        'ENERGY': [],
        'FLOPS_DP' : [],
        'flops_avx' : []
        }

def find_metric(table, metric):
    filtered_row = table[table[0].str.startswith(metric, na=False)]
    return filtered_row.iloc[0, 1] if not filtered_row.empty else None

for v in versoes:
    for grau in graus:
        for m in metricas:
            for pontos in kpontos:
               data = pd.read_csv("dados/"+v+'-'+grau+'-'+pontos+'-'+m+'.csv') 
               split_table = data[data[0].str.startswith('TABLE'), na=False].index().tolist()
               table_monta_sl = data.iloc[:split_table[0]]
               table_resolve_sl = data.iloc[split_table[0] + 1:]
              
               if m == "L3CACHE"
                    metric =  "L3 miss ratio"
               elif m == "ENERGY"
                    metric = "Energy [J]"
               else
                    metric = "FLOPS_DP/s"

               dado1 = find_metric(table_monta_sl, metric)  
               dado2 = find_metric(table_resolve_sl, metric)  
            
               montaSl["pontos"].append(pontos)
               resolveSl["pontos"].append(pontos)
            
               montaSl[m].append(dado1)
               resolveSl[m].append(dado2)

        pd.DataFrame(montaSl).to_csv(basedir+v+"-"+grau+"-"+"montaSl"+".csv",index=False)
        pd.DataFrame(resolveSl).to_csv(basedir+v+"-"+grau+"-"+"resolveSl"+".csv",index=False)
        for a, b in montaSl, resolveSl
            a.clear() 
            b.clear() 
        
               
        
