/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include <stdio.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint32_t pwm_cnt1 = 0;
uint32_t pwm_cnt2 = 0;

static int adc_cnt = 0;

uint32_t led_max = 3999;
uint32_t led_R = 1999;
uint32_t led_G = 1999;
uint32_t led_B = 1999;

float percentage = 0.5;

int limit_up = 4000;

uint32_t LBC = 3999;

const int LBM = 7998;

void unit_delay(unsigned int t) {
  for (t; t>0 ;t--) {
  }
}
void rgb_ref(int perc) {
	if (perc>100) perc = 100;
	if (perc<0) perc = 0;
  if (perc >= 50) {
    // int rate = ((float)(100 - perc) / 50.0);
    led_R = ((float)(100 - perc) / 50.0) * led_max;
    led_G = led_max;
  }
  if (perc < 50) {
    // int rate = ((float)(perc / 50) / 50.0);
    led_G = (perc / 50.0) * led_max;
    led_R = led_max;
  }
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, led_G);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, led_R);
  // __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, led_G);  A7
}

void print(char * msg) {
  HAL_UART_Transmit(&huart1,(uint8_t *)msg,strlen(msg),HAL_MAX_DELAY);
}
void printfloat(float t){
	char str[40];
	snprintf(str, sizeof(str), "%.4f \r\n", t);
	print(str);
}
int ADC_update(void) {
  ADC_ON;
  HAL_Delay(50);
  float adc_raw;
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
  adc_raw = HAL_ADC_GetValue(&hadc1);
  float voltage = (adc_raw * 3.3f) / 4095.0f; // votage range : 3.6 - 4.2
  HAL_ADC_Stop(&hadc1);
  int perc = (((voltage - 2.456) / 0.639) * 100);
  rgb_ref(perc);
  ADC_OFF;
  return perc;
  /*char str[40];
  snprintf(str, sizeof(str), "%f V\r\n", voltage);
  print(str);*/

}
void ADC_test(void){
	ADC_ON;
	HAL_Delay(50);
	float adc_raw;
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	adc_raw = HAL_ADC_GetValue(&hadc1);
	float voltage = (adc_raw * 3.3f) / 4095.0f; // votage range : 3.6 - 4.2
	HAL_ADC_Stop(&hadc1);
	int perc = (((voltage - 2.456) / 0.639) * 100);
	char str[40];
	char vcc[40];
	snprintf(vcc, sizeof(vcc), "%.4f V\r\n", voltage);
	snprintf(str, sizeof(str), "%d V\r\n", perc);
	print("voltage : ");
	print(vcc);
	print("PERCENTAGE : ");
	print(str);
	snprintf(str, sizeof(str), "%d V\r\n", led_G);
	print("led_G   ");
	print(str);
	snprintf(str, sizeof(str), "%d V\r\n", led_R);
	print("led_R   ");
	print(str);

	//float perc = (((voltage - 3.6) / 0.6) * 100);
	//rgb_ref((int)perc);
	ADC_OFF;
	HAL_Delay(1000);
}
void print_pwm(void) {
  char str[40];
  snprintf(str, sizeof(str), "%d V\r\n", pwm_cnt1);
  print("PWM1: ");
  print(str);

  snprintf(str, sizeof(str), "%.4f V\r\n", percentage);
  print("PERCENTAGE: ");
  print(str);

  snprintf(str, sizeof(str), "%d V\r\n", pwm_cnt2);
  print("PWM2: ");
  print(str);
}
void pwm_ref(void) {
  //print_pwm();
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,pwm_cnt2);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,pwm_cnt1);
}
void dim(void) {
  while ((pwm_cnt1 == 0) && (pwm_cnt2 == 0)) {
    if ((pwm_cnt1 - 1) > 0) {
      pwm_cnt1--;
    }
    if ((pwm_cnt2 - 1) > 0) {
      pwm_cnt2--;
    }
    pwm_ref();
  }
  pwm_cnt1 = 0;
  pwm_cnt2 = 0;
  pwm_ref();
}
/* USER CODE END PV */
void pwm_show(void) {
  pwm_cnt1 = 0;
  pwm_cnt2 = 0;
  for (int i=0;i<4000;i++) {
    pwm_cnt1++;
    pwm_cnt2++;
    HAL_Delay(1);
    pwm_ref();
  }
  for (int i=0;i<4000;i++) {
    pwm_cnt1--;
    pwm_cnt2--;
    HAL_Delay(1);
    pwm_ref();
  }

}
void goto_sleep(void){
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_PWR_EnterSTANDBYMode();
}
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();


  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);

  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
  /* USER CODE BEGIN 2 */

  ADC_OFF;
  DRAIN_OFF;
  _Bool tr = 1;
  /* USER CODE END 2 */
  HAL_Delay(500);
  if (ON_CHARGE){
	  DRAIN_ON;
	  HAL_Delay(1000);
	  while(1){
		  if(OFF_CHARGE){
		    break;
	 	  }
		  DRAIN_ON;
		  ADC_update();
  	 	  HAL_Delay(3000);


  	 	  if(KEY_C){
  	 		  DRAIN_OFF;
  	 		  //break;
  	 	  }


  	  	  }

		  goto_sleep();
  }

  while(KEY_C){
  }
  if(OFF_CHARGE){

	  pwm_cnt1 = 1999;
	  pwm_cnt2 = 1999;
		  pwm_ref();
	  ADC_update();
  }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  pwm_show();
	  //ADC_test();
	  if (KEY_2) {
		  HAL_Delay(10);
		  while (KEY_2) {
			  if (LBC < LBM && (pwm_cnt1 <= limit_up) && (pwm_cnt2 <= limit_up)) {
				  LBC++;
				  pwm_cnt1 = percentage * LBC;
				  pwm_cnt2 = LBC -  pwm_cnt1;
	          }
			  unit_delay(900);
	          pwm_ref();
	        }
	      }
	  if (KEY_3) {
	        HAL_Delay(10);
	        while (KEY_3) {
	          if (LBC > 0 && (pwm_cnt1 >= 0) && (pwm_cnt2 >= 0)) {
	            LBC--;
	          pwm_cnt1 = percentage * LBC;
	          pwm_cnt2 = LBC -  pwm_cnt1;
	          }
	          unit_delay(900);
	          pwm_ref();
	        }
	      } //            blue     red
	  if (KEY_1) { // p1 ++   p2 -- ;
	        HAL_Delay(10);
	        while (KEY_1) {
	          if ((percentage <= 0.99) && (pwm_cnt1 >= 0) && (pwm_cnt1 <= limit_up)
	            && (pwm_cnt2 >= 0) && (pwm_cnt2 <= limit_up)) {
	            percentage = percentage + 0.01;
	            pwm_cnt1 = percentage * LBC;
	            if ((LBC -  pwm_cnt1) <= 1) {
	              pwm_cnt2 = 0;
	            }
	            else pwm_cnt2 = LBC -  pwm_cnt1;
	          }
	          else if (pwm_cnt2 >= limit_up) {
	            while (KEY_1 && (pwm_cnt2 >= 0)) {
	              pwm_cnt1++;
	              pwm_cnt2--;
	              percentage = percentage + (1.00/(float)limit_up);
	              HAL_Delay(1);
	              pwm_ref();
	            }
	            percentage = (float)pwm_cnt1 / (float)LBC;
	          }
	          else if (pwm_cnt1 >= limit_up) {
	            while (KEY_1 && (pwm_cnt2 > 0)) {
	              if ((pwm_cnt2 - 1) > 0) {
	                pwm_cnt2--;
	              }
	              HAL_Delay(1);
	              pwm_ref();
	            }
	            percentage = (float)pwm_cnt1 / (float)LBC;
	          }
	          pwm_ref();
	          HAL_Delay(17);
	        }
	      }
	      //             blue     red
	  if (KEY_4) {// p1 --   p2 ++ ;
	        HAL_Delay(10);
	        while (KEY_4) {
	          if ((percentage >= 0.01) && (pwm_cnt1 >= 0) && (pwm_cnt2 >= 0)
	            && (pwm_cnt2 < limit_up)) {
	            percentage = percentage - 0.01;
	            pwm_cnt1 = percentage * LBC;
	            if ((LBC -  pwm_cnt1) <= 1) {
	              pwm_cnt2 = 0;
	            }
	            else pwm_cnt2 = LBC -  pwm_cnt1;
	          }
	          else if (pwm_cnt2 >= limit_up && pwm_cnt1 >= 1) {
	            pwm_cnt2 = limit_up;
	            if ((pwm_cnt1 - 15) > 15) {
	              pwm_cnt1 = pwm_cnt1 - 15;
	            }
	            percentage = (float)pwm_cnt1 / (float)LBC;
	          }
	          pwm_ref();
	          HAL_Delay(17);
	        }
	      }

	  //////////////////////////////////////////

	  if (KEY_C){
		  HAL_Delay(400);
			  if (KEY_C){
				  DRAIN_OFF;
				  dim();
				  HAL_Delay(500);
				  goto_sleep();
			  }
		  }

		 //if(OFF_CHARGE){
	     // }
/*
		  if (ON_CHARGE){
	        DRAIN_ON;
	        dim();
	        //NVIC_SystemReset();
	        while (1) {
	          HAL_Delay(10);
	          adc_cnt++;
	          if (adc_cnt >= 3000) {
	            adc_cnt = 0;
	            ADC_update();
	          }
	        }
	      }*/


	  //////////////////////////////////////////////

	      HAL_Delay(10);
	      adc_cnt++;
	      if (adc_cnt >= 3000) {
	        adc_cnt = 0;
	        if(ADC_update()==0){
	        	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,3999);
	        	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,0);
	        	for(int i=0;i==5;i++){
	        		HAL_Delay(500);
	        		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,5);
	        		HAL_Delay(500);
	        		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,3999);
	        	}
	        	dim();
	        	goto_sleep();

	        }
	      }
	      if (OFF_CHARGE) {
	        DRAIN_OFF;
	      }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV16;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 3999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 3999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA2 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_12|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA3 PA4 PA5 PA6
                           PA8 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
