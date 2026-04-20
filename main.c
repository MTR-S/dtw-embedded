#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Função auxiliar para encontrar o mínimo entre três valores de ponto flutuante
double min3(double a, double b, double c) {
    double min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}

// Função principal do Dynamic Time Warping
double dtw_distance(double *seq1, int n, double *seq2, int m) {
    // Aloca memória dinamicamente para a matriz de programação dinâmica (n+1) x (m+1)
    double **dtw = (double **)malloc((n + 1) * sizeof(double *));
    if (dtw == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    
    for (int i = 0; i <= n; i++) {
        dtw[i] = (double *)malloc((m + 1) * sizeof(double));
        if (dtw[i] == NULL) {
            printf("Erro de alocação de memória!\n");
            exit(1);
        }
    }

    // Inicialização: define todos os valores como "infinito"
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            dtw[i][j] = INFINITY;
        }
    }
    
    // O custo de alinhar duas sequências vazias é 0
    dtw[0][0] = 0.0;

    // Preenchimento da matriz de custos
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            // Custo local: distância absoluta entre os elementos (distância Euclidiana 1D)
            double cost = fabs(seq1[i - 1] - seq2[j - 1]);
            
            // Equação de recorrência do DTW
            dtw[i][j] = cost + min3(
                dtw[i - 1][j],    // Inserção
                dtw[i][j - 1],    // Deleção
                dtw[i - 1][j - 1] // Correspondência (Match)
            );
        }
    }

    // A distância final fica armazenada no canto inferior direito da matriz
    double final_distance = dtw[n][m];

    // Liberação da memória alocada
    for (int i = 0; i <= n; i++) {
        free(dtw[i]);
    }
    free(dtw);

    return final_distance;
}

int main() {
    // Sequências de exemplo
    double sequencia_A[] = {1, 3, 4, 9, 8, 2, 1, 5, 7, 3};
    double sequencia_B[] = {1, 6, 2, 3, 0, 9, 4, 3, 6, 3};
    
    // Calculando o tamanho das sequências
    int tamanho_A = sizeof(sequencia_A) / sizeof(sequencia_A[0]);
    int tamanho_B = sizeof(sequencia_B) / sizeof(sequencia_B[0]);

    // Executando o DTW
    double distancia = dtw_distance(sequencia_A, tamanho_A, sequencia_B, tamanho_B);
    
    printf("A distância DTW entre as sequencias é: %f\n", distancia);

    return 0;
}