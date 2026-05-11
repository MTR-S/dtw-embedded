/**
 * @file main.c
 * @brief Bancada de testes (Testbench) automatizada para o algoritmo DTW.
 *
 * @details Como usar (help): Este arquivo contém o ponto de entrada principal do programa.
 * Ele inicializa os vetores, aguarda a injeção do ID do cenário via entrada padrão (stdin),
 * popula os sinais com formatos analógicos variados, aciona o cálculo DTW e exibe os resultados.
 * Ele foi projetado para ser orquestrado por um script Python externo.
 * * Contexto do desenvolvimento: Trabalho da disciplina Sistemas Embarcados/T1 Workstation.
 * * Entrada: Um número inteiro (0 a 19) via entrada padrão (`stdin`), representando o cenário.
 * * Saída: Texto formatado via saída padrão (`stdout`) contendo a Distância Escalar DTW (Custo), 
 * o número de passos do caminho ótimo (Backtracking) e o tempo de execução em segundos.
 * * Plataforma Alvo: Estação de Trabalho Host (PC Windows/Linux/macOS).
 *
 * @author Matheus de Sousa Almeida e Vinicius Silva Pereira
 * @date 30 de Abril de 2026
 * @copyright Permissões de uso: Uso acadêmico.
 */

#define _POSIX_C_SOURCE 199309L /**< Habilita funções de tempo de alta precisão do POSIX. */
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include "dtw.h"

/* ==========================================================================
 * FUNÇÕES AUXILIARES PARA INJEÇÃO DE SINAIS NÃO-BINÁRIOS
 * ========================================================================== */

/**
 * @brief Copia um padrão de onda para dentro do vetor de sinal.
 */
void injetar_sinal(float *vetor, int inicio, const float *formato, int tamanho) {
    for(int i = 0; i < tamanho; i++) {
        if ((inicio + i) < DTW_SIGNAL_SIZE) {
            vetor[inicio + i] = formato[i];
        }
    }
}

/**
 * @brief Preenche uma faixa do vetor com um valor analógico constante.
 */
void preencher_constante(float *vetor, int inicio, int fim, float valor) {
    for(int i = inicio; i <= fim && i < DTW_SIGNAL_SIZE; i++) {
        vetor[i] = valor;
    }
}

/**
 * @brief Ponto de entrada da aplicação. Orquestra a execução de um cenário de teste específico.
 */
int main(void) {
    /* Declaração e inicialização estática dos vetores de sinal e caminho. */
    float signal_a[DTW_SIGNAL_SIZE] = {0.0f}; /**< Vetor do sinal A, inicializado com zeros. */
    float signal_b[DTW_SIGNAL_SIZE] = {0.0f}; /**< Vetor do sinal B, inicializado com zeros. */
    dtw_path_point_t path[DTW_MAX_PATH_LEN];  /**< Buffer para armazenar a rota de backtracking. */
    int path_length = 0;                      /**< Variável para capturar o tamanho real da rota. */

    int CENARIO_TESTE = -1; /**< ID numérico do caso de teste a ser executado. */
    
    // Interface de comunicação com o usuário ou script orquestrador
    printf("-> Digite sua opcao (0 a 19): ");
    if (scanf("%d", &CENARIO_TESTE) != 1 || CENARIO_TESTE < 0 || CENARIO_TESTE > 19) {
        printf("\n[ERRO] Entrada invalida. Selecione um cenario entre 0 e 19.\n");
        return 1;
    }

    /* ==========================================================
     * BIBLIOTECA DE PADRÕES DE ONDA (Simulação Analógica)
     * ========================================================== */
    const float pulso_gaussiano[11] = {0.2f, 0.6f, 1.5f, 2.8f, 3.7f, 4.0f, 3.7f, 2.8f, 1.5f, 0.6f, 0.2f};
    const float pulso_quadrado[6]   = {1.5f, 3.2f, 3.2f, 3.2f, 3.2f, 1.5f};
    const float ruido_forte[3]      = {3.8f, 4.5f, 2.9f};

    /* ==========================================================
     * INJEÇÃO DOS 20 CENÁRIOS DE TESTE
     * ========================================================== */
    
    // --- CATEGORIA 1: SINAIS IDEAIS E IGUAIS ---
    if      (CENARIO_TESTE == 0) { 
        injetar_sinal(signal_a, 10, pulso_gaussiano, 11); 
        injetar_sinal(signal_b, 10, pulso_gaussiano, 11); 
    } 
    else if (CENARIO_TESTE == 1) { 
        injetar_sinal(signal_a, 15, pulso_quadrado, 6); 
        injetar_sinal(signal_b, 15, pulso_quadrado, 6); 
    } 
    else if (CENARIO_TESTE == 2) { 
        // Sinal Dente de Serra longo
        for(int i=5; i<=40; i++) { signal_a[i] = signal_b[i] = 0.5f + (i % 5) * 0.6f; } 
    } 
    else if (CENARIO_TESTE == 3) { 
        // Pulsos duplos
        injetar_sinal(signal_a, 10, pulso_quadrado, 6); injetar_sinal(signal_b, 10, pulso_quadrado, 6);
        injetar_sinal(signal_a, 30, pulso_quadrado, 6); injetar_sinal(signal_b, 30, pulso_quadrado, 6);
    } 
    
    // --- CATEGORIA 2: ATRASOS TEMPORAIS (SHIFT) ---
    else if (CENARIO_TESTE == 4) { 
        injetar_sinal(signal_a, 10, pulso_gaussiano, 11); 
        injetar_sinal(signal_b, 15, pulso_gaussiano, 11); 
    } 
    else if (CENARIO_TESTE == 5) { 
        injetar_sinal(signal_a, 20, pulso_gaussiano, 11); 
        injetar_sinal(signal_b, 10, pulso_gaussiano, 11); 
    } 
    else if (CENARIO_TESTE == 6) { 
        injetar_sinal(signal_a, 5, pulso_gaussiano, 11); 
        injetar_sinal(signal_b, 30, pulso_gaussiano, 11); 
    } 
    else if (CENARIO_TESTE == 7) { 
        injetar_sinal(signal_a, 30, pulso_gaussiano, 11); 
        injetar_sinal(signal_b, 5, pulso_gaussiano, 11); 
    } 

    // --- CATEGORIA 3: DISTORÇÃO ELÁSTICA (WARPING) ---
    else if (CENARIO_TESTE == 8)  { 
        injetar_sinal(signal_a, 10, pulso_quadrado, 6); 
        preencher_constante(signal_b, 10, 25, 3.2f); signal_b[9] = 1.5f; signal_b[26] = 1.5f; 
    } 
    else if (CENARIO_TESTE == 9)  { 
        preencher_constante(signal_a, 10, 25, 3.2f); signal_a[9] = 1.5f; signal_a[26] = 1.5f; 
        injetar_sinal(signal_b, 10, pulso_quadrado, 6); 
    } 
    else if (CENARIO_TESTE == 10) { 
        injetar_sinal(signal_a, 5, pulso_gaussiano, 11); 
        // Rampa suave de expansão
        for(int i=5; i<=35; i++) { signal_b[i] = 0.5f + (i - 5) * 0.11f; } 
    } 
    else if (CENARIO_TESTE == 11) { 
        for(int i=5; i<=35; i++) { signal_a[i] = 0.5f + (i - 5) * 0.11f; } 
        injetar_sinal(signal_b, 5, pulso_gaussiano, 11); 
    } 

    // --- CATEGORIA 4: RUÍDOS E ANOMALIAS ---
    else if (CENARIO_TESTE == 12) { 
        injetar_sinal(signal_a, 10, pulso_gaussiano, 11); 
        injetar_sinal(signal_b, 10, pulso_gaussiano, 11); 
        injetar_sinal(signal_b, 2, ruido_forte, 3); // Ruído antes do sinal
    } 
    else if (CENARIO_TESTE == 13) { 
        injetar_sinal(signal_a, 10, pulso_gaussiano, 11); 
        injetar_sinal(signal_b, 10, pulso_gaussiano, 11); 
        injetar_sinal(signal_b, 40, ruido_forte, 3); // Ruído após o sinal
    } 
    else if (CENARIO_TESTE == 14) { 
        injetar_sinal(signal_a, 10, pulso_gaussiano, 11); 
        injetar_sinal(signal_b, 5, ruido_forte, 3); 
        injetar_sinal(signal_b, 35, ruido_forte, 3); 
    } 
    else if (CENARIO_TESTE == 15) { 
        injetar_sinal(signal_a, 10, pulso_gaussiano, 11); 
        // Sinal B invertido na fase (espelhado)
        preencher_constante(signal_b, 0, 44, 2.0f);
        for(int i=0; i<11; i++) { signal_b[10 + i] = 4.0f - pulso_gaussiano[i]; } 
    } 

    // --- CATEGORIA 5: CASOS EXTREMOS DE SENSOR ---
    else if (CENARIO_TESTE == 16) { /* Ambos vazios (0.0f) */ } 
    else if (CENARIO_TESTE == 17) { 
        preencher_constante(signal_a, 0, 44, 5.0f); 
        preencher_constante(signal_b, 0, 44, 5.0f); 
    } 
    else if (CENARIO_TESTE == 18) { injetar_sinal(signal_a, 10, pulso_gaussiano, 11); } 
    else if (CENARIO_TESTE == 19) { injetar_sinal(signal_b, 10, pulso_gaussiano, 11); } 

    /* ----------------------------------------------------------------------
     * EXECUÇÃO E BENCHMARKING DE PERFORMANCE
     * Mede o tempo de relógio monotônico contornando oscilações de SO.
     * ---------------------------------------------------------------------- */
    struct timespec start; /**< Estrutura para capturar o tempo de início. */
    struct timespec end;   /**< Estrutura para capturar o tempo de término. */
    
    clock_gettime(CLOCK_MONOTONIC, &start); 
    
    // Chama o módulo core do DTW
    float distance = dtw_compute(signal_a, signal_b, path, &path_length);
    
    clock_gettime(CLOCK_MONOTONIC, &end); 

    // Converte segundos e nanossegundos para uma única variável float em segundos
    double tempo_execucao_s = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    /* ----------------------------------------------------------------------
     * EXPORTAÇÃO DOS RESULTADOS 
     * ---------------------------------------------------------------------- */
    printf(" -> Distancia Escalar DTW Final: %.8f\n", distance);
    printf(" -> Passos no Caminho Otimo: %d passos\n", path_length);
    printf(" -> Tempo de Execucao (C): %.7f segundos\n", tempo_execucao_s);

    return 0;
}