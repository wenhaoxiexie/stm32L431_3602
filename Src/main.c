
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "adc.h"
#include "i2c.h"
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "74hc595.h"
#include "ssd1305.h"
#include "win6170.h"
#include "delay.h"
#include "apt8l16.h"
#include "stmflash.h"
#include "stdlib.h"
#include "menu.h"
#include "global.h"
#include "flash.h"
#include "fpcst90x.h"
#include "wifi_8266.h"
#include "nfc.h"
#include "nfc9883.h"
#include "hsj08.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;

extern KEY_STATE get_cur_number;
uint8_t finger_press_flag=0x00;

uint8_t usart3_rx[1];
uint8_t usart3_rx_buf[USART3_REC_LEN];
uint16_t usart3_rx_len=0;

uint8_t usart2_rx[1];
uint8_t usart2_rx_buf[USART2_REC_LEN];
uint16_t usart2_rx_len=0;

wifi_upload_store_t wifi_upload[100];

uint32_t battery_value=0;
int8_t battery_class=100;
uint8_t try_count=0;
uint8_t facteryReset=0;
uint8_t prying_flag=0;
uint8_t pryingUpload_flag=0;
uint8_t alarmUploadFlag=0;
uint8_t stopModeFlag=0;
uint32_t hjsReverse=0;
extern uint8_t doorBell;
uint8_t factoryFlag=0;
uint8_t powerOffFlag=0;
extern void SYSCLKConfig_STOP(uint16_t GPIO_Pin);

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t permision;
	uint32_t delay_count;

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_RTC_Init();
  MX_USART2_UART_Init();
  MX_I2C3_Init();
  MX_TIM2_Init();
  MX_IWDG_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	multi_mode_init();
	
	debug_printf("IWDG start \r\n");
	
	// 开启定时器6
	HAL_TIM_Base_Start_IT(&htim6);
	
	/* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Ensure that MSI is wake-up system clock */ 
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		
	debug_printf(" systerm ... \r\n");

	MAIN_LOOP:
	
		delay_count=50000;
	
		do{
				if(get_cur_number.key_prees||finger_press_flag||nfc_temp.irq_flag)
				{
					delay_count=50000;
					if(get_cur_number.key_prees&&get_cur_number.number==10)
						break;
				}
				if(keyboard_login(&permision)==LOCK_OK||finger_login(&permision)==LOCK_OK||card_login(&permision)==LOCK_OK)
				{
					// 防止其他方式开门
					HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
					HAL_NVIC_DisableIRQ(EXTI4_IRQn);
					//HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
					if(permision==ANDMI)
					{
						send_bytes(KEY10_11_ON);
						APT_KeyEnableSel(KDR0_CH_10_11_EN,KDR1_CH_10_11_EN);
						delay_count=8000;
						do{
							if(get_cur_number.key_prees==1)
							{
								delay_count=8000;
								get_cur_number.key_prees=0;
								if(get_cur_number.number==11)
								{
									stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
									// 第一个用户只能是管理员身份
									if(get_cur_info->cur_index_number<1)
									{
										//HAL_NVIC_DisableIRQ(EXTI4_IRQn);
										user_add_menu_sub(ANDMI);
									}else{
										if(main_menu()==LOCK_OUTTIME)
										{
											break;
										}
									}
								}
								if(get_cur_number.number==10)
								{
									send_bytes(KEY_ALL_OFF);
									display_menu();
									APT_KeyEnableSel(KDR0_CH_ALL_DISA,KDR1_CH_ALL_DISA);
									break;
								}
							}
							sys_delay_us(1000);
						}while(--delay_count);
					}else{
						send_bytes(KEY_ALL_OFF);
						APT_KeyEnableSel(KDR0_CH_ALL_DISA,KDR1_CH_ALL_DISA);
						sys_delay_us(5000000);
						if(nfc_temp.irq_flag==1)
							nfc_temp.irq_flag=0;
						// 清除中断位
						__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);

						// 开启TOUCH 中断
						HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
						
						fp_power_on();
						fp_sleep();
						
						break;
					}
					if(nfc_temp.irq_flag==1)
						nfc_temp.irq_flag=0;
					
					// 清除中断位
					__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);

					// 开启TOUCH 中断
					HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
					
					fp_power_on();
					fp_sleep();
				}
				sys_delay_us(100);
		}while(delay_count--);
		
		if(prying_flag|doorBell)
		{
			if(doorBell)
				doorBell=0;
			if(prying_flag&&pryingUpload_flag)
			{
				pryingUpload_flag=0;
				sys_delay_us(5000000);
				//wifi_upload_alarm_info();
				wifi_upload_all_info();
			}
			goto MAIN_LOOP;
		}
		sleep_mode();
  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Configure LSE Drive Capability 
    */
  HAL_PWR_EnableBkUpAccess();

  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_LSE
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C3
                              |RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 16;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /**Enable MSI Auto calibration 
    */
  HAL_RCCEx_EnableMSIPLLMode();

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint8_t delay_count=5;
	uint8_t pryStatus;
	
	if(stopModeFlag)
	{
		stopModeFlag=0;
		SYSCLKConfig_STOP(GPIO_Pin);
	}
	if(GPIO_Pin==GPIO_PIN_2)
	{
		debug_printf(" ################  RESTORE interrupt... \r\n");
		do{
			sys_delay_us(1000000);
			delay_count--;
		}while(HAL_GPIO_ReadPin(GPIOD,restore_key_Pin)==0&&delay_count>0);
		if(delay_count==0)
		{
			factory_reset();
			debug_printf(" ################  restore factory... \r\n");
		}
	}
	else if(GPIO_Pin == GPIO_PIN_3)
  {
		debug_printf(" ################  APT interrupt... \r\n");
		APT_ScanTouchKey();	
  }
	else if(GPIO_Pin == GPIO_PIN_4)
	{
		HAL_NVIC_DisableIRQ(EXTI4_IRQn);
		sys_delay_us(10000);
		pryStatus =HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4);
		debug_printf(" ################## pryStatus:%d \r\n",pryStatus);
		if(pryStatus)
		{
				prying_flag=0;
				set_win6170(0XFE);
				sys_delay_us(2000);
				set_win6170(get_cur_info->cur_vol_size);
				sys_delay_us(2000);
		}else{
				if(get_cur_info->cur_index_number>=1||factoryFlag)
				{
					wifi_power_on();
					set_win6170(0XEF);
					sys_delay_us(2000);
					set_win6170(3);
					sys_delay_us(2000);
					set_win6170(0XF2);
					sys_delay_us(2000);
					alarmUploadFlag=1;
					prying_flag=1;
					pryingUpload_flag=1;
					wifi_upload_alarm_info();
				}
		}
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
		HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	}
	else if(GPIO_Pin == GPIO_PIN_9)
	{
		debug_printf("detect finger is press \r\n");
		
		finger_press_flag=1;
		fp_power_on();
		
		set_win6170(5);
		if(facteryReset)
			return;
	
	}
	else if(GPIO_Pin == GPIO_PIN_12)
	{
		if(facteryReset)
			return;
		set_win6170(get_cur_info->cur_vol_size);
		debug_printf(" ################## detect NFC is coming \r\n");
		detect_card();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==htim2.Instance)
	{
		//debug_printf(" time2 arrived.... \r\n");
		LOCK();
		HAL_TIM_Base_Stop_IT(&htim2);
	}
	else if(htim->Instance==htim6.Instance)
	{
		//debug_printf(" update  hiwdg ...\r\n");
		HAL_IWDG_Refresh(&hiwdg);
		
	}
	else if(htim->Instance==htim7.Instance)
	{
		debug_printf(" hsj_drv test... \r\n");
		
		hjsReverse++;
		
		if(factoryFlag==0)
			display_factory_hjs_number(hjsReverse);
		
		if(hjsReverse%2)
			LOCK();
		else
			UNLOCK();
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==&huart2)
	{
		if(usart2_rx_len<USART2_REC_LEN)
		{
			usart2_rx_buf[usart2_rx_len++]=usart2_rx[0];
		}
	}
	else if(huart==&huart3)
	{	 
		if(usart3_rx_len<USART3_REC_LEN)
		{
			usart3_rx_buf[usart3_rx_len++]=usart3_rx[0];
		}
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(0)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
