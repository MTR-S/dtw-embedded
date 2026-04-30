"""
Script de Automação, Validação de Paridade e Benchmarking do Algoritmo DTW.

Este script atua como o orquestrador (Test Runner) para a disciplina de Sistemas Embarcados.
Ele replica internamente 20 casos de uso focados em processamento de sinais, 
executa a implementação de referência (SciPy via dtw-python) e a compara em tempo 
real com o binário em C recém-compilado (dtw_app). O objetivo é garantir 100% de 
precisão matemática e auditar a performance (tempo de execução).

Autor: Matheus de Sousa Almeida e Vinicius Silva Pereira
Data: Abril de 2026
"""

import numpy as np
from dtw import dtw
import subprocess
import time
import re
import sys
import os

EXECUTAVEL_C = "./dtw_app"  # Caminho para o binário compilado do projeto C
ARQUIVO_SAIDA = "relatorio_dtw.txt" # Nome do arquivo de relatório a ser gerado

def gerar_sinais(cenario: int) -> tuple[np.ndarray, np.ndarray]:
    """
    Gera o par de sinais sintéticos para o cenário de teste especificado.

    Args:
        cenario (int): O ID numérico (0-19) do cenário desejado.

    Returns:
        tuple: Retorna uma tupla (sig_a, sig_b) contendo dois arrays numpy de 45 elementos.
    """
    sig_a, sig_b = np.zeros(45), np.zeros(45)
    
    # Categoria 1: Ideais
    if cenario == 0: sig_a[10:21], sig_b[10:21] = 1, 1
    elif cenario == 1: sig_a[15:19], sig_b[15:19] = 1, 1
    elif cenario == 2: sig_a[5:41], sig_b[5:41] = 1, 1
    elif cenario == 3: sig_a[10:16], sig_b[10:16] = 1, 1; sig_a[30:36], sig_b[30:36] = 1, 1
    # Categoria 2: Shift (Atrasos temporais)
    elif cenario == 4: sig_a[10:21] = 1; sig_b[15:26] = 1
    elif cenario == 5: sig_a[20:31] = 1; sig_b[10:21] = 1
    elif cenario == 6: sig_a[5:11] = 1; sig_b[35:41] = 1
    elif cenario == 7: sig_a[30:36] = 1; sig_b[5:11] = 1
    # Categoria 3: Warping (Distorção Elástica)
    elif cenario == 8: sig_a[10:16] = 1; sig_b[10:26] = 1
    elif cenario == 9: sig_a[10:26] = 1; sig_b[10:16] = 1
    elif cenario == 10: sig_a[5:11] = 1; sig_b[5:36] = 1
    elif cenario == 11: sig_a[5:36] = 1; sig_b[5:11] = 1
    # Categoria 4: Anomalias e Ruídos
    elif cenario == 12: sig_a[10:21] = 1; sig_b[10:21] = 1; sig_b[2:4] = 1
    elif cenario == 13: sig_a[10:21] = 1; sig_b[10:21] = 1; sig_b[42:44] = 1
    elif cenario == 14: sig_a[10:21] = 1; sig_b[5:9] = 1; sig_b[35:39] = 1
    elif cenario == 15: sig_a[10:21] = 1; sig_b[0:10] = 1; sig_b[21:45] = 1
    # Categoria 5: Extremos e limites de matriz
    elif cenario == 16: pass
    elif cenario == 17: sig_a[:], sig_b[:] = 1, 1
    elif cenario == 18: sig_a[10:21] = 1
    elif cenario == 19: sig_b[10:21] = 1

    return sig_a, sig_b

def categorizar(c: int) -> str:
    """Classifica o cenário em categorias semânticas amigáveis para o relatório."""
    if c < 4: return "1. Sinais Iguais"
    if c < 8: return "2. Atraso Temporal"
    if c < 12: return "3. Distorcao (Warping)"
    if c < 16: return "4. Ruidos/Anomalias"
    return "5. Extremos de Sensor"

# ==============================================================================
# INÍCIO DO PROCESSAMENTO E FORMATAÇÃO VISUAL
# ==============================================================================
relatorio = []
linha_divisoria = "-" * 135

cabecalho = f"{'ID':<3} | {'CATEGORIA':<22} | {'DISTÂNCIA ESCALAR DTW':<26}  | {'PASSOS ROTA':<11}         | {'TEMPOS DE EXECUÇÃO (us)':<25}  | {'PARIDADE'}\n"
sub_cabecalho = f"{'':<3} | {'':<22} | {'Python (SciPy)':<12}| {'Linguagem C':<11} | {'Python':<4} |{'Linguagem C':<4} | {'Python':<12}| {'Linguagem C':<10}  | \n"

relatorio.append("="*135 + "\nRELATORIO OFICIAL DE VALIDACAO E BENCHMARKING DTW (PYTHON SciPy vs FIRMWARE C)\n" + "="*135 + "\n")
relatorio.append(cabecalho)
relatorio.append(sub_cabecalho)
relatorio.append(linha_divisoria + "\n")

print(cabecalho, end="")
print(sub_cabecalho, end="")
print(linha_divisoria)

# ==============================================================================
# MOTOR DE EXECUÇÃO DOS CASOS (TEST RUNNER)
# ==============================================================================
for cenario in range(20):
    sig_a, sig_b = gerar_sinais(cenario)
    
    # ------------------------------------
    # Execução do modelo de referência (Python)
    # ------------------------------------
    start_py = time.perf_counter()
    # Utiliza 'cityblock' (diferença absoluta) para espelhar a lógica de fabsf() do C.
    alignment = dtw(sig_a, sig_b, dist_method='cityblock')
    end_py = time.perf_counter()
    
    custo_py = alignment.distance
    passos_py = len(alignment.index1)
    tempo_py_us = (end_py - start_py) * 1_000_000 # Conversão para microsegundos

    # ------------------------------------
    # Execução do modelo em teste (Binário em C) via PIPES do Sistema Operacional
    # ------------------------------------
    try:
        # Injeta o 'cenario' via stdin e captura o stdout gerado pelo printf do main.c
        result = subprocess.run([EXECUTAVEL_C], input=f"{cenario}\n", text=True, capture_output=True, check=True)
        
        # Expressões regulares (Regex) para garimpar os dados cruciais da string formatada
        match_custo = re.search(r"Distancia Escalar DTW Final:\s*([0-9.]+)", result.stdout)
        match_passos = re.search(r"Passos no Caminho Otimo:\s*([0-9]+)", result.stdout)
        match_tempo = re.search(r"Tempo de Execucao \(C\):\s*([0-9.]+)", result.stdout)
        
        custo_c = float(match_custo.group(1))
        passos_c = int(match_passos.group(1))
        tempo_c_us = float(match_tempo.group(1)) * 1_000_000 
            
    except Exception as e:
        print(f"\n[ERRO FATAL] Falha ao executar o C no cenario {cenario}. Verifique se o executavel {EXECUTAVEL_C} foi compilado via 'make'.")
        sys.exit(1)

    # ------------------------------------
    # Validação Restrita de Paridade
    # ------------------------------------
    # Compara a distância escalar com tolerância mínima para floats e o caminho com tolerância ZERO.
    if (abs(custo_py - custo_c) < 1e-8) and (passos_py == passos_c):
        paridade = "✅ Valores Exatos"
    else:
        paridade = "❌ Divergência Encontrada"

    # Montagem da linha da tabela estruturada
    linha = f"[{cenario:02d}] | {categorizar(cenario):<22} | {custo_py:<12.8f} | {custo_c:<11.8f} | {passos_py:<4d}   | {passos_c:<4d}       | {tempo_py_us:>8.1f} us | {tempo_c_us:>8.1f} us  | {paridade}"
    
    print(linha)
    relatorio.append(linha + "\n")

# Fechamento e exportação
relatorio.append("="*135 + "\nTESTE E BENCHMARKING CONCLUIDOS COM SUCESSO.\n")

with open(ARQUIVO_SAIDA, "w", encoding="utf-8") as file:
    file.writelines(relatorio)

print(linha_divisoria)
print(f"✅ Relatório completo salvo no arquivo: {ARQUIVO_SAIDA}")