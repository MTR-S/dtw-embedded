/**
 * @file dtw.h
 * @brief Interface do módulo Dynamic Time Warping (DTW) com Backtracking.
 * * @details Este módulo implementa o algoritmo DTW otimizado para sistemas embarcados.
 * Garante ausência total de alocação dinâmica (zero malloc/free) e recursividade.
 * Utiliza estruturas de dados alocadas estaticamente em tempo de compilação.
 * * @author Matheus de Sousa Almeida e Vinicius Silva Pereira
 * @date Abril 2026
 * @copyright Uso educacional - Disciplina de Sistemas Embarcados (Etapa T1)
 * @note Plataforma alvo: Host (PC) via GCC na Etapa T1; Microcontrolador na Etapa T2.
 */

#ifndef DTW_H
#define DTW_H

/** * @def DTW_SIGNAL_SIZE
 * @brief Tamanho fixo dos vetores de entrada (sinais).
 * @note A matriz de custo terá dimensão [45][45] = 2025 elementos floats (~8.1 KB),
 * cumprindo rigorosamente o requisito do algoritmo.
 */
#define DTW_SIGNAL_SIZE 45

/** * @def DTW_MAX_PATH_LEN
 * @brief Tamanho máximo possível do caminho de alinhamento (backtracking).
 * Matematicamente, o pior caso de caminho em uma matriz NxM é N + M - 1.
 */
#define DTW_MAX_PATH_LEN ((DTW_SIGNAL_SIZE * 2) - 1)

/**
 * @struct dtw_path_point_t
 * @brief Coordenadas (x, y) que representam um ponto na matriz de custo durante o backtracking.
 */
typedef struct {
    int x;
    int y;
} dtw_path_point_t;

/**
 * @brief Calcula a distância DTW entre dois sinais e extrai o caminho ótimo de alinhamento.
 * * @param[in]  signal_a    Ponteiro para o vetor do primeiro sinal (tamanho DTW_SIGNAL_SIZE).
 * @param[in]  signal_b    Ponteiro para o vetor do segundo sinal (tamanho DTW_SIGNAL_SIZE).
 * @param[out] path_out    Vetor pré-alocado estaticamente para armazenar a rota do backtracking.
 * @param[out] path_length Ponteiro para armazenar o número real de passos dados no caminho ótimo.
 * * @return float A distância mínima escalar acumulada entre os dois sinais.
 * * @note Variáveis globais afetadas: matriz_custo (declarada internamente no dtw.c).
 */
float dtw_compute(const float *signal_a, const float *signal_b, dtw_path_point_t *path_out, int *path_length);

#endif /* DTW_H */