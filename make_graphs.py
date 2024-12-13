import pandas as pd


versoes = ["v1", "v2"]
graus = [10, 1000]
kpontos =  [64, 128, 200, 256, 512, 600, 800, 1024, 2000, 3000, 4096, 6000, 7000, 10000, 50000, 100000]
metricas = ["L3CACHE", "ENERGY", "FLOPS_DP"]


def find_metric(table, metric):
    filtered_row = table[table[0].str.startswith(metric, na=False)]
    return filtered_row.iloc[0, 1] if not filtered_row.empty else None

for v in versoes:
    for m in metricas:
        for grau in graus:
            for pontos in kpontos:
               data = pd.read_csv(v+'-'+grau+'-'+k+'-'+m+'.csv') 
               split_table = data[data[0].str.startswith('TABLE'), na=False].index().tolist()
               table_monta_sl = data.iloc[:split_table[0]]
               table_resolve_sl = data.iloc[split_table[0] + 1:]
              
               if m == "L3CACHE"
                    metric =  "L3 miss ratio"
               elif m == "ENERGY"
                    metric = "Energy [J]"
               else
                    metric = "FLOPS_DP
 
               dado = find_metric(table_monta_sl, metric)  
            
            
        
