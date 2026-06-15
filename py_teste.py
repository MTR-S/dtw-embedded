"""
Script de Validação e Benchmarking do DTW (Apenas Python).

Este script atua como gerador do "Gabarito de Ouro" matemático.
Ele executa exclusivamente a implementação de referência (SciPy via dtw-python) 
no PC para gerar os custos ideais e o número de passos para os 20 cenários.

Autor: Matheus de Sousa Almeida e Vinicius Silva Pereira
Data: Maio de 2026
"""

import numpy as np
from dtw import dtw
import time

# ==============================================================================
# CONFIGURAÇÕES DO AMBIENTE
# ==============================================================================
ARQUIVO_SAIDA = "relatorio_dtw_python.txt"

def gerar_sinais(cenario: int) -> tuple[np.ndarray, np.ndarray]:
    """Gera o par de sinais sintéticos exatos para o teste."""
    sig_a, sig_b = np.zeros(45), np.zeros(45)
    
    pulso_gaussiano = np.array([0.2, 0.6, 1.5, 2.8, 3.7, 4.0, 3.7, 2.8, 1.5, 0.6, 0.2])
    pulso_quadrado  = np.array([1.5, 3.2, 3.2, 3.2, 3.2, 1.5])
    ruido_forte     = np.array([3.8, 4.5, 2.9])
    
    if cenario == 0: sig_a[10:21] = pulso_gaussiano; sig_b[10:21] = pulso_gaussiano
    elif cenario == 1: sig_a[15:21] = pulso_quadrado; sig_b[15:21] = pulso_quadrado
    elif cenario == 2: 
        for i in range(5, 41): sig_a[i] = sig_b[i] = 0.5 + (i % 5) * 0.6
    elif cenario == 3: 
        sig_a[10:16] = pulso_quadrado; sig_b[10:16] = pulso_quadrado
        sig_a[30:36] = pulso_quadrado; sig_b[30:36] = pulso_quadrado
    elif cenario == 4: sig_a[10:21] = pulso_gaussiano; sig_b[15:26] = pulso_gaussiano
    elif cenario == 5: sig_a[20:31] = pulso_gaussiano; sig_b[10:21] = pulso_gaussiano
    elif cenario == 6: sig_a[5:16] = pulso_gaussiano; sig_b[30:41] = pulso_gaussiano
    elif cenario == 7: sig_a[30:41] = pulso_gaussiano; sig_b[5:16] = pulso_gaussiano
    elif cenario == 8: 
        sig_a[10:16] = pulso_quadrado; sig_b[10:26] = 3.2; sig_b[9] = 1.5; sig_b[26] = 1.5
    elif cenario == 9: 
        sig_a[10:26] = 3.2; sig_a[9] = 1.5; sig_a[26] = 1.5; sig_b[10:16] = pulso_quadrado
    elif cenario == 10: 
        sig_a[5:16] = pulso_gaussiano
        for i in range(5, 36): sig_b[i] = 0.5 + (i - 5) * 0.11
    elif cenario == 11: 
        for i in range(5, 36): sig_a[i] = 0.5 + (i - 5) * 0.11
        sig_b[5:16] = pulso_gaussiano
    elif cenario == 12: 
        sig_a[10:21] = pulso_gaussiano; sig_b[10:21] = pulso_gaussiano; sig_b[2:5] = ruido_forte
    elif cenario == 13: 
        sig_a[10:21] = pulso_gaussiano; sig_b[10:21] = pulso_gaussiano; sig_b[40:43] = ruido_forte
    elif cenario == 14: 
        sig_a[10:21] = pulso_gaussiano; sig_b[5:8] = ruido_forte; sig_b[35:38] = ruido_forte
    elif cenario == 15: 
        sig_a[10:21] = pulso_gaussiano; sig_b[:] = 2.0
        for i in range(11): sig_b[10 + i] = 4.0 - pulso_gaussiano[i]
    elif cenario == 16: pass
    elif cenario == 17: sig_a[:] = 5.0; sig_b[:] = 5.0
    elif cenario == 18: sig_a[10:21] = pulso_gaussiano
    elif cenario == 19: sig_b[10:21] = pulso_gaussiano

    return sig_a, sig_b

def categorizar(c: int) -> str:
    if c < 4: return "1. Sinais Iguais"
    if c < 8: return "2. Atraso Temporal"
    if c < 12: return "3. Distorcao (Warping)"
    if c < 16: return "4. Ruidos/Anomalias"
    return "5. Extremos de Sensor"

# ==============================================================================
# INICIALIZAÇÃO DA INTERFACE VISUAL DA TABELA
# ==============================================================================
relatorio = []
linha_divisoria = "-" * 95

cabecalho = f"{'ID':<3} | {'CATEGORIA':<22} | {'DISTÂNCIA ESCALAR':<18} | {'PASSOS ROTA':<12} | {'TEMPO DE EXECUÇÃO (us)':<25}\n"

texto_inicio = "="*95 + "\nRELATORIO DE VALIDACAO (APENAS PYTHON SciPy)\n" + "="*95 + "\n"
relatorio.extend([texto_inicio, cabecalho, linha_divisoria + "\n"])

print(texto_inicio, end="")
print(cabecalho, end="")
print(linha_divisoria)

# ==============================================================================
# EXECUÇÃO DO GABARITO (PYTHON)
# ==============================================================================
for cenario in range(20):
    sig_a, sig_b = gerar_sinais(cenario)
    
    # --- Roda o Python ---
    start_py = time.perf_counter()
    alignment = dtw(sig_a, sig_b, dist_method='cityblock', step_pattern='symmetric1')
    end_py = time.perf_counter()
    
    py_custo = alignment.distance
    py_passos = len(alignment.index1)
    py_tempo_us = (end_py - start_py) * 1_000_000

    # Montagem da Linha da Tabela
    linha_tabela = f"[{cenario:02d}] | {categorizar(cenario):<22} | {py_custo:<18.2f} | {py_passos:<12d} | {py_tempo_us:>15.1f} us"
    
    print(linha_tabela)
    relatorio.append(linha_tabela + "\n")

# Fechamento e exportação
relatorio.append("="*95 + "\nTESTE CONCLUIDO COM SUCESSO.\n")

with open(ARQUIVO_SAIDA, "w", encoding="utf-8") as file:
    file.writelines(relatorio)

print(linha_divisoria)
print(f"✅ Relatório salvo em: {ARQUIVO_SAIDA}")