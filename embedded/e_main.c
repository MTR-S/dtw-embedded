/**
 * @file      main.c
 * @author    Matheus de Sousa Almeida e [Nome da sua Dupla]
 * @date      11 de Maio de 2026
 * @brief     Bancada embarcada de validação do algoritmo DTW via UART (Ponto Fixo).
 * @details   Desenvolvido para a disciplina de Sistemas Embarcados (T2).
 * O sistema gera arrays de sinais escalonados simulando curvas analógicas,
 * calcula a distância elástica temporal e transmite os resultados decimais via USART2.
 * @copyright Todos os direitos reservados.
 *
 * @note      RESTRIÇÕES DA PLATAFORMA ALVO:
 * - MCU: STM32F030R8 (ARM Cortex-M0 sem FPU).
 * - Memória limitante: 8 KB SRAM.
 * - Solução: Adoção de Matemática de Ponto Fixo. Sinais e Matriz DTW foram refatorados
 * para 'uint16_t' com fator de escala 100 (2 casas decimais). Isso eliminou
 * o overhead de processamento (soft-float) e impediu o RAM/Integer Overflow.
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "dtw.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 * @brief Redireciona a saída padrão (printf) para a UART2.
 */
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}

/* ==========================================================================
 * FUNÇÕES AUXILIARES PARA INJEÇÃO DE SINAIS (PONTO FIXO)
 * ========================================================================== */

/**
 * @brief Copia um padrão de onda para dentro do vetor de sinal.
 */
void injetar_sinal_fixo(uint16_t *vetor, int inicio, const uint16_t *formato, int tamanho) {
    for(int i = 0; i < tamanho; i++) {
        if ((inicio + i) < DTW_SIGNAL_SIZE) {
            vetor[inicio + i] = formato[i];
        }
    }
}

/**
 * @brief Preenche uma faixa do vetor com um valor constante escalonado.
 */
void preencher_constante_fixa(uint16_t *vetor, int inicio, int fim, uint16_t valor) {
    for(int i = inicio; i <= fim && i < DTW_SIGNAL_SIZE; i++) {
        vetor[i] = valor;
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */

  // Alocação das variáveis com uint16_t (Ponto Fixo)
  uint16_t signal_a[DTW_SIGNAL_SIZE] = {0};
  uint16_t signal_b[DTW_SIGNAL_SIZE] = {0};
  dtw_path_point_t path[DTW_MAX_PATH_LEN];
  int path_length = 0;

  /* ==========================================================
   * BIBLIOTECA DE PADRÕES DE ONDA (ESCALONADOS x100)
   * Armazenados na Flash (const) para evitar consumo de SRAM
   * ========================================================== */
  const uint16_t pulso_gaussiano[11] = {20, 60, 150, 280, 370, 400, 370, 280, 150, 60, 20};
  const uint16_t pulso_quadrado[6]   = {150, 320, 320, 320, 320, 150};
  const uint16_t ruido_forte[3]      = {380, 450, 290};

  // Limpa o terminal na inicialização
  printf("\033[2J\033[H");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    printf("\n==========================================================\r\n");
    printf("   VALIDACAO DTW EMBARCADO (PONTO FIXO) - NUCLEO STM32 \r\n");
    printf("==========================================================\r\n");
    printf("ID | CUSTO (Decimal) | PASSOS ROTA | TEMPO (ms)\r\n");
    printf("----------------------------------------------------------\r\n");

    for (int cenario = 0; cenario < 20; cenario++) {

        // Zera os arrays para a próxima iteração
        for(int i = 0; i < DTW_SIGNAL_SIZE; i++) { signal_a[i] = 0; signal_b[i] = 0; }

        /* ==========================================================
         * INJEÇÃO DOS 20 CENÁRIOS DE TESTE DIRETAMENTE NA PLACA
         * ========================================================== */

        // --- CATEGORIA 1: SINAIS IDEAIS E IGUAIS ---
        if      (cenario == 0) {
            injetar_sinal_fixo(signal_a, 10, pulso_gaussiano, 11);
            injetar_sinal_fixo(signal_b, 10, pulso_gaussiano, 11);
        }
        else if (cenario == 1) {
            injetar_sinal_fixo(signal_a, 15, pulso_quadrado, 6);
            injetar_sinal_fixo(signal_b, 15, pulso_quadrado, 6);
        }
        else if (cenario == 2) {
            // Sinal Dente de Serra longo (0.5 + variação) escalonado x100
            for(int i=5; i<=40; i++) { signal_a[i] = signal_b[i] = 50 + (i % 5) * 60; }
        }
        else if (cenario == 3) {
            injetar_sinal_fixo(signal_a, 10, pulso_quadrado, 6); injetar_sinal_fixo(signal_b, 10, pulso_quadrado, 6);
            injetar_sinal_fixo(signal_a, 30, pulso_quadrado, 6); injetar_sinal_fixo(signal_b, 30, pulso_quadrado, 6);
        }

        // --- CATEGORIA 2: ATRASOS TEMPORAIS (SHIFT) ---
        else if (cenario == 4) {
            injetar_sinal_fixo(signal_a, 10, pulso_gaussiano, 11);
            injetar_sinal_fixo(signal_b, 15, pulso_gaussiano, 11);
        }
        else if (cenario == 5) {
            injetar_sinal_fixo(signal_a, 20, pulso_gaussiano, 11);
            injetar_sinal_fixo(signal_b, 10, pulso_gaussiano, 11);
        }
        else if (cenario == 6) {
            injetar_sinal_fixo(signal_a, 5, pulso_gaussiano, 11);
            injetar_sinal_fixo(signal_b, 30, pulso_gaussiano, 11);
        }
        else if (cenario == 7) {
            injetar_sinal_fixo(signal_a, 30, pulso_gaussiano, 11);
            injetar_sinal_fixo(signal_b, 5, pulso_gaussiano, 11);
        }

        // --- CATEGORIA 3: DISTORÇÃO ELÁSTICA (WARPING) ---
        else if (cenario == 8)  {
            injetar_sinal_fixo(signal_a, 10, pulso_quadrado, 6);
            preencher_constante_fixa(signal_b, 10, 25, 320); signal_b[9] = 150; signal_b[26] = 150;
        }
        else if (cenario == 9)  {
            preencher_constante_fixa(signal_a, 10, 25, 320); signal_a[9] = 150; signal_a[26] = 150;
            injetar_sinal_fixo(signal_b, 10, pulso_quadrado, 6);
        }
        else if (cenario == 10) {
            injetar_sinal_fixo(signal_a, 5, pulso_gaussiano, 11);
            // Rampa suave de expansão escalonada x100
            for(int i=5; i<=35; i++) { signal_b[i] = 50 + (i - 5) * 11; }
        }
        else if (cenario == 11) {
            for(int i=5; i<=35; i++) { signal_a[i] = 50 + (i - 5) * 11; }
            injetar_sinal_fixo(signal_b, 5, pulso_gaussiano, 11);
        }

        // --- CATEGORIA 4: RUÍDOS E ANOMALIAS ---
        else if (cenario == 12) {
            injetar_sinal_fixo(signal_a, 10, pulso_gaussiano, 11);
            injetar_sinal_fixo(signal_b, 10, pulso_gaussiano, 11);
            injetar_sinal_fixo(signal_b, 2, ruido_forte, 3);
        }
        else if (cenario == 13) {
            injetar_sinal_fixo(signal_a, 10, pulso_gaussiano, 11);
            injetar_sinal_fixo(signal_b, 10, pulso_gaussiano, 11);
            injetar_sinal_fixo(signal_b, 40, ruido_forte, 3);
        }
        else if (cenario == 14) {
            injetar_sinal_fixo(signal_a, 10, pulso_gaussiano, 11);
            injetar_sinal_fixo(signal_b, 5, ruido_forte, 3);
            injetar_sinal_fixo(signal_b, 35, ruido_forte, 3);
        }
        else if (cenario == 15) {
            injetar_sinal_fixo(signal_a, 10, pulso_gaussiano, 11);
            preencher_constante_fixa(signal_b, 0, 44, 200);
            for(int i=0; i<11; i++) { signal_b[10 + i] = 400 - pulso_gaussiano[i]; }
        }

        // --- CATEGORIA 5: CASOS EXTREMOS DE SENSOR ---
        else if (cenario == 16) { /* Ambos vazios (0) */ }
        else if (cenario == 17) {
            preencher_constante_fixa(signal_a, 0, 44, 500);
            preencher_constante_fixa(signal_b, 0, 44, 500);
        }
        else if (cenario == 18) { injetar_sinal_fixo(signal_a, 10, pulso_gaussiano, 11); }
        else if (cenario == 19) { injetar_sinal_fixo(signal_b, 10, pulso_gaussiano, 11); }

        // ==========================================================
        // EXECUÇÃO E BENCHMARKING
        // ==========================================================
        uint32_t start_time = HAL_GetTick(); // Inicia cronômetro do Systick

        uint16_t distance = dtw_compute(signal_a, signal_b, path, &path_length);

        uint32_t end_time = HAL_GetTick();   // Para cronômetro
        uint32_t tempo_execucao_ms = end_time - start_time;

        // Pisca o LED da placa para mostrar que está processando
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

        // O TRUQUE DE ENGENHARIA: Restaura a vírgula para impressão no PC
        // Imprime a parte inteira (distance / 100) e a parte fracionária (distance % 100)
        printf("[%02d]| %6d.%02d   | %-11d | %lu ms\r\n",
               cenario,
               distance / DTW_SCALE_FACTOR,
               distance % DTW_SCALE_FACTOR,
               path_length,
               tempo_execucao_ms);
    }

    // Desliga o LED após terminar o lote de 20
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    printf("==========================================================\r\n");
    printf("TESTE CONCLUIDO. Reiniciando em 10 segundos...\r\n");

    HAL_Delay(10000); // Espera 10 segundos antes de recomeçar o loop
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
