/**
 * @file main.c
 * @brief Bancada de testes no PC para validação do algoritmo DTW.
 * @details Injeta sinais artificiais conhecidos para validar o alinhamento
 * temporal antes da migração para o microcontrolador. Apresenta UI no terminal.
 */

#include <stdio.h>
#include <stdlib.h> // Necessário para exit()
#include "dtw.h"

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

    printf("\n======================================================================\n");
    printf("             VALIDAÇÃO DTW - SISTEMAS EMBARCADOS (T1)                 \n");
    printf("======================================================================\n\n");

    // ==========================================================
    // 2. MENU INTERATIVO PARA SELEÇÃO DE CENÁRIO
    // ==========================================================
    int CENARIO_TESTE = -1; 
    
    printf("Selecione o cenario de teste para a demonstracao:\n");
    printf("  [0] Aceitacao Total (Sinais exatamente iguais)\n");
    printf("  [1] O Atraso Perfeito (Mesmo sinal, tempos diferentes)\n");
    printf("  [2] A Distorcao Elastica (Sinal rapido vs Sinal Lento)\n");
    printf("  [3] A Rejeicao de Anomalia (Falso Positivo)\n");
    printf("-> Digite sua opcao (0 a 3): ");

    // Lê a entrada do teclado e valida se é um número e se está no intervalo correto
    if (scanf("%d", &CENARIO_TESTE) != 1 || CENARIO_TESTE < 0 || CENARIO_TESTE > 3) {
        printf("\n[ERRO] Entrada invalida. O programa foi encerrado.\n");
        return 1;
    }
    
    printf("\n----------------------------------------------------------------------\n\n");

    if (CENARIO_TESTE == 0) {
        printf(">> CENARIO 0: Aceitacao total <<\n\n");
        for (int i = 10; i <= 20; i++) signal_a[i] = 1.0f; 
        for (int i = 10; i <= 20; i++) signal_b[i] = 1.0f; 
    }
    else if (CENARIO_TESTE == 1) {
        printf(">> CENARIO 1: O Atraso Perfeito <<\n\n");
        for (int i = 10; i <= 15; i++) signal_a[i] = 1.0f; 
        for (int i = 30; i <= 35; i++) signal_b[i] = 1.0f; 
    } 
    else if (CENARIO_TESTE == 2) {
        printf(">> CENARIO 2: A Distorcao Elastica <<\n\n");
        for (int i = 10; i <= 15; i++) signal_a[i] = 1.0f; 
        for (int i = 10; i <= 25; i++) signal_b[i] = 1.0f; 
    } 
    else if (CENARIO_TESTE == 3) {
        printf(">> CENARIO 3: A Rejeicao de Anomalia <<\n\n");
        for (int i = 10; i <= 20; i++) signal_a[i] = 1.0f; 
        for (int i = 5;  i <= 8;  i++) signal_b[i] = 1.0f; 
        for (int i = 35; i <= 38; i++) signal_b[i] = 1.0f; 
    }

    // 3. Impressão dos Sinais de Entrada
    printf("[1] SINAIS DE ENTRADA (Janela de 45 amostras)\n");
    plot_signal_ascii("Sinal A (Ref)   ", signal_a);
    plot_signal_ascii("Sinal B (Teste) ", signal_b);
    printf("\nComputando Matriz de Programacao Dinamica...\n\n");

    // 4. Execução do Algoritmo
    float distance = dtw_compute(signal_a, signal_b, path, &path_length);

    // 5. Exibição dos Resultados Métricos
    printf("[2] RESULTADOS DO ALINHAMENTO\n");
    printf(" -> Distancia Escalar DTW Final: %.2f\n", distance);
    printf(" -> Passos no Caminho Otimo: %d passos (Max: %d)\n", path_length, DTW_MAX_PATH_LEN);

    printf("\n======================================================================\n\n");

    return 0;
}