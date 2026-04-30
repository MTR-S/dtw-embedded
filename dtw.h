/**
 * @file dtw.h
 * @brief Interface do módulo Dynamic Time Warping (DTW) com Backtracking.
 *
 * @details Como usar (help): A aplicação principal deve incluir este cabeçalho e 
 * invocar dtw_compute() passando os sinais de entrada e os ponteiros para armazenamento 
 * do caminho de backtracking. O algoritmo preencherá o caminho percorrido e seu tamanho.
 * 
 * Contexto do desenvolvimento: Trabalho da disciplina Sistemas Embarcados/T1 Workstation.
 * 
 * Entrada: O algoritmo DTW recebe os dois sinais a serem comparados em forma de vetor 
 * (const float *signal_a e const float *signal_b).
 * 
 * Saída: O módulo possui duas vias de saída:
 * 1. Retorno principal (float): A distância escalar mínima acumulada (custo DTW) entre os sinais.
 * 2. Parâmetros por referência: O caminho ótimo de alinhamento (backtracking), retornado 
 *    através dos ponteiros (dtw_path_point_t *path_out) e seu tamanho real (int *path_length).
 * 
 * Plataforma Alvo: Estação de Trabalho Host (PC Windows/Linux/macOS).
 *
 * @author Matheus de Sousa Almeida e Vinicius Silva Pereira
 * @date 30 de Abril de 2026
 * @copyright Permissões de uso: Uso acadêmico.
 */

#ifndef DTW_H
#define DTW_H

/** 
 * @def DTW_SIGNAL_SIZE
 * @brief Tamanho fixo dos vetores de entrada (sinais).
 */
#define DTW_SIGNAL_SIZE 45 /**< Dimensão padrão dos sinais amostrados para o cálculo DTW. */

/** 
 * @def DTW_MAX_PATH_LEN
 * @brief Tamanho máximo possível do caminho de alinhamento (backtracking) - Definimos inicialmente por causa da restrição de alocação dinâmica.
 */
#define DTW_MAX_PATH_LEN ((DTW_SIGNAL_SIZE * 2) - 1) /**< Pior caso de deslocamento na matriz: N + M - 1. */

/**
 * @struct dtw_path_point_t
 * @brief Coordenadas que representam um ponto na matriz de custo durante o backtracking.
 */
typedef struct {
    int x; /**< Coordenada no eixo X (índice do signal_a) na matriz de custo. */
    int y; /**< Coordenada no eixo Y (índice do signal_b) na matriz de custo. */
} dtw_path_point_t;

/**
 * @brief Calcula a distância DTW entre dois sinais e extrai o caminho ótimo de alinhamento.
 *
 * @param signal_a Ponteiro constante para o vetor do primeiro sinal de entrada.
 * @param signal_b Ponteiro constante para o vetor do segundo sinal de entrada.
 * @param path_out Ponteiro para o vetor pré-alocado que armazenará as coordenadas do caminho (backtracking).
 * @param path_length Ponteiro para a variável que armazenará a quantidade de passos do caminho.
 *
 * @return float Distância mínima escalar acumulada entre os dois sinais.
 *
 * @note OBRIGATÓRIO: Lê e modifica a variável global matriz_custo.
 */
float dtw_compute(const float *signal_a, const float *signal_b, dtw_path_point_t *path_out, int *path_length);

#endif /* DTW_H */