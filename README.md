# Projeto: Dynamic Time Warping (DTW) com Backtracking - Etapa T1

## Apresentação Geral e Contexto
Este projeto faz parte da disciplina de Sistemas Embarcados (Etapa T1 - Workstation). O objetivo é implementar o algoritmo chamado de **Dynamic Time Warping (DTW)** na estação de trabalho (Host/PC) como prova de conceito, já projetado estritamente para os recursos e limitações de um sistema embarcado.

O algoritmo mede a similaridade entre duas sequências temporais sujeitas a distorções (ex: variação de velocidade) e traça o caminho ótimo de alinhamento. A arquitetura de software obedece a três regras restritas:
1. **100% Iterativo:** Nenhuma função recursiva é utilizada.
2. **Alocação Estática:** Ausência total de alocação dinâmica de memória (`malloc`/`free`).
3. **Controle de Memória:** Estrutura de dados central alocada globalmente (matriz de ~8 Kbytes em tempo de compilação).

## Autores / Estudantes
- Matheus de Sousa Almeida
- Vinicius Silva Pereira

## Plataforma Alvo
- **Fase Atual (T1 - Validação):** Estação de Trabalho Host (PC Windows/Linux/macOS) via GCC.
- **Fase Futura (T2 - Migração):** Microcontrolador (Arquitetura Embarcada).

## Entradas e Saídas
- **Entrada:** Dois vetores estáticos unidimensionais de tamanho 45 (`float sinalA[45]` e `float sinalB[45]`), simulando dados de sensores.
- **Saída:** 1. Valor de distância mínima escalar (`float`).
  2. Vetores de coordenadas X e Y detalhando o caminho do alinhamento ótimo.

## Hierarquia de Arquivos
A modularização do projeto foi feita para facilitar o reuso na Etapa T2.

```text
dtw-embedded/
├── .gitignore      # Ignora arquivos binários e gerados automaticamente.
├── Makefile        # Script de automação de compilação (gcc).
├── README.md       # Documentação principal da hierarquia e escopo.
├── dtw.h           # Interface do módulo: definições, macros e protótipos (Doxygen).
├── dtw.c           # Implementação: lógica do DTW e alocação da matriz de custo global.
└── main.c          # Ponto de entrada no PC: injeção de sinais falsos para testes.