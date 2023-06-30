/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_EN_Pin GPIO_PIN_0
#define LED1_EN_GPIO_Port GPIOA
#define LED2_EN_Pin GPIO_PIN_1
#define LED2_EN_GPIO_Port GPIOA
#define LED3_EN_Pin GPIO_PIN_2
#define LED3_EN_GPIO_Port GPIOA
#define Break_CS_Pin GPIO_PIN_3
#define Break_CS_GPIO_Port GPIOA
#define ADC_SDI_Pin GPIO_PIN_7
#define ADC_SDI_GPIO_Port GPIOA
#define Throttle_CS_Pin GPIO_PIN_4
#define Throttle_CS_GPIO_Port GPIOC
#define Board_Ok_Pin GPIO_PIN_5
#define Board_Ok_GPIO_Port GPIOC
#define Error_Pin GPIO_PIN_0
#define Error_GPIO_Port GPIOB
#define INT_IMU_Pin GPIO_PIN_2
#define INT_IMU_GPIO_Port GPIOB
#define HORN_EN_Pin GPIO_PIN_12
#define HORN_EN_GPIO_Port GPIOB
#define LED5_EN_Pin GPIO_PIN_13
#define LED5_EN_GPIO_Port GPIOB
#define LED6_EN_Pin GPIO_PIN_14
#define LED6_EN_GPIO_Port GPIOB
#define Break_EN_Pin GPIO_PIN_7
#define Break_EN_GPIO_Port GPIOC
#define LED4_EN_Pin GPIO_PIN_9
#define LED4_EN_GPIO_Port GPIOC
#define Throttle_Switch_Pin GPIO_PIN_3
#define Throttle_Switch_GPIO_Port GPIOB
#define Aux_C2_Pin GPIO_PIN_4
#define Aux_C2_GPIO_Port GPIOB
#define Aux_C3_Pin GPIO_PIN_5
#define Aux_C3_GPIO_Port GPIOB
#define Aux_C4_Pin GPIO_PIN_6
#define Aux_C4_GPIO_Port GPIOB
#define Breaks_Pin GPIO_PIN_7
#define Breaks_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define HLR_GPIO_Pin LED4_EN_Pin
#define HLR_GPIO_Port LED4_EN_GPIO_Port
#define HLL_GPIO_Pin LED1_EN_Pin
#define HLL_GPIO_Port LED1_EN_GPIO_Port
#define BRK_GPIO_Pin LED4_EN_Pin
#define BRK_GPIO_Port LED4_EN_GPIO_Port
#define LT_GPIO_Pin LED2_EN_Pin
#define LT_GPIO_Port LED2_EN_GPIO_Port
#define RT_GPIO_Pin LED5_EN_Pin
#define RT_GPIO_Port LED5_EN_GPIO_Port
#define FI_GPIO_Pin  LED6_EN_Pin
#define FI_GPIO_Port LED6_EN_GPIO_Port
#define HRN_GPIO_Pin  HORN_EN_Pin
#define HRN_GPIO_Port HORN_EN_GPIO_Port
#define LHeadlight_GPIO_Port LED3_EN_GPIO_Port
#define LHeadlight_Pin LED3_EN_Pin
#define RHeadlight_GPIO_Port LED6_EN_GPIO_Port
#define RHeadlight_Pin LED6_EN_Pin
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
