# Projeto DTW (Dynamic Time Warping) - Sistemas Embarcados

Este repositório contém a implementação rigorosa em linguagem C do algoritmo **Dynamic Time Warping (DTW)** com suporte a *Backtracking*, desenvolvido como parte da avaliação "T1 Workstation" para a disciplina de Sistemas Embarcados.

O foco principal desta base de código é a legibilidade, previsibilidade de memória (zero alocação dinâmica), aderência irrestrita aos padrões de documentação **Doxygen** e validação contínua através de scripts de Benchmarking.

## Arquitetura e Hierarquia do Projeto

O ecossistema do projeto foi projetado para espelhar ambientes profissionais de firmware e sistemas restritos:

### 1. Módulos Core do Algoritmo
* **`dtw.h` (Interface):** Atua como o contrato público do módulo. Define os `#defines` globais, `structs` e as assinaturas. Ele blinda o usuário da complexidade interna do algoritmo.
* **`dtw.c` (Motor Algorítmico):** Abriga a lógica de processamento matemático de tabulação e o backtracking não-recursivo. Garante encapsulamento profundo usando variáveis `static` e alocação na seção `.bss`.

### 2. Infraestrutura de Validação e Teste (Testbench)
* **`main.c` (Ponto de Entrada / Testbench):** Um ambiente de testes parametrizável em C que reage a injeções de entrada (via `stdin`). Contém 20 casos de testes divididos em 5 categorias (sinais ideais, distorção, ruídos, etc.) e utiliza timers de alta resolução do S.O. (`clock_gettime`) para medições puras de performance.
* **`teste_automatizado.py` (O Orquestrador de CI):** Script de benchmarking escrito em Python. Seu papel é interagir com o `main.c`, executar o cálculo DTW usando a consagrada biblioteca do `SciPy` (Golden Reference), executar o binário C em paralelo e gerar relatórios comprovando que ambas as arquiteturas chegam matematicamente aos exatos mesmos resultados e caminhos. O resultado sai no arquivo gerado `relatorio_dtw.txt`.

### 3. Automação de Build
* **`Makefile`:** Orquestra a compilação local definindo flags de segurança restritas (`-Wall -Wextra -Wpedantic -std=c99`). Facilita a vida do desenvolvedor condensando o fluxo de build.
* **`.gitignore`:** Previne que binários gerados, metadados locais e o próprio arquivo de log `relatorio_dtw.txt` contaminem o repositório GIT.

## Como Compilar e Usar (Estação Host)

### Pré-requisitos
* Compilador GCC e `make`.
* Python 3.x com as bibliotecas `numpy` e `dtw-python` (`pip install numpy dtw-python`).

### Passo 1: Compilar o Firmware/Projeto
Abra o terminal na pasta raiz do projeto e execute:
```bash
make
```