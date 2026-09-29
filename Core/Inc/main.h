/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */



/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define ADC_ON HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,1)
#define ADC_OFF HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,0)
#define DRAIN_ON HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,1)
#define DRAIN_OFF HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,0)
/* USER CODE END ET */
#define KEY_1 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3) //right
#define KEY_2 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) // up
#define KEY_3 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5) // down
#define KEY_4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) //left
#define KEY_C HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)
#define ON_CHARGE (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)==0) || (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15)==0)
#define OFF_CHARGE (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)) && (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15))

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
