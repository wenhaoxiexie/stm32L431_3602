#include "hsj08.h"
#include "delay.h"
#include "global.h"

uint8_t reValue;
void LOCK(void)
{
	 HAL_GPIO_WritePin(pwm1_GPIO_Port, pwm1_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(pwm2_GPIO_Port, pwm2_Pin, GPIO_PIN_RESET);
	 sys_delay_us(150000);
	
	 HAL_GPIO_WritePin(pwm1_GPIO_Port, pwm1_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(pwm2_GPIO_Port, pwm2_Pin, GPIO_PIN_RESET);
}

void UNLOCK(void)
{
	 HAL_GPIO_WritePin(pwm1_GPIO_Port, pwm1_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(pwm2_GPIO_Port, pwm2_Pin, GPIO_PIN_SET);
	 sys_delay_us(150000);
	 HAL_GPIO_WritePin(pwm1_GPIO_Port, pwm1_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(pwm2_GPIO_Port, pwm2_Pin, GPIO_PIN_RESET);
}

void hsj08_exit(void)
{
	 HAL_GPIO_WritePin(pwm1_GPIO_Port, pwm1_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(pwm2_GPIO_Port, pwm2_Pin, GPIO_PIN_RESET);
}
