#include "apt8l16.h"
#include "win6170.h"
#include "delay.h"
#include "i2c.h"
#include "74hc595.h"
#include "global.h"
#include "usart.h"

extern cur_index_t* get_cur_info;

uint8_t doorBell=0;

/*
static void APT_SDA_PIN_INPUT()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.Pin = SDA1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void APT_SDA_PIN_OUTPUT()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = SDA1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void	APT_IIC_Start()
{
	APT_SDA_HIGH;
	sys_delay_us(5);
	APT_SCL_HIGH;
	sys_delay_us(5);
	APT_SDA_LOW;
	sys_delay_us(5);
	APT_SCL_LOW;
}

void  APT_IIC_Stop()
{
	APT_SCL_LOW;
	sys_delay_us(5);
	APT_SDA_LOW;
	sys_delay_us(5);
	APT_SCL_HIGH;
	sys_delay_us(5);
	APT_SDA_HIGH;  
}

void  APT_IIC_WaitAck(void)
{
	uint8_t WaitTimeCnt=0;

	APT_SCL_HIGH;
	APT_SDA_PIN_INPUT();
	
	do{
		sys_delay_us(5);
	  WaitTimeCnt++;
	}while((APT_SDA_STATUS) && (WaitTimeCnt<5));
	
	APT_SCL_LOW;
	APT_SDA_PIN_OUTPUT();
}

void  APT_IIC_SendACK(uint8_t Ack)
{
	if(Ack==1)
		APT_SDA_LOW;
	else
		APT_SDA_HIGH;
	
	sys_delay_us(5);
	APT_SCL_HIGH;
	sys_delay_us(5);
	APT_SCL_LOW;
}

void  APT_IIC_Write8bit(uint8_t WrData)
{
	uint8_t  WrDataCnt=0;

	for(WrDataCnt=0;WrDataCnt<8;WrDataCnt++)
	{
		if((WrData<<WrDataCnt)&0x80)
			APT_SDA_HIGH;
		else
			APT_SDA_LOW;
		
		sys_delay_us(5);
		APT_SCL_HIGH;
		sys_delay_us(5);
		APT_SCL_LOW;
	}
}

uint8_t  APT_IIC_Read8bit(void)
{
	uint8_t  RdDataCnt=0,RdData=0;

	APT_SDA_PIN_INPUT();
	
	for(RdDataCnt=0;RdDataCnt<8;RdDataCnt++)
	{
		APT_SCL_HIGH;
		sys_delay_us(5);
		 
		RdData<<=1;
		if(APT_SDA_STATUS)
		  RdData |= 0x01;
		
		APT_SCL_LOW;
		sys_delay_us(5);
	}
	
	APT_SDA_PIN_OUTPUT();
	
	return(RdData);
}

void  APT_WriteByte(uint8_t APTWrAdd,uint8_t APTWrData)
{
	APT_IIC_Start();
	APT_IIC_Write8bit(0xae);
	APT_IIC_WaitAck();
	APT_IIC_Write8bit(APTWrAdd);
	APT_IIC_WaitAck();
	APT_IIC_Write8bit(APTWrData);
	APT_IIC_WaitAck();
	APT_IIC_Stop();
}
*/

KEY_STATE get_cur_number;

static uint8_t key_map[]={8,12,10,0,7,8,4,9,11,5,6,3,2,1,1}; 

void  APT_WriteChipReg(uint8_t ChipRegAdd,uint8_t ChipRegValueA)
{
	HAL_StatusTypeDef reValue;
	reValue = HAL_I2C_Mem_Write(&hi2c1,WRITE_ADDR,ChipRegAdd,1,&ChipRegValueA,1,100);
	if(reValue==HAL_OK)
	{
		//debug_printf("write sucessful %d\r\n",reValue);
	}
	else
	{
		HAL_I2C_Mem_Write(&hi2c1,WRITE_ADDR,ChipRegAdd,1,&ChipRegValueA,1,10);
		debug_printf("write failed %d\r\n",reValue);
	}
//	uint8_t  ChipRegValueB=0,WaitTimeCnt=0;

//	do{
//		APT_WriteByte(ChipRegAdd,ChipRegValueA);
//		sys_delay_us(5);
//		ChipRegValueB=APT_ReadChipReg(ChipRegAdd);
//		
//		if(ChipRegValueB!=ChipRegValueA)
//		{
//			sys_delay_us(5);
//			WaitTimeCnt++;
//		}
//	}while(ChipRegValueB!=ChipRegValueA);
}

uint8_t  APT_ReadChipReg(uint8_t APTRdAdd)
{
	uint8_t data;
	HAL_StatusTypeDef reValue;
	
	reValue = HAL_I2C_Mem_Read(&hi2c1,READ_ADDR,APTRdAdd,1,&data,1,1000);
	if(reValue==HAL_OK)
	{
		//debug_printf("read sucessful %d\n",reValue);
	}
	else
	{
		//debug_printf("read failed %d\n",reValue);
	}
	return data;
	
//	uint8_t APTRdData=0;

//	APT_IIC_Start();
//	APT_IIC_Write8bit(0xae);//1010 1100 APT????,? 
//	APT_IIC_WaitAck();
//	APT_IIC_Write8bit(APTRdAdd);//APT???????
//	APT_IIC_WaitAck();
//	
//	APT_IIC_Start();
//	APT_IIC_Write8bit(0xAF);//1010 1101 APT????,?
//	APT_IIC_WaitAck();
//	APTRdData=APT_IIC_Read8bit();//???
//	
//	APT_IIC_SendACK(0);//???
//	APT_IIC_Stop();
//	
//	return APTRdData;
}

void APT_ChipRegInit(){
	//debug_printf("-----------APT init-------\r\n");
	
	APT_WriteChipReg(0x3A,0x5a);
	
	APT_WriteChipReg(0x23,0x00);
	APT_WriteChipReg(0x24,0xe0);
	APT_WriteChipReg(0x25,0x00);
	APT_WriteChipReg(0x26,0x08);
	APT_WriteChipReg(0x27,0x02);
	APT_WriteChipReg(0x28,0x02);
	APT_WriteChipReg(0x29,0x10);
	APT_WriteChipReg(0x2A,0x10);
	APT_WriteChipReg(0x2B,0x04);
	//APT_WriteChipReg(0x2C,0x00);
	APT_WriteChipReg(0x2D,0x00);
	 
	APT_WriteChipReg(0x3A,0x00);
}

void APT_ChipRegSet()
{
	APT_ChipRegInit();
	
	sys_delay_us(5000);
	
	APT_WriteChipReg(0x3A,0x5a);

	APT_WriteChipReg(0x21,0x00);
	APT_WriteChipReg(0x22,0x00);
	APT_WriteChipReg(0x2C,0x00);


	// 禁止未使用按键 测试板
	APT_WriteChipReg(0x24,0xe0);
	

#if 0
	// 调试阶段，灵敏度自适应
	APT_WriteChipReg(0x2A,0xff);
	APT_WriteChipReg(0x2D,0x08);
#else
	
	// 全局灵敏度
	APT_WriteChipReg(0x20,0x02);
	
	// 配置灵敏度寄存器，k00~k17 阀值
	APT_WriteChipReg(0x00,0x08);
	APT_WriteChipReg(0x01,0x06);
	APT_WriteChipReg(0x02,0x06);
	APT_WriteChipReg(0x03,0x04);
	APT_WriteChipReg(0x04,0x04);
	APT_WriteChipReg(0x05,0x04);
	APT_WriteChipReg(0x06,0x04);
	APT_WriteChipReg(0x07,0x0a);
	
	APT_WriteChipReg(0x08,0x04);
	APT_WriteChipReg(0x09,0x04);
	APT_WriteChipReg(0x0A,0x04);
	APT_WriteChipReg(0x0B,0x02);
	APT_WriteChipReg(0x0C,0x02);
	
	APT_WriteChipReg(0x0D,0XFF);
	APT_WriteChipReg(0x0E,0XFF);
	APT_WriteChipReg(0x0F,0XFF);

#endif

	APT_WriteChipReg(0x3A,0x00);
	
}
void APT_Init()
{
	APT_ChipRegSet();
}

void  APT_ScanTouchKey()
{
		
	uint8_t	APTTouchKeyValue0=0,APTTouchKeyValue1=0;
	uint8_t aptcnt;
	
	//uint8_t Temp=0;
	
	//HAL_NVIC_DisableIRQ(EXTI3_IRQn);
	
//  Temp=APT_ReadChipReg(0x2C);	
//	
//  if(!(Temp&0x08))
//  {
//		debug_printf("unpower \r\n");
//		APT_ChipRegSet();
//		return;
//  }
	
  APTTouchKeyValue0=APT_ReadChipReg(0x34);
  APTTouchKeyValue1=APT_ReadChipReg(0x35);
	
	//debug_printf("APTTouchKeyValue0:%x  APTTouchKeyValue1:%x\r\n",APTTouchKeyValue0,APTTouchKeyValue1);

	if(APTTouchKeyValue0!=0)
	{
		for(aptcnt=0;aptcnt<8;aptcnt++)
		{
				if((APTTouchKeyValue0>>aptcnt)&0x01)
				{
						if(APTTouchKeyValue0==0x01)
						{
							HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET); //门铃
							set_win6170(0xef);
							sys_delay_us(2000);
							set_win6170(4);
							sys_delay_us(800000);
							set_win6170(get_cur_info->cur_vol_size);
							HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
							doorBell=1;
							break;
						}
						set_win6170(0x00);
						get_cur_number.key_prees=1;
						get_cur_number.number=key_map[aptcnt+1];
						//debug_printf("get_cur_number.number:%d\r\n",get_cur_number.number);
						
						break;
				}
		}
	}	
	else if(APTTouchKeyValue1!=0)
	{
		for(aptcnt=0;aptcnt<8;aptcnt++)
		{
				if((APTTouchKeyValue1>>aptcnt)&0x01)
				{
						set_win6170(0x00);
						get_cur_number.key_prees=1;
						get_cur_number.number=key_map[aptcnt+9];
						//debug_printf("get_cur_number.number:%d \r\n",get_cur_number.number);
						break;
				}
		}
	}
	else
	{
		debug_printf("key unpress \r\n");
	}
	return ;
	// HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

// 使能按键通道
void APT_KeyEnableSel(uint8_t kdr0,uint8_t kdr1)
{
	APT_WriteChipReg(0x3A,0x5a);
	APT_WriteChipReg(0x23,kdr0);
	APT_WriteChipReg(0x24,kdr1);
	APT_WriteChipReg(0x3A,0x00);
}

void APT_WORK_125HZ_VAL(void)
{
		//APT 降低工作频
	APT_WriteChipReg(0x3A,0x5a);
	APT_WriteChipReg(0x21,0x02);

	// 使能按键通道
	APT_WriteChipReg(0x23,0x00);
	APT_WriteChipReg(0x24,0xe0);
	
	// 全局灵敏度
	APT_WriteChipReg(0x20,0x01);
	
	// 配置灵敏度寄存器，k00~k17 阀值
	APT_WriteChipReg(0x00,0x04);
	APT_WriteChipReg(0x01,0x07);
	APT_WriteChipReg(0x02,0x04);
	APT_WriteChipReg(0x03,0x04);
	APT_WriteChipReg(0x04,0x04);
	APT_WriteChipReg(0x05,0x03);
	APT_WriteChipReg(0x06,0x04);
	APT_WriteChipReg(0x07,0x06);
	
	APT_WriteChipReg(0x08,0x03);
	APT_WriteChipReg(0x09,0x02);
	APT_WriteChipReg(0x0A,0x02);
	APT_WriteChipReg(0x0B,0x02);
	APT_WriteChipReg(0x0C,0x03);
	
	APT_WriteChipReg(0x0D,0XFF);
	APT_WriteChipReg(0x0E,0XFF);
	APT_WriteChipReg(0x0F,0XFF);

	APT_WriteChipReg(0x3A,0x00);
}

