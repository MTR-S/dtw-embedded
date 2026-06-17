/**
 * @file dtw.c
 * @brief Implementação do módulo Dynamic Time Warping (DTW) com Banda de Sakoe-Chiba e Backtracking.
 *
 * @details Como usar (help): Este arquivo contém a lógica de tabulação iterativa restrita
 * por janela temporal e o backtracking não-recursivo do DTW. Compile junto com o arquivo 
 * principal da aplicação.
 * * Contexto do desenvolvimento: Trabalho da disciplina Sistemas Embarcados/T1 Workstation.
 * * Entrada: O algoritmo DTW recebe os dois sinais a serem comparados em forma de vetor 
 * (const float *signal_a e const float *signal_b).
 * * Saída: O módulo possui duas vias de saída:
 * 1. Retorno principal (float): A distância escalar mínima acumulada (custo DTW) dentro da banda.
 * 2. Parâmetros por referência: O caminho ótimo de alinhamento (backtracking), retornado 
 * através dos ponteiros (dtw_path_point_t *path_out) e seu tamanho real (int *path_length).
 * * Plataforma Alvo: Estação de Trabalho Host (PC Windows/Linux/macOS).
 *
 * @author Matheus de Sousa Almeida e Vinicius Silva Pereira
 * @date 30 de Abril de 2026
 * @copyright Permissões de uso: Uso acadêmico.
 */

#include "dtw.h"
#include <math.h> /**< Necessário para fabsf() e INFINITY. */

/* ==========================================================================
 * MACROS AUXILIARES DE FRONTEIRA
 * ========================================================================== */
#define MAX_INT(a, b) ((a) > (b) ? (a) : (b))
#define MIN_INT(a, b) ((a) < (b) ? (a) : (b))

/* ==========================================================================
 * ESTRUTURA DE DADOS GLOBAL 
 * ========================================================================== */

/** * @brief Matriz de tabulação para a Programação Dinâmica. 
 * Alocada estaticamente na seção .bss para evitar overhead e fragmentação de heap (sem malloc - restrição do T1).
 */
float matriz_custo[DTW_SIGNAL_SIZE][DTW_SIGNAL_SIZE]; /**< Matriz de 45x45 floats (2025 floats - Aprox. 8.1 KB). */

/**
 * @brief Encontra o menor valor entre três números de ponto flutuante.
 * * @param a Primeiro valor de entrada (movimento vertical/inserção).
 * @param b Segundo valor de entrada (movimento horizontal/deleção).
 * @param c Terceiro valor de entrada (movimento diagonal/match).
 * * @return float O menor valor encontrado entre a, b e c.
 * * @note OBRIGATÓRIO: Nenhuma variável global afetada.
 */
static float min_of_3(float a, float b, float c) {
    float min = a; /**< Variável auxiliar para rastrear o menor valor em tempo real de execução. */

    if (b < min) min = b;
    if (c < min) min = c;
    
    return min;
}

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
float dtw_compute(const float *signal_a, const float *signal_b, dtw_path_point_t *path_out, int *path_length) {
    int i; /**< Iterador para o eixo X (linhas da matriz / sinal A). */
    int j; /**< Iterador para o eixo Y (colunas da matriz / sinal B). */
    float custo_atual; /**< Armazena a distância euclidiana instantânea entre dois pontos dos sinais. */

    /* ----------------------------------------------------------------------
     * FASE 1: TABULAÇÃO (Programação Dinâmica com Banda de Sakoe-Chiba)
     * O uso de uma janela restrita reduz a complexidade de cálculo na matriz.
     * ---------------------------------------------------------------------- */
     
    // 1. Inicializa toda a matriz de custo com INFINITO.
    // Isso garante que qualquer tentativa de indexação pelo backtracking fora da banda 
    // seja descartada por ter peso proibitivo.
    for (i = 0; i < DTW_SIGNAL_SIZE; i++) {
        for (j = 0; j < DTW_SIGNAL_SIZE; j++) {
            matriz_custo[i][j] = INFINITY;
        }
    }

    // Inicializa a origem (0,0) com a diferença absoluta inicial.
    matriz_custo[0][0] = fabsf(signal_a[0] - signal_b[0]);

    // 2. Preenchimento iterativo limitado pelas fronteiras da janela dinâmica
    for (i = 0; i < DTW_SIGNAL_SIZE; i++) {
        // Restringe os limites de J baseando-se no valor atual de I e no tamanho do canal de busca (DTW_WINDOW_SIZE)
        int j_start = MAX_INT(0, i - DTW_WINDOW_SIZE);
        int j_end   = MIN_INT(DTW_SIGNAL_SIZE - 1, i + DTW_WINDOW_SIZE);

        for (j = j_start; j <= j_end; j++) {
            // Pula a origem, já calculada isoladamente
            if (i == 0 && j == 0) continue;

            custo_atual = fabsf(signal_a[i] - signal_b[j]);
            
            // Captura de forma segura os três vizinhos anteriores na malha de estados.
            // Se o vizinho estiver posicionado fora das bordas da tabela, herda INFINITY automaticamente.
            float custo_cima = (i > 0) ? matriz_custo[i-1][j] : INFINITY;
            float custo_esq  = (j > 0) ? matriz_custo[i][j-1] : INFINITY;
            float custo_diag = (i > 0 && j > 0) ? matriz_custo[i-1][j-1] : INFINITY;

            matriz_custo[i][j] = custo_atual + min_of_3(custo_cima, custo_esq, custo_diag);
        }
    }

    float distancia_total = matriz_custo[DTW_SIGNAL_SIZE-1][DTW_SIGNAL_SIZE-1]; /**< Custo total acumulado na célula final. */

    /* ----------------------------------------------------------------------
     * FASE 2: BACKTRACKING ITERATIVO (Sem Recursão)
     * Realiza a reconstrução do caminho do último elemento (N-1, M-1)
     * até a origem (0,0), seguindo a trilha confinada dentro da janela.
     * ---------------------------------------------------------------------- */
    i = DTW_SIGNAL_SIZE - 1;
    j = DTW_SIGNAL_SIZE - 1;
    int step_count = 0; /**< Contador do número de passos do caminho final. */

    while (i > 0 || j > 0) {
        // Registra o nó atual como parte do caminho ótimo.
        path_out[step_count].x = i;
        path_out[step_count].y = j;
        step_count++;

        // Tratamento de bordas durante o percurso de volta
        if (i == 0) {
            j--; // Limite superior atingido, recua para a esquerda.
        } else if (j == 0) {
            i--; // Limite esquerdo atingido, recua para cima.
        } else {
            // Busca gulosa reversa pelos vizinhos diretos
            float diag = matriz_custo[i-1][j-1]; /**< Custo vizinho diagonal. */
            float cima = matriz_custo[i-1][j];   /**< Custo vizinho de cima. */
            float esq  = matriz_custo[i][j-1];   /**< Custo vizinho à esquerda. */

            // Prioridade de empate dada à diagonal (match estável)
            if (diag <= cima && diag <= esq) {
                i--; j--;
            } else if (cima <= diag && cima <= esq) {
                i--;
            } else {
                j--;
            }
        }
    }

    // Salva a coordenada da origem, garantindo que o caminho seja fechado.
    path_out[step_count].x = 0;
    path_out[step_count].y = 0;
    step_count++;

    // Exporta o tamanho real do caminho percorrido.
    *path_length = step_count;

    return distancia_total;
}