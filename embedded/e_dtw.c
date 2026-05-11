/**
 * @file dtw.c
 * @brief Implementação do módulo DTW otimizado para MCU com Matemática de Ponto Fixo.
 *
 * @details Como usar (help): Este arquivo contém a lógica de tabulação iterativa e o
 * backtracking não-recursivo do DTW. Compile junto com o arquivo principal da aplicação.
 * * * Contexto do desenvolvimento: Trabalho da disciplina Sistemas Embarcados/T2.
 * * Entrada: O algoritmo DTW recebe os dois sinais a serem comparados em forma de vetor
 * de inteiros de 16 bits.
 * @note ARQUITETURA DE PONTO FIXO: Os vetores (const uint16_t *signal_a e *signal_b)
 * não são inteiros puros, mas sim representações decimais escalonadas por um fator
 * multiplicador (ex: x1000 para 3 casas decimais). O valor 1000 representa 1.000.
 * * * Saída: O módulo possui duas vias de saída:
 * 1. Retorno principal (uint16_t): A distância escalar mínima acumulada em Ponto Fixo.
 * 2. Parâmetros por referência: O caminho ótimo de alinhamento (dtw_path_point_t *path_out)
 * e seu tamanho real (int *path_length).
 * * Plataforma Alvo: Nucleo STM32F030R8 (ARM Cortex-M0).
 *
 * @author Matheus de Sousa Almeida e Vinicius Silva Pereira
 * @date 11 de Maio de 2026
 * @copyright Permissões de uso: Uso acadêmico.
 */

#include "dtw.h"
#include <stdlib.h> /**< Necessário para abs() - valor absoluto de inteiros. */

/* ==========================================================================
 * ESTRUTURA DE DADOS E FUNÇÕES PRIVADAS
 * ========================================================================== */

/**
 * @brief Encontra o menor valor entre três números inteiros em Ponto Fixo.
 * @param a Primeiro valor de entrada (movimento vertical/inserção).
 * @param b Segundo valor de entrada (movimento horizontal/deleção).
 * @param c Terceiro valor de entrada (movimento diagonal/match).
 * @return uint16_t O menor valor encontrado entre a, b e c.
 */
static uint16_t min_of_3(uint16_t a, uint16_t b, uint16_t c) {
    uint16_t min = a; /**< Variável auxiliar para rastrear o menor valor. */

    if (b < min) min = b;
    if (c < min) min = c;

    return min;
}

/* ==========================================================================
 * IMPLEMENTAÇÃO DAS FUNÇÕES PÚBLICAS
 * ========================================================================== */

uint16_t dtw_compute(const uint16_t *signal_a, const uint16_t *signal_b, dtw_path_point_t *path_out, int *path_length) {
    int i; /**< Iterador para o eixo X (linhas da matriz / sinal A). */
    int j; /**< Iterador para o eixo Y (colunas da matriz / sinal B). */
    uint16_t custo_atual; /**< Armazena a distância euclidiana instantânea em Ponto Fixo. */

    /** * @note AJUSTE DE MIGRAÇÃO: Matriz de tabulação (Aprox. 3.9 KB).
     * Alocada internamente como 'static' para ser colocada na seção .bss da RAM,
     * evitando o uso da variável global do T1 e prevenindo Stack Overflow (Hardware Fault).
     * Armazena valores acumulados em Ponto Fixo.
     */
    static uint16_t matriz_custo[DTW_SIGNAL_SIZE][DTW_SIGNAL_SIZE];

    /* ----------------------------------------------------------------------
     * FASE 1: TABULAÇÃO (Programação Dinâmica Iterativa)
     * O uso de iteração previne stack overflow, comum em microcontroladores.
     * ---------------------------------------------------------------------- */

    // Inicializa a origem (0,0) com a diferença absoluta inicial.
    matriz_custo[0][0] = abs((int)signal_a[0] - (int)signal_b[0]);

    // Preenche a primeira coluna acumulando o custo anterior (j=0 fixo).
    for (i = 1; i < DTW_SIGNAL_SIZE; i++) {
        matriz_custo[i][0] = abs((int)signal_a[i] - (int)signal_b[0]) + matriz_custo[i-1][0];
    }

    // Preenche a primeira linha acumulando o custo anterior (i=0 fixo).
    for (j = 1; j < DTW_SIGNAL_SIZE; j++) {
        matriz_custo[0][j] = abs((int)signal_a[0] - (int)signal_b[j]) + matriz_custo[0][j-1];
    }

    // Preenche o "miolo" da matriz. Complexidade O(N*M).
    for (i = 1; i < DTW_SIGNAL_SIZE; i++) {
        for (j = 1; j < DTW_SIGNAL_SIZE; j++) {
            custo_atual = abs((int)signal_a[i] - (int)signal_b[j]);

            matriz_custo[i][j] = custo_atual + min_of_3(
                matriz_custo[i-1][j],   // Custo do Movimento vertical
                matriz_custo[i][j-1],   // Custo do Movimento horizontal
                matriz_custo[i-1][j-1]  // Custo do Movimento diagonal
            );
        }
    }

    uint16_t distancia_total = matriz_custo[DTW_SIGNAL_SIZE-1][DTW_SIGNAL_SIZE-1];

    /* ----------------------------------------------------------------------
     * FASE 2: BACKTRACKING ITERATIVO (Sem Recursão)
     * ---------------------------------------------------------------------- */
    i = DTW_SIGNAL_SIZE - 1;
    j = DTW_SIGNAL_SIZE - 1;
    int step_count = 0;

    while (i > 0 || j > 0) {
        // Registra o nó atual como parte do caminho ótimo.
        path_out[step_count].x = (uint8_t)i; // Cast explícito para uint8_t
        path_out[step_count].y = (uint8_t)j;
        step_count++;

        if (i == 0) {
            j--;
        } else if (j == 0) {
            i--;
        } else {
            uint16_t diag = matriz_custo[i-1][j-1];
            uint16_t cima = matriz_custo[i-1][j];
            uint16_t esq  = matriz_custo[i][j-1];

            if (diag <= cima && diag <= esq) {
                i--; j--;
            } else if (cima <= diag && cima <= esq) {
                i--;
            } else {
                j--;
            }
        }
    }

    // Salva a coordenada da origem.
    path_out[step_count].x = 0;
    path_out[step_count].y = 0;
    step_count++;

    *path_length = step_count;

    return distancia_total;
}
