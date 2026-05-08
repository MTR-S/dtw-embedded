/**
 * @file dtw.h
 * @brief Interface do módulo Dynamic Time Warping (DTW) com Backtracking.
 *
 * @details Como usar (help): A aplicação principal deve incluir este cabeçalho e
 * invocar dtw_compute() passando os sinais de entrada e os ponteiros para armazenamento
 * do caminho de backtracking. O algoritmo preencherá o caminho percorrido e seu tamanho.
 * * Contexto do desenvolvimento: Trabalho da disciplina Sistemas Embarcados/T2 (Migração).
 * * Entrada: O algoritmo DTW recebe os dois sinais a serem comparados em forma de vetor
 * de inteiros de 16 bits (const uint16_t *signal_a e const uint16_t *signal_b).
 * * Saída: O módulo possui duas vias de saída:
 * 1. Retorno principal (uint16_t): A distância escalar mínima acumulada (custo DTW).
 * 2. Parâmetros por referência: O caminho ótimo de alinhamento (backtracking), retornado
 * através dos ponteiros (dtw_path_point_t *path_out) e seu tamanho real (int *path_length).
 * * Plataforma Alvo: Placa Nucleo STM32F030R8 (ARM Cortex-M0).
 *
 * @note AJUSTE DE MIGRAÇÃO: Para contornar a limitação de 8KB de RAM da MCU,
 * os dados foram refatorados de 'float' para 'uint16_t', e as coordenadas
 * do caminho foram otimizadas de 'int' para 'uint8_t'.
 *
 * @author Matheus de Sousa Almeida e Vinicius Silva Pereira
 * @date 08 de Maio de 2026
 * @copyright Permissões de uso: Uso acadêmico.
 */

#ifndef DTW_H
#define DTW_H

#include <stdint.h>

/** * @def DTW_SIGNAL_SIZE
 * @brief Tamanho fixo dos vetores de entrada (sinais).
 */
#define DTW_SIGNAL_SIZE 45 /**< Dimensão padrão dos sinais amostrados para o cálculo DTW. */

/** * @def DTW_MAX_PATH_LEN
 * @brief Tamanho máximo possível do caminho de alinhamento (backtracking).
 */
#define DTW_MAX_PATH_LEN ((DTW_SIGNAL_SIZE * 2) - 1) /**< Pior caso de deslocamento na matriz: N + M - 1. */

/**
 * @struct dtw_path_point_t
 * @brief Coordenadas que representam um ponto na matriz de custo durante o backtracking.
 * @details Uso de uint8_t reduz o consumo de RAM no vetor de caminho em 75%.
 */
typedef struct {
    uint8_t x; /**< Coordenada no eixo X (índice do signal_a) na matriz de custo. */
    uint8_t y; /**< Coordenada no eixo Y (índice do signal_b) na matriz de custo. */
} dtw_path_point_t;

/**
 * @brief Calcula a distância DTW entre dois sinais e extrai o caminho ótimo de alinhamento.
 *
 * @param signal_a Ponteiro constante para o vetor do primeiro sinal de entrada.
 * @param signal_b Ponteiro constante para o vetor do segundo sinal de entrada.
 * @param path_out Ponteiro para o vetor pré-alocado que armazenará as coordenadas do caminho.
 * @param path_length Ponteiro para a variável que armazenará a quantidade de passos do caminho.
 *
 * @return uint16_t Distância mínima escalar acumulada entre os dois sinais.
 *
 * @note OBRIGATÓRIO: Modifica a variável estática matriz_custo internamente.
 */
uint16_t dtw_compute(const uint16_t *signal_a, const uint16_t *signal_b, dtw_path_point_t *path_out, int *path_length);

#endif /* DTW_H */
