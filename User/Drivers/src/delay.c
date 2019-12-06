#include "delay.h"

static uint8_t fac_us=0;							//us延时倍乘数
static uint16_t fac_ms=0;

void delay_init(uint8_t SYSCLK)
{
  //HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTick频率为HCLK
	fac_us=SYSCLK;		
	fac_ms=(uint16_t)fac_us*1000;		
}

void sys_delay_us(uint32_t nus)
{		
	uint8_t fac_us=80;
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};
}

void sys_delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)nms*fac_ms;				
	SysTick->VAL =0x00;							
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	
	SysTick->VAL =0X00;       					    
}
