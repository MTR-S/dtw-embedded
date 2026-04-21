/**
 * @file main.c
 * @brief Bancada de testes no PC para validação do algoritmo DTW.
 * @details Injeta sinais artificiais conhecidos para validar o alinhamento
 * temporal antes da migração para o microcontrolador. Apresenta UI no terminal.
 */

#include <stdio.h>
#include "dtw.h"

/* * Acesso à matriz global declarada no dtw.c para impressão do fragmento. */
extern float matriz_custo[DTW_SIGNAL_SIZE][DTW_SIGNAL_SIZE];

/**
 * @brief Função auxiliar para plotar gráficos de sinais simples em texto (ASCII).
 */
void plot_signal_ascii(const char* label, const float* signal) {
    printf("%s |", label);
    for (int i = 0; i < DTW_SIGNAL_SIZE; i++) {
        if (signal[i] > 0.5f) printf("#");
        else printf("_");
    }
    printf("|\n");
}

int main(void) {
    // 1. Alocação Estática (Zero malloc)
    float signal_a[DTW_SIGNAL_SIZE] = {0.0f};
    float signal_b[DTW_SIGNAL_SIZE] = {0.0f};
    dtw_path_point_t path[DTW_MAX_PATH_LEN];
    int path_length = 0;

    // ==========================================================
    // 2. SELEÇÃO DE CENÁRIO PARA A DEMONSTRAÇÃO
    // ==========================================================
    // Modifique esta variável (1, 2 ou 3) antes de compilar para apresentar
    int CENARIO_TESTE = 1; 

    printf("\n======================================================================\n");
    printf("             VALIDAÇÃO DTW - SISTEMAS EMBARCADOS (T1)                 \n");
    printf("======================================================================\n\n");

    if (CENARIO_TESTE == 1) {
        printf(">> CENARIO 1: O Atraso Perfeito (Mesmo sinal, tempos diferentes) <<\n\n");
        for (int i = 10; i <= 15; i++) signal_a[i] = 1.0f; // Pulso Original
        for (int i = 30; i <= 35; i++) signal_b[i] = 1.0f; // Pulso Atrasado
    } 
    else if (CENARIO_TESTE == 2) {
        printf(">> CENARIO 2: A Distorcao Elastica (Sinal rapido vs Sinal Lento) <<\n\n");
        for (int i = 10; i <= 15; i++) signal_a[i] = 1.0f; // Curto
        for (int i = 10; i <= 25; i++) signal_b[i] = 1.0f; // Longo
    } 
    else if (CENARIO_TESTE == 3) {
        printf(">> CENARIO 3: A Rejeicao de Anomalia (Falso Positivo) <<\n\n");
        for (int i = 10; i <= 20; i++) signal_a[i] = 1.0f; // Pulso normal
        for (int i = 5;  i <= 8;  i++) signal_b[i] = 1.0f; // Ruido 1
        for (int i = 35; i <= 38; i++) signal_b[i] = 1.0f; // Ruido 2
    }

    // 3. Impressão dos Sinais de Entrada
    printf("[1] SINAIS DE ENTRADA (Janela de 45 amostras)\n");
    plot_signal_ascii("Sinal A (Ref)   ", signal_a);
    plot_signal_ascii("Sinal B (Teste) ", signal_b);
    printf("\nComputando Matriz de Programação Dinâmica...\n\n");

    // 4. Execução do Algoritmo
    float distance = dtw_compute(signal_a, signal_b, path, &path_length);

    // 5. Exibição dos Resultados Métricos
    printf("[2] RESULTADOS DO ALINHAMENTO\n");
    printf(" -> Distancia Escalar DTW Final: %.2f\n", distance);
    printf(" -> Passos no Caminho Otimo: %d passos (Max: %d)\n\n", path_length, DTW_MAX_PATH_LEN);

    // 6. Exibição Fragmentada da Matriz
    printf("[3] FRAGMENTO DA MATRIZ DE CUSTO (Canto 10x10)\n");
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%4.1f ", matriz_custo[i][j]);
        }
        printf("\n");
    }
    printf(" (Restante omitido por legibilidade...)\n\n");

    // 7. Mapa Visual do Backtracking
    printf("[4] MAPA VISUAL DO BACKTRACKING (Grade 45x45)\n");
    printf(" ('.' = Matriz preenchida | '*' = Caminho escolhido de ponta a ponta)\n\n");

    char mapa_visual[DTW_SIGNAL_SIZE][DTW_SIGNAL_SIZE];
    
    for (int i = 0; i < DTW_SIGNAL_SIZE; i++) {
        for (int j = 0; j < DTW_SIGNAL_SIZE; j++) {
            mapa_visual[i][j] = '.';
        }
    }
    
    for (int k = 0; k < path_length; k++) {
        mapa_visual[path[k].x][path[k].y] = '*';
    }

    for (int i = 0; i < DTW_SIGNAL_SIZE; i++) {
        printf("%02d| ", i);
        for (int j = 0; j < DTW_SIGNAL_SIZE; j++) {
            printf("%c ", mapa_visual[i][j]);
        }
        printf("\n");
    }

    printf("\n======================================================================\n\n");

    return 0;
}