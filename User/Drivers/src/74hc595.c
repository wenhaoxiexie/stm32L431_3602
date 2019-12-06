#include "74hc595.h"
#include "delay.h"

void led_powerOff(void)
{
	HAL_GPIO_WritePin(VLED_GPIO_Port,VLED_Pin,GPIO_PIN_RESET);
}

void led_powerOn(void)
{
	HAL_GPIO_WritePin(VLED_GPIO_Port,VLED_Pin,GPIO_PIN_SET);
}

void send_bytes(uint16_t data)
{	
	uint16_t i;
	data=~data;
	HAL_GPIO_WritePin(RCLK_GPIO_Port, RCLK_Pin, GPIO_PIN_RESET);
	for(i=0;i<16;i++)
	{
		HAL_GPIO_WritePin(SRCLK_GPIO_Port, SRCLK_Pin, GPIO_PIN_RESET);
		if(data&0x0001)
			HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, GPIO_PIN_RESET);
		data>>=1;
		HAL_GPIO_WritePin(SRCLK_GPIO_Port, SRCLK_Pin, GPIO_PIN_SET);
		//__nop();
		sys_delay_us(1000);
	}
	HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, GPIO_PIN_RESET);
	sys_delay_us(1000);
	HAL_GPIO_WritePin(SRCLK_GPIO_Port, SRCLK_Pin, GPIO_PIN_RESET);
	sys_delay_us(1000);
	HAL_GPIO_WritePin(RCLK_GPIO_Port, RCLK_Pin, GPIO_PIN_SET);
	sys_delay_us(1000);
	HAL_GPIO_WritePin(RCLK_GPIO_Port, RCLK_Pin, GPIO_PIN_RESET);
}

