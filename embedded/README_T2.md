#  Algoritmo DTW em Sistemas Embarcados

**Disciplina:** Sistemas Embarcados (T2)  
**Plataforma Alvo:** Nucleo STM32F030R8 
**Autores:** Matheus de Sousa Almeida e Vinicius Silva Pereira  
**Data:** Maio de 2026  

---

## Visão Geral do Projeto

Este projeto consiste na migração e otimização do algoritmo **Dynamic Time Warping (DTW)** — tradicionalmente executado em computadores com amplos recursos (float 32/64 bits) — para um microcontrolador com restrições severas de hardware (8 KB de SRAM, sem FPU nativa).

---

## Hierarquia e Arquitetura de Arquivos

O projeto adota uma separação clara entre a lógica da aplicação, a biblioteca matemática (motor DTW) e a infraestrutura de testes automatizados.

### 1. `dtw.h` (Interface e Definições de Hardware)
Cabeçalho principal que define a interface pública do algoritmo.
* **Função no Projeto:** Centralizar as macros de calibração matemática e as estruturas de dados otimizadas.
* **Destaques de Engenharia:**
  * Define o `DTW_SCALE_FACTOR 100` (Fator x100), consolidando o uso de Matemática de Ponto Fixo (2 casas decimais) em todo o sistema.
  * Implementa o rebaixamento de tipos (*Type Downgrading*) na struct `dtw_path_point_t`. Como o tamanho máximo da matriz é 45x45, as coordenadas `x` e `y` do backtracking foram reduzidas de `int` (32 bits) para `uint8_t` (8 bits), reduzindo o consumo de memória da rota em 75%.

### 2. `dtw.c` (Motor de Cálculo Otimizado)
Implementação do núcleo matemático do DTW (Tabulação Dinâmica e Backtracking).
* **Função no Projeto:** Executar o alinhamento temporal elástico sem depender de bibliotecas matemáticas pesadas ou processamento de ponto flutuante.
* **Destaques de Engenharia:**
  * **Alocação na Seção `.bss`:** A matriz de custos principal (`45x45` de `uint16_t`) consome aproximadamente 4 KB (metade da SRAM do chip). Ela foi declarada como `static` internamente na função `dtw_compute()`. Isso tira a matriz da Stack (Pilha) — prevenindo o travamento do sistema por *Stack Overflow* — e a força de forma segura na seção `.bss` da RAM.
  * **Lógica Iterativa:** Toda a Programação Dinâmica e o Backtracking foram feitos via iteração (loops `while` e `for`) para evitar o aninhamento profundo de funções que ocorreria em uma abordagem recursiva.

### 3. `main.c` (Firmware de Aplicação e Injeção de Sinais)
Arquivo principal rodando na STM32, contendo o fluxo de controle, configuração de periféricos (HAL) e os cenários de teste.
* **Função no Projeto:** Atuar como a "bancada de testes física". Ele inicializa a UART2, recarrega as variáveis iterativamente, injeta os pulsos de teste, cronometra a execução (via `SysTick`) e imprime a telemetria formatada.
* **Destaques de Engenharia:**
  * **Otimização de Flash (`.rodata`):** Os dicionários de formas de onda (Pulsos Gaussianos, Quadrados e Ruídos) foram declarados como `static const`. O compilador os aloca na Memória Flash (ROM) protegida, acessando-os sob demanda sem gastar memória SRAM para instanciá-los.
  * **Sanitização de Memória:** Implementa um apagamento sistemático dos arrays locais a cada iteração de teste para evitar corrupção por Lixo de Memória (*Garbage Values*) e Efeito Fantasma (*Data Bleed*).


---