/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define VWIN6_Pin GPIO_PIN_13
#define VWIN6_GPIO_Port GPIOC
#define RESET_Pin GPIO_PIN_2
#define RESET_GPIO_Port GPIOC
#define VOLED_Pin GPIO_PIN_1
#define VOLED_GPIO_Port GPIOA
#define pwm1_Pin GPIO_PIN_5
#define pwm1_GPIO_Port GPIOC
#define pwm2_Pin GPIO_PIN_0
#define pwm2_GPIO_Port GPIOB
#define FPReset_Pin GPIO_PIN_1
#define FPReset_GPIO_Port GPIOB
#define FPPower_Pin GPIO_PIN_2
#define FPPower_GPIO_Port GPIOB
#define SPI2_CSS_Pin GPIO_PIN_12
#define SPI2_CSS_GPIO_Port GPIOB
#define SRCLK_Pin GPIO_PIN_6
#define SRCLK_GPIO_Port GPIOC
#define RCLK_Pin GPIO_PIN_7
#define RCLK_GPIO_Port GPIOC
#define SER_Pin GPIO_PIN_8
#define SER_GPIO_Port GPIOC
#define FPINT_Pin GPIO_PIN_9
#define FPINT_GPIO_Port GPIOC
#define FPINT_EXTI_IRQn EXTI9_5_IRQn
#define VLED_Pin GPIO_PIN_11
#define VLED_GPIO_Port GPIOA
#define WAKEUP_NFC_Pin GPIO_PIN_12
#define WAKEUP_NFC_GPIO_Port GPIOA
#define WAKEUP_NFC_EXTI_IRQn EXTI15_10_IRQn
#define WIFI_POWER_Pin GPIO_PIN_12
#define WIFI_POWER_GPIO_Port GPIOC
#define restore_key_Pin GPIO_PIN_2
#define restore_key_GPIO_Port GPIOD
#define restore_key_EXTI_IRQn EXTI2_IRQn
#define WKTOUCH_Pin GPIO_PIN_3
#define WKTOUCH_GPIO_Port GPIOB
#define WKTOUCH_EXTI_IRQn EXTI3_IRQn
#define AUDBusy_Pin GPIO_PIN_8
#define AUDBusy_GPIO_Port GPIOB
#define AUDData_Pin GPIO_PIN_9
#define AUDData_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* Base address of the Flash sectors */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
