/**
 * @file main.c
 * @brief Bancada de testes (Testbench) automatizada para o algoritmo DTW.
 *
 * @details Como usar (help): Este arquivo contém o ponto de entrada principal do programa.
 * Ele inicializa os vetores, aguarda a injeção do ID do cenário via entrada padrão (stdin),
 * popula os sinais, aciona o cálculo DTW e exibe os resultados na saída padrão (stdout).
 * Ele foi projetado para ser orquestrado por um script Python externo.
 * 
 * Contexto do desenvolvimento: Trabalho da disciplina Sistemas Embarcados/T1 Workstation.
 * 
 * Entrada: Um número inteiro (0 a 19) via entrada padrão (`stdin`), representando o cenário.
 * 
 * Saída: Texto formatado via saída padrão (`stdout`) contendo a Distância Escalar DTW (Custo), 
 * o número de passos do caminho ótimo (Backtracking) e o tempo de execução em segundos.
 * 
 * Plataforma Alvo: Estação de Trabalho Host (PC Windows/Linux/macOS).
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

/**
 * @brief Ponto de entrada da aplicação. Orquestra a execução de um cenário de teste específico.
 *
 * @return int Retorna 0 (EXIT_SUCCESS) em caso de execução correta, ou 1 (EXIT_FAILURE) em falhas.
 *
 * @note OBRIGATÓRIO: Nenhuma variável global afetada (diretamente neste escopo). 
 * Variáveis globais são alteradas apenas internamente pelas chamadas ao módulo dtw.c.
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
     * INJEÇÃO DOS 20 CENÁRIOS DE TESTE
     * Lógica de chaveamento para popular os arrays baseada no ID.
     * ========================================================== */
    
    // --- CATEGORIA 1: SINAIS IDEAIS E IGUAIS ---
    if      (CENARIO_TESTE == 0) { for(int i=10; i<=20; i++) { signal_a[i]=1; signal_b[i]=1; } } 
    else if (CENARIO_TESTE == 1) { for(int i=15; i<=18; i++) { signal_a[i]=1; signal_b[i]=1; } } 
    else if (CENARIO_TESTE == 2) { for(int i=5;  i<=40; i++) { signal_a[i]=1; signal_b[i]=1; } } 
    else if (CENARIO_TESTE == 3) { for(int i=10; i<=15; i++) { signal_a[i]=1; signal_b[i]=1; }   
                                   for(int i=30; i<=35; i++) { signal_a[i]=1; signal_b[i]=1; } } 
    
    // --- CATEGORIA 2: ATRASOS TEMPORAIS (SHIFT) ---
    else if (CENARIO_TESTE == 4) { for(int i=10; i<=20; i++) signal_a[i]=1; for(int i=15; i<=25; i++) signal_b[i]=1; } 
    else if (CENARIO_TESTE == 5) { for(int i=20; i<=30; i++) signal_a[i]=1; for(int i=10; i<=20; i++) signal_b[i]=1; } 
    else if (CENARIO_TESTE == 6) { for(int i=5;  i<=10; i++) signal_a[i]=1; for(int i=35; i<=40; i++) signal_b[i]=1; } 
    else if (CENARIO_TESTE == 7) { for(int i=30; i<=35; i++) signal_a[i]=1; for(int i=5;  i<=10; i++) signal_b[i]=1; } 

    // --- CATEGORIA 3: DISTORÇÃO ELÁSTICA (WARPING) ---
    else if (CENARIO_TESTE == 8)  { for(int i=10; i<=15; i++) signal_a[i]=1; for(int i=10; i<=25; i++) signal_b[i]=1; } 
    else if (CENARIO_TESTE == 9)  { for(int i=10; i<=25; i++) signal_a[i]=1; for(int i=10; i<=15; i++) signal_b[i]=1; } 
    else if (CENARIO_TESTE == 10) { for(int i=5;  i<=10; i++) signal_a[i]=1; for(int i=5;  i<=35; i++) signal_b[i]=1; } 
    else if (CENARIO_TESTE == 11) { for(int i=5;  i<=35; i++) signal_a[i]=1; for(int i=5;  i<=10; i++) signal_b[i]=1; } 

    // --- CATEGORIA 4: RUÍDOS E ANOMALIAS ---
    else if (CENARIO_TESTE == 12) { for(int i=10; i<=20; i++) signal_a[i]=1; for(int i=10; i<=20; i++) signal_b[i]=1; signal_b[2]=1; signal_b[3]=1; } 
    else if (CENARIO_TESTE == 13) { for(int i=10; i<=20; i++) signal_a[i]=1; for(int i=10; i<=20; i++) signal_b[i]=1; signal_b[42]=1; signal_b[43]=1; } 
    else if (CENARIO_TESTE == 14) { for(int i=10; i<=20; i++) signal_a[i]=1; for(int i=5;  i<=8;  i++) signal_b[i]=1; for(int i=35; i<=38; i++) signal_b[i]=1; } 
    else if (CENARIO_TESTE == 15) { for(int i=10; i<=20; i++) signal_a[i]=1; for(int i=0;  i<=9;  i++) signal_b[i]=1; for(int i=21; i<=44; i++) signal_b[i]=1; } 

    // --- CATEGORIA 5: CASOS EXTREMOS DE SENSOR ---
    else if (CENARIO_TESTE == 16) { /* Ambos vazios (0.0), nada a fazer - Simula ausência de sinal. */ } 
    else if (CENARIO_TESTE == 17) { for(int i=0; i<45; i++) { signal_a[i]=1; signal_b[i]=1; } } 
    else if (CENARIO_TESTE == 18) { for(int i=10; i<=20; i++) signal_a[i]=1; } 
    else if (CENARIO_TESTE == 19) { for(int i=10; i<=20; i++) signal_b[i]=1; } 

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
     * EXPORTAÇÃO DOS RESULTADOS (Padrão Regex-Friendly)
     * ---------------------------------------------------------------------- */
    printf(" -> Distancia Escalar DTW Final: %.8f\n", distance);
    printf(" -> Passos no Caminho Otimo: %d passos\n", path_length);
    printf(" -> Tempo de Execucao (C): %.7f segundos\n", tempo_execucao_s);

    return 0;
}