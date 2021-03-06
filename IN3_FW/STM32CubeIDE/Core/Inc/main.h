/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern SPI_HandleTypeDef hspi2;

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CS_Pin GPIO_PIN_13
#define CS_GPIO_Port GPIOC
#define GSM_PWRKEY_Pin GPIO_PIN_14
#define GSM_PWRKEY_GPIO_Port GPIOC
#define TOUCH_IRQ_Pin GPIO_PIN_15
#define TOUCH_IRQ_GPIO_Port GPIOC
#define ENCODER_PULSE_Pin GPIO_PIN_5
#define ENCODER_PULSE_GPIO_Port GPIOC
#define JAUNDICE_CTL_Pin GPIO_PIN_0
#define JAUNDICE_CTL_GPIO_Port GPIOB
#define BACKUP_MOSFET_Pin GPIO_PIN_1
#define BACKUP_MOSFET_GPIO_Port GPIOB
#define POWER_EN_Pin GPIO_PIN_12
#define POWER_EN_GPIO_Port GPIOB
#define FAN_CTL_Pin GPIO_PIN_7
#define FAN_CTL_GPIO_Port GPIOC
#define ENCODER_A_Pin GPIO_PIN_8
#define ENCODER_A_GPIO_Port GPIOC
#define ENCODER_B_Pin GPIO_PIN_9
#define ENCODER_B_GPIO_Port GPIOC
#define HEATER_CTL_Pin GPIO_PIN_8
#define HEATER_CTL_GPIO_Port GPIOA
#define SD_CS_Pin GPIO_PIN_11
#define SD_CS_GPIO_Port GPIOA
#define RST_Pin GPIO_PIN_2
#define RST_GPIO_Port GPIOD
#define PWR_ALERT_Pin GPIO_PIN_4
#define PWR_ALERT_GPIO_Port GPIOB
#define PULSIOXIMETER_EN_Pin GPIO_PIN_5
#define PULSIOXIMETER_EN_GPIO_Port GPIOB
#define TFT_LED_CTL_Pin GPIO_PIN_6
#define TFT_LED_CTL_GPIO_Port GPIOB
#define HUMIDIFIER_CTL_Pin GPIO_PIN_7
#define HUMIDIFIER_CTL_GPIO_Port GPIOB
#define DC_Pin GPIO_PIN_8
#define DC_GPIO_Port GPIOB
#define TOUCH_CS_Pin GPIO_PIN_9
#define TOUCH_CS_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
