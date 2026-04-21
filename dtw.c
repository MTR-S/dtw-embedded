/**
 * @file dtw.c
 * @brief Implementação do algoritmo Dynamic Time Warping (DTW) com Backtracking.
 */

#include "dtw.h"
#include <math.h> // Necessário para fabsf() - valor absoluto de floats (Módulo)

/* ==========================================================================
 * ESTRUTURA DE DADOS GLOBAL (Obrigatória pelos requisitos do T1)
 * Matriz de 45 x 45 floats = 2025 elementos * 4 bytes = 8100 bytes (~8.1 KB).
 * Alocada na seção .bss/.data (estática), garantindo ZERO malloc().
 * ========================================================================== */
float matriz_custo[DTW_SIGNAL_SIZE][DTW_SIGNAL_SIZE];

/**
 * @brief Função auxiliar para encontrar o menor de três valores.
 * @note Declarada como 'static' para ser visível apenas dentro deste arquivo,
 * encapsulando a lógica e não poluindo o namespace global do microcontrolador.
 */
static float min_of_3(float a, float b, float c) {
    float min = a;
    if (b < min) min = b;
    if (c < min) min = c;

    return min;
}

float dtw_compute(const float *signal_a, const float *signal_b, dtw_path_point_t *path_out, int *path_length) {
    int i, j;
    float custo_atual;

    /* ----------------------------------------------------------------------
     * FASE 1: TABULAÇÃO (Programação Dinâmica Iterativa)
     * ---------------------------------------------------------------------- */
    // 1.1 Inicializa a origem (0,0)
    matriz_custo[0][0] = fabsf(signal_a[0] - signal_b[0]);

    // 1.2 Preenche a primeira coluna (j = 0)
    for (i = 1; i < DTW_SIGNAL_SIZE; i++) {
        matriz_custo[i][0] = fabsf(signal_a[i] - signal_b[0]) + matriz_custo[i-1][0];
    }

    // 1.3 Preenche a primeira linha (i = 0)
    for (j = 1; j < DTW_SIGNAL_SIZE; j++) {
        matriz_custo[0][j] = fabsf(signal_a[0] - signal_b[j]) + matriz_custo[0][j-1];
    }

    // 1.4 Preenche o restante da matriz (Utilizando 2 laços aninhados - Exigência do algoritmo)
    for (i = 1; i < DTW_SIGNAL_SIZE; i++) {
        for (j = 1; j < DTW_SIGNAL_SIZE; j++) {
            custo_atual = fabsf(signal_a[i] - signal_b[j]);
            
            matriz_custo[i][j] = custo_atual + min_of_3(
                matriz_custo[i-1][j],   // Inserção / Movimento vertical
                matriz_custo[i][j-1],   // Deleção / Movimento horizontal
                matriz_custo[i-1][j-1]  // Match / Movimento diagonal
            );
        }
    }

    float distancia_total = matriz_custo[DTW_SIGNAL_SIZE-1][DTW_SIGNAL_SIZE-1];

    /* ----------------------------------------------------------------------
     * FASE 2: BACKTRACKING ITERATIVO (Sem Recursão)
     * ---------------------------------------------------------------------- */
    i = DTW_SIGNAL_SIZE - 1;
    j = DTW_SIGNAL_SIZE - 1;
    int step_count = 0;

    // Rastreia o caminho de (44, 44) até (0,0)
    while (i > 0 || j > 0) {
        // Salva a coordenada atual
        path_out[step_count].x = i;
        path_out[step_count].y = j;
        step_count++;

        if (i == 0) {
            j--; // Só pode mover para a esquerda
        } else if (j == 0) {
            i--; // Só pode mover para cima
        } else {
            // Avalia os três vizinhos anteriores
            float diag = matriz_custo[i-1][j-1];
            float cima = matriz_custo[i-1][j];
            float esq  = matriz_custo[i][j-1];

            if (diag <= cima && diag <= esq) {
                i--; j--;
            } else if (cima <= diag && cima <= esq) {
                i--;
            } else {
                j--;
            }
        }
    }

    // Adiciona a origem (0,0) ao final do caminho rastreado
    path_out[step_count].x = 0;
    path_out[step_count].y = 0;
    step_count++;

    *path_length = step_count;

    return distancia_total;
}