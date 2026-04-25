import numpy as np
from dtw import dtw
import sys

print("\n==================================================")
print("      VALIDAÇÃO GROUND TRUTH - PYTHON             ")
print("==================================================")

# ==========================================================
# SELEÇÃO DINÂMICA DE CENÁRIO
# ==========================================================
try:
    CENARIO_TESTE = int(input("Digite o número do cenário que deseja testar (0, 1, 2 ou 3): "))
except ValueError:
    print("Erro: Entrada inválida. Digite apenas números inteiros de 0 a 3.")
    sys.exit()

# 1. Alocação (Matrizes de 45 posições zeradas)
signal_a = np.zeros(45)
signal_b = np.zeros(45)

print("\n--------------------------------------------------")

# 2. Carregamento dos Sinais
if CENARIO_TESTE == 0:
    print(">> CENARIO 0: Aceitação total (Sinais exatamente iguais) <<")
    signal_a[10:21] = 1.0  # C: 10 a 20
    signal_b[10:21] = 1.0  # C: 10 a 20

elif CENARIO_TESTE == 1:
    print(">> CENARIO 1: O Atraso Perfeito (Mesmo sinal, tempos diferentes) <<")
    signal_a[10:16] = 1.0  # C: 10 a 15
    signal_b[30:36] = 1.0  # C: 30 a 35

elif CENARIO_TESTE == 2:
    print(">> CENARIO 2: A Distorcao Elastica (Sinal rapido vs Sinal Lento) <<")
    signal_a[10:16] = 1.0  # C: 10 a 15
    signal_b[10:26] = 1.0  # C: 10 a 25

elif CENARIO_TESTE == 3:
    print(">> CENARIO 3: A Rejeicao de Anomalia (Falso Positivo) <<")
    signal_a[10:21] = 1.0  # C: 10 a 20
    signal_b[5:9]   = 1.0  # C: 5 a 8
    signal_b[35:39] = 1.0  # C: 35 a 38
    
else:
    print(f"Erro: Cenário {CENARIO_TESTE} inválido. Escolha entre 0 e 3.")
    sys.exit()

# 3. Execução do Algoritmo DTW
# 'cityblock' é a implementação otimizada do Scipy para Diferença Absoluta
alignment = dtw(signal_a, signal_b, dist_method='cityblock')

# 4. Exibição dos Resultados
print("\nRESULTADOS DO ALINHAMENTO (REFERÊNCIA):")
print(f"-> Distancia Escalar DTW (Python): {alignment.distance:.2f}")
print(f"-> Passos no Caminho Otimo: {len(alignment.index1)} passos")
print("==================================================\n")