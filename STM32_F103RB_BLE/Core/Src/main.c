/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFER_SIZE 100 // Define the size of the buffer
#define BLEBUF_SIZE 40 // Define the size of the buffer
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint32_t gBLECnt 	= 0;       // Counter for BLE messages
static uint32_t gGPIOState 	= 0;       // State of the GPIO pin
uint8_t rxIndex 			= 0;       // Current index for the buffer
char rxBuffer[BUFFER_SIZE]; 		   // Buffer for received data
char messageBuffer[BLEBUF_SIZE];
uint32_t messageIndex = 0;
uint8_t bBLEFlag =0;
extern UART_HandleTypeDef huart1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
typedef struct UserInfo
{
    float fCurrent;
    uint32_t iDuty;
    uint32_t iFrequency;
} UserInfo;

UserInfo userinfo = {0.0,0,0};
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
UserInfo parseMessage(char *messageBuffer) {
    // Find the first occurrence of 'S' and 'E'
    char *start = strchr(messageBuffer, 'S');
    char *end = strchr(messageBuffer, 'E');

    // Check if both 'S' and 'E' exist and their positions are valid
    if (start != NULL && end != NULL && start < end) {
        bBLEFlag = 1;
    } else {
        bBLEFlag = 0;
        return; // Exit the function if 'S' and 'E' are not valid
    }

    if (bBLEFlag == 1) {
        // Parse Current value between 'c' and 'c'
        char *startCurrent = strchr(start, 'c');
        char *endCurrent = strchr(startCurrent + 1, 'c');
        if (startCurrent != NULL && endCurrent != NULL && endCurrent < end) {
            char currentStr[10] = {0};
            strncpy(currentStr, startCurrent + 1, endCurrent - startCurrent - 1);
            userinfo.fCurrent = atof(currentStr);
        }

        // Parse Duty Cycle value between 'd' and 'd'
        char *startDuty = strchr(start, 'd');
        char *endDuty = strchr(startDuty + 1, 'd');
        if (startDuty != NULL && endDuty != NULL && endDuty < end) {
            char dutyStr[10] = {0};
            strncpy(dutyStr, startDuty + 1, endDuty - startDuty - 1);
            userinfo.iDuty = (uint32_t)atoi(dutyStr);
        }

        // Parse Frequency value between 'f' and 'f'
        char *startFreq = strchr(start, 'f');
        char *endFreq = strchr(startFreq + 1, 'f');
        if (startFreq != NULL && endFreq != NULL && endFreq < end) {
            char freqStr[10] = {0};
            strncpy(freqStr, startFreq + 1, endFreq - startFreq - 1);
            userinfo.iFrequency = (uint32_t)atoi(freqStr);
        }
    }
    return userinfo;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, (uint8_t *)&rxBuffer[rxIndex], 1);
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

      /* USER CODE END WHILE */

      /* USER CODE BEGIN 3 */
	  UserInfo tempInfo = parseMessage(messageBuffer);

      switch (gGPIOState)
      {
          case 1:
              HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
              break;
          case 2:
              HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
              break;
          default:
              break;
      }
      HAL_Delay(500);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
	{ 	// Check if it's the correct UART instance
        gBLECnt++;  // Increment the BLE counter

        // Process the received character
        switch (rxBuffer[rxIndex-1])
		{
            case '1':
                gGPIOState = 1;  // Set GPIO state to high
                HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
                // Optionally, add code to toggle the GPIO pin here
                break;
            case '2':
                gGPIOState = 2;  // Set GPIO state to low
                HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
                // Optionally, add code to toggle the GPIO pin here
                break;
            default:
                // Handle unexpected messages if necessary
                break;
        }

        // Prepare to receive the next byte
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&rxBuffer[rxIndex], 1);
        // Move to the next index, wrap around if necessary
        messageIndex = (rxBuffer[rxIndex-1] == 'S') ? 0 : messageIndex;
        messageBuffer[messageIndex] = rxBuffer[rxIndex-1];

        ++messageIndex;
        ++rxIndex;
		// 2024-10-27, hjkim: Reset indexes after reaching buffer size limit
		messageIndex= (messageIndex >= BLEBUF_SIZE) ? 0 : messageIndex;
		rxIndex		= (rxIndex >= BLEBUF_SIZE) 		? 0 : rxIndex;
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
