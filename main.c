/**
 * @file main.c
 * @brief Bancada de testes no PC para validação do algoritmo DTW.
 * @details Injeta sinais artificiais conhecidos para validar o alinhamento
 * temporal antes da migração para o microcontrolador.
 */

#include <stdio.h>
#include "dtw.h"

int main(void) {
    // 1. Alocação dos vetores de entrada e saída (Utilizando apenas alocamento estático, para não utilizar malloc)
    float signal_a[DTW_SIGNAL_SIZE] = {0.0f}; // Inicializa tudo com 0
    float signal_b[DTW_SIGNAL_SIZE] = {0.0f};
    dtw_path_point_t path[DTW_MAX_PATH_LEN];
    int path_length = 0;

    // 2. Geração dos Sinais de Teste (Procedimento de Validação)
    // Sinal A: Um pulso quadrado entre os índices 10 e 20
    for (int i = 10; i <= 20; i++) {
        signal_a[i] = 1.0f;
    }

    // Sinal B: O mesmo pulso quadrado, mas atrasado (entre 15 e 25)
    for (int i = 15; i <= 25; i++) {
        signal_b[i] = 1.0f;
    }

    printf("==================================================\n");
    printf("   VALIDAÇÃO DTW - SISTEMAS EMBARCADOS (T1)       \n");
    printf("==================================================\n");
    printf("Sinal A: Pulso em t=[10, 20]\n");
    printf("Sinal B: Pulso em t=[15, 25] (Atrasado)\n");
    printf("Iniciando computação na matriz de custo global...\n\n");

    // 3. Execução do Algoritmo
    float distance = dtw_compute(signal_a, signal_b, path, &path_length);

    // 4. Exibição dos Resultados
    printf("RESULTADOS:\n");
    printf("-> Distancia Escalar DTW: %.2f\n", distance);
    printf("-> Passos no Caminho Otimo: %d passos (Max Permitido: %d)\n\n", path_length, DTW_MAX_PATH_LEN);

    printf("ROTA DE ALINHAMENTO (Do Fim [44,44] para o Inicio [0,0]):\n");
    for (int i = 0; i < path_length; i++) {
        printf("(%02d, %02d) ", path[i].x, path[i].y);
        // Quebra a linha a cada 8 coordenadas para ficar bonito no terminal
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("\n\n==================================================\n");

    return 0;
}