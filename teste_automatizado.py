"""
Script de Validação Tripla (MIL + HIL) e Benchmarking do DTW.

Este script atua como o orquestrador Mestre. Ele executa 3 vertentes simultâneas:
1. Python (SciPy): O "Gabarito de Ouro" matemático.
2. C (PC Host): O modelo em C de referência rodando com recursos infinitos (Float 32-bit).
3. C (STM32 HIL): O modelo físico em Ponto Fixo (16-bit) via UART (Hardware-in-the-Loop).

Autor: Matheus de Sousa Almeida e Vinicius Silva Pereira
Data: Maio de 2026
"""

import numpy as np
from dtw import dtw
import serial
import time
import re
import sys
import subprocess

# ==============================================================================
# CONFIGURAÇÕES DO AMBIENTE
# ==============================================================================
PORTA_SERIAL = "COM6"  # Troque para a porta correta (ex: COM4, /dev/ttyS3 no WSL)
BAUD_RATE = 115200
ARQUIVO_SAIDA = "relatorio_dtw_triplo.txt"
EXECUTAVEL_C = "./dtw_app.exe"  # No Windows, pode ser "./dtw_app.exe"

def gerar_sinais(cenario: int) -> tuple[np.ndarray, np.ndarray]:
    """Gera o par de sinais sintéticos exatos usando os fatores da placa."""
    sig_a, sig_b = np.zeros(45), np.zeros(45)
    
    # Padrões originais sem escala (o Python e o C do PC cuidam da matemática float)
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
# INICIALIZAÇÃO DA INTERFACE VISUAL DA TABELA TRIPLA
# ==============================================================================
relatorio = []
linha_divisoria = "-" * 155

# Tabela alargada para comportar as 3 fontes de dados
cabecalho = f"{'ID':<3} | {'CATEGORIA':<22} | {'DISTÂNCIA ESCALAR DTW':<30} | {'PASSOS ROTA':<18} | {'TEMPOS DE EXECUÇÃO':<32} | {'PARIDADE'}\n"
sub_cabecalho = f"{'':<3} | {'':<22} | {'Python':<9}| {'C (PC)':<9}| {'STM32':<8} | {'Py':<5}| {'C_PC':<5}| {'STM32':<5}| {'Python (us)':<11}| {'C PC (us)':<10}| {'STM32 (ms)':<8} | \n"

texto_inicio = "="*155 + "\nRELATORIO TRIPLO DE VALIDACAO (PYTHON SciPy vs C HOST vs STM32 HIL)\n" + "="*155 + "\n"
relatorio.extend([texto_inicio, cabecalho, sub_cabecalho, linha_divisoria + "\n"])

print(texto_inicio, end="")
print(cabecalho, end="")
print(sub_cabecalho, end="")
print(linha_divisoria)

# ==============================================================================
# FASE 1: PRÉ-PROCESSAMENTO (GABARITOS DO PYTHON E C NO PC)
# ==============================================================================
print(f"[*] Calculando gabaritos no Host (Python e C-PC)... aguarde.")
gabarito_host = []

for cenario in range(20):
    sig_a, sig_b = gerar_sinais(cenario)
    
    # --- 1. Roda o Python ---
    start_py = time.perf_counter()
    alignment = dtw(sig_a, sig_b, dist_method='cityblock')
    end_py = time.perf_counter()
    
    py_custo = alignment.distance
    py_passos = len(alignment.index1)
    py_tempo_us = (end_py - start_py) * 1_000_000

    # --- 2. Roda o C (Binário Local no PC) ---
    c_custo, c_passos, c_tempo_us = 0.0, 0, 0.0
    try:
        result = subprocess.run([EXECUTAVEL_C], input=f"{cenario}\n", text=True, capture_output=True, check=True)
        match_custo = re.search(r"Distancia Escalar DTW Final:\s*([0-9.]+)", result.stdout)
        match_passos = re.search(r"Passos no Caminho Otimo:\s*([0-9]+)", result.stdout)
        match_tempo = re.search(r"Tempo de Execucao \(C\):\s*([0-9.]+)", result.stdout)
        
        c_custo = float(match_custo.group(1)) if match_custo else 0.0
        c_passos = int(match_passos.group(1)) if match_passos else 0
        c_tempo_us = (float(match_tempo.group(1)) * 1_000_000) if match_tempo else 0.0
    except FileNotFoundError:
        print(f"\n[AVISO] Executável C ({EXECUTAVEL_C}) nao encontrado no PC. Os dados do C(PC) ficarao zerados.")
    except Exception as e:
        print(f"\n[ERRO] Falha ao rodar o C no PC para o cenario {cenario}: {e}")

    # Salva os resultados do Host para comparar depois com a Placa
    gabarito_host.append({
        "py_custo": py_custo, "py_passos": py_passos, "py_tempo_us": py_tempo_us,
        "c_custo": c_custo, "c_passos": c_passos, "c_tempo_us": c_tempo_us
    })

# ==============================================================================
# FASE 2: SINCRONIZAÇÃO E CAPTURA UART COM A PLACA STM32 (HIL)
# ==============================================================================
print(f"[*] Gabaritos do Host concluidos! Aguardando conexao com a placa na porta {PORTA_SERIAL}...")

try:
    with serial.Serial(PORTA_SERIAL, BAUD_RATE, timeout=15) as ser:
        print("[*] Conexao serial estabelecida! Pressione o botao RESET (Preto) na Nucleo STM32.")
        
        # Sincroniza esperando o cabeçalho da tabela da placa
        while True:
            linha_raw = ser.readline()
            if not linha_raw: continue
            linha_str = linha_raw.decode('utf-8', errors='ignore').strip()
            if "ID | CUSTO" in linha_str:
                ser.readline() # Ignora a linha de "----------"
                break

        # Inicia a captura das 20 linhas de dados da STM32
        for cenario in range(20):
            linha_dados = ser.readline().decode('utf-8', errors='ignore').strip()
            
            # Lê o printf C da Placa: "[00]|      0.00   | 45          | 8 ms"
            match = re.search(r"\[(\d+)\]\|\s*([\d.]+)\s*\|\s*(\d+)\s*\|\s*(\d+)\s*ms", linha_dados)
            
            if not match:
                print(f"[ERRO] Falha ao ler dados da UART: {linha_dados}")
                continue
                
            stm_custo = float(match.group(2))
            stm_passos = int(match.group(3))
            stm_tempo_ms = int(match.group(4))
            
            # Puxa os dados que já calculamos no PC
            ref = gabarito_host[cenario]
            
            # Validação Tripla: O C do PC deve ser quase idêntico ao Python, 
            # e a STM32 deve ter tolerância de 0.05 pelo Ponto Fixo (2 casas).
            paridade = "✅ Passou"
            if abs(ref['py_custo'] - ref['c_custo']) > 1e-4: paridade = "❌ Falhou (PC)"
            if abs(ref['py_custo'] - stm_custo) > 0.05: paridade = "❌ Falhou (STM32)"
            if not (ref['py_passos'] == ref['c_passos'] == stm_passos): paridade = "❌ Falhou (Rotas)"

            # Montagem da Super Linha da Tabela
            linha_tabela = f"[{cenario:02d}] | {categorizar(cenario):<22} | {ref['py_custo']:<9.2f}| {ref['c_custo']:<9.2f}| {stm_custo:<8.2f} | {ref['py_passos']:<5d}| {ref['c_passos']:<5d}| {stm_passos:<5d} | {ref['py_tempo_us']:>8.1f} us| {ref['c_tempo_us']:>7.1f} us| {stm_tempo_ms:>5d} ms | {paridade}"
            
            print(linha_tabela)
            relatorio.append(linha_tabela + "\n")

except serial.SerialException as e:
    print(f"\n[ERRO FATAL] Nao foi possivel abrir a porta {PORTA_SERIAL}.")
    print("Verifique se o TeraTerm/PuTTY esta FECHADO e a porta esta correta.")
    sys.exit(1)

# Fechamento e exportação
relatorio.append("="*155 + "\nTESTE TRIPLO MIL+HIL CONCLUIDO COM SUCESSO.\n")

with open(ARQUIVO_SAIDA, "w", encoding="utf-8") as file:
    file.writelines(relatorio)

print(linha_divisoria)
print(f"✅ Relatório Triplo completo salvo em: {ARQUIVO_SAIDA}")