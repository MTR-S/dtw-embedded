/**
 * @file      main.c
 * @author    Matheus de Sousa Almeida e [Nome da sua Dupla]
 * @date      10 de Maio de 2026
 * @brief     Bancada embarcada de validação do algoritmo DTW via UART.
 * @details   Desenvolvido para a disciplina de Sistemas Embarcados (T2).
 * O sistema gera arrays de sinais, calcula a distância
 * elástica temporal e transmite os resultados da matriz via USART2.
 * @copyright Todos os direitos reservados.
 *
 * @note      RESTRIÇÕES DA PLATAFORMA ALVO:
 * - MCU: STM32F030R8 (ARM Cortex-M0)
 * - Memória limitante: 8 KB SRAM.
 * - Solução: Refatoração da matriz DTW de float (32 bits) para uint16_t (16 bits).
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
 * @details Permite o uso de printf para enviar strings diretamente para
 * o terminal do PC (PuTTY/TeraTerm) via cabo USB.
 */
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
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

  // Alocação das variáveis com uint16_t para salvar RAM (Downgrade de Float)
  uint16_t signal_a[DTW_SIGNAL_SIZE] = {0};
  uint16_t signal_b[DTW_SIGNAL_SIZE] = {0};
  dtw_path_point_t path[DTW_MAX_PATH_LEN];
  int path_length = 0;

  // Limpa o terminal na inicialização
  printf("\033[2J\033[H");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    printf("\n========================================================\r\n");
    printf("   VALIDACAO DTW EMBARCADO - NUCLEO STM32F030R8 \r\n");
    printf("========================================================\r\n");
    printf("ID | CUSTO (uint16) | PASSOS ROTA | TEMPO (ms)\r\n");
    printf("--------------------------------------------------------\r\n");

    for (int cenario = 0; cenario < 20; cenario++) {

        // Zera os arrays para a próxima iteração
        for(int i = 0; i < DTW_SIGNAL_SIZE; i++) { signal_a[i] = 0; signal_b[i] = 0; }

        // ==========================================================
        // INJEÇÃO DOS 20 CENÁRIOS DE TESTE DIRETAMENTE NA PLACA
        // ==========================================================
        // --- CATEGORIA 1: SINAIS IDEAIS E IGUAIS ---
        if      (cenario == 0) { for(int i=10; i<=20; i++) { signal_a[i]=1; signal_b[i]=1; } }
        else if (cenario == 1) { for(int i=15; i<=18; i++) { signal_a[i]=1; signal_b[i]=1; } }
        else if (cenario == 2) { for(int i=5;  i<=40; i++) { signal_a[i]=1; signal_b[i]=1; } }
        else if (cenario == 3) { for(int i=10; i<=15; i++) { signal_a[i]=1; signal_b[i]=1; }
                                 for(int i=30; i<=35; i++) { signal_a[i]=1; signal_b[i]=1; } }

        // --- CATEGORIA 2: ATRASOS TEMPORAIS (SHIFT) ---
        else if (cenario == 4) { for(int i=10; i<=20; i++) signal_a[i]=1; for(int i=15; i<=25; i++) signal_b[i]=1; }
        else if (cenario == 5) { for(int i=20; i<=30; i++) signal_a[i]=1; for(int i=10; i<=20; i++) signal_b[i]=1; }
        else if (cenario == 6) { for(int i=5;  i<=10; i++) signal_a[i]=1; for(int i=35; i<=40; i++) signal_b[i]=1; }
        else if (cenario == 7) { for(int i=30; i<=35; i++) signal_a[i]=1; for(int i=5;  i<=10; i++) signal_b[i]=1; }

        // --- CATEGORIA 3: DISTORÇÃO ELÁSTICA (WARPING) ---
        else if (cenario == 8)  { for(int i=10; i<=15; i++) signal_a[i]=1; for(int i=10; i<=25; i++) signal_b[i]=1; }
        else if (cenario == 9)  { for(int i=10; i<=25; i++) signal_a[i]=1; for(int i=10; i<=15; i++) signal_b[i]=1; }
        else if (cenario == 10) { for(int i=5;  i<=10; i++) signal_a[i]=1; for(int i=5;  i<=35; i++) signal_b[i]=1; }
        else if (cenario == 11) { for(int i=5;  i<=35; i++) signal_a[i]=1; for(int i=5;  i<=10; i++) signal_b[i]=1; }

        // --- CATEGORIA 4: RUÍDOS E ANOMALIAS ---
        else if (cenario == 12) { for(int i=10; i<=20; i++) signal_a[i]=1; for(int i=10; i<=20; i++) signal_b[i]=1; signal_b[2]=1; signal_b[3]=1; }
        else if (cenario == 13) { for(int i=10; i<=20; i++) signal_a[i]=1; for(int i=10; i<=20; i++) signal_b[i]=1; signal_b[42]=1; signal_b[43]=1; }
        else if (cenario == 14) { for(int i=10; i<=20; i++) signal_a[i]=1; for(int i=5;  i<=8;  i++) signal_b[i]=1; for(int i=35; i<=38; i++) signal_b[i]=1; }
        else if (cenario == 15) { for(int i=10; i<=20; i++) signal_a[i]=1; for(int i=0;  i<=9;  i++) signal_b[i]=1; for(int i=21; i<=44; i++) signal_b[i]=1; }

        // --- CATEGORIA 5: CASOS EXTREMOS DE SENSOR ---
        else if (cenario == 16) { /* Ambos vazios (0), nada a fazer */ }
        else if (cenario == 17) { for(int i=0; i<DTW_SIGNAL_SIZE; i++) { signal_a[i]=1; signal_b[i]=1; } }
        else if (cenario == 18) { for(int i=10; i<=20; i++) signal_a[i]=1; }
        else if (cenario == 19) { for(int i=10; i<=20; i++) signal_b[i]=1; }

        // ==========================================================
        // EXECUÇÃO E BENCHMARKING
        // ==========================================================
        uint32_t start_time = HAL_GetTick(); // Inicia cronômetro do Systick

        uint16_t distance = dtw_compute(signal_a, signal_b, path, &path_length);

        uint32_t end_time = HAL_GetTick();   // Para cronômetro
        uint32_t tempo_execucao_ms = end_time - start_time;

        // Pisca o LED da placa para mostrar que está processando
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

        // Imprime o resultado formatado via UART
        printf("[%02d]| %-14d | %-11d | %lu ms\r\n", cenario, distance, path_length, tempo_execucao_ms);
    }

    // Desliga o LED após terminar o lote de 20
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    printf("========================================================\r\n");
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
