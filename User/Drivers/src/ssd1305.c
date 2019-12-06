#include "math.h"
#include "ssd1305.h"
#include "code_lib.h"
#include "flash.h"
#include "usart.h"
#include "global.h"
#include "wifi_8266.h"
#include "i2c.h"


uint8_t OLED_GRAM[128][8];	 
uint8_t OLED_GRAM_BAK[128][8];
	
extern int8_t battery_class;
//extern RTC_DateTypeDef sdatestructure;
//extern RTC_TimeTypeDef stimestructure;

uint8_t OP_Command[2];
uint8_t OP_Data[2];

uint8_t OP_Data[2];
uint8_t	OP_Data[2];

#if 0
/**
  * @brief  Set SDA In.
  * @param  None
  * @retval None
  */
static void SDA_IN()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.Pin = SDA_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(SDA_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  Set SDA Out.
  * @param  None
  * @retval None
  */
static void SDA_OUT()
{	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = SDA_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(SDA_GPIO_PORT, &GPIO_InitStruct);
	
}

/**********************************************
//
//IIC??????
//
**********************************************/

void IC_IIC_Start()
{
	SDA_OUT(); 
	SET_SDA();	 
	SET_CLK();
	__nop();
	__nop();
	
	CLR_SDA();
	__nop();
	__nop();
	CLR_CLK();

}

/**********************************************
//
//IIC??????
//
**********************************************/

void IC_IIC_Stop()
{
	
	SDA_OUT();

	CLR_SDA();
	__nop();
	SET_CLK();
	__nop();__nop();
	SET_SDA();
}

/**********************************************
//
//?IIC?????
//????acknowledgement???
//
**********************************************/

GPIO_PinState Write_IIC_Data(uchar Data)
{
	uchar i;
	SDA_OUT(); 	    
	GPIO_PinState pin_state;
	
	for(i=0;i<8;i++)
	{
		if(Data&0x80)
			SET_SDA();
		else	
			CLR_SDA();
		Data<<=1; 
		__nop();
		SET_CLK();
		__nop();
		__nop();
		CLR_CLK();
	}
	SET_SDA();
	__nop();
	__nop();
	SDA_IN(); 	  
	SET_CLK();
	__nop();
	__nop();
	pin_state =READ_SDA();
	CLR_CLK();
	
	return pin_state;
}

#endif 
/**********************************************
//
//?????

**********************************************/

void Write_Command(uchar command)
{
//   IC_IIC_Start();
//   Write_IIC_Data(Slave_Address);                //Salve Adress
//   Write_IIC_Data(OP_Command);                   //???
//   Write_IIC_Data(command); 
//   IC_IIC_Stop();
	
	OP_Command[0]=0x00;
	OP_Command[1]=command;
	
	HAL_I2C_Master_Transmit(&hi2c3, Slave_Address, OP_Command, 2,100);
}

/**********************************************
//
//?????
//
**********************************************/
//
//

void Write_Data(uchar date)
{
//   IC_IIC_Start();
//   Write_IIC_Data(Slave_Address);                //Salve Adress
//   Write_IIC_Data(OP_Data);                      //???
//   Write_IIC_Data(date);
//   IC_IIC_Stop();
	
	OP_Data[0]=0x40;
	OP_Data[1]=date;
	
	HAL_I2C_Master_Transmit(&hi2c3, Slave_Address, OP_Data, 2,100);
}

/******************************************************
//
//??IC??
//
******************************************************/

void Reset_IC()
{
		sys_delay_us(10);
		HAL_GPIO_WritePin(GPIOC,RESET_Pin,GPIO_PIN_RESET);
		sys_delay_us(10);
		HAL_GPIO_WritePin(GPIOC,RESET_Pin,GPIO_PIN_SET);
		sys_delay_us(10);
}

/******************************************************
//
//???IC??
//
******************************************************/
void init_ic()
{
	Write_Command(0xAE); //Set Display Off
	Write_Command(0xD5); //Display divide ratio/osc. freq. mode
	Write_Command(0x80); //
	Write_Command(0xA8); //Multiplex ration mode:63
	Write_Command(0x3F);
	Write_Command(0xD3); //Set Display Offset
	Write_Command(0x00);
	Write_Command(0x40); //Set Display Start Line
	Write_Command(0xAD); //DC-DC Control Mode Set
	Write_Command(0x8B); //8A:External 8b:Built-in DC-DC is used
	Write_Command(0x32); //Set Pump voltage value
	Write_Command(0xA0); //A0:上 A1:下
	Write_Command(0xC0); //C0:左 C8:右
	Write_Command(0xDA); //Common pads hardware: alternative
	Write_Command(0x12);
	Write_Command(0x81); //Contrast control
	Write_Command(0x40);  // 40
	Write_Command(0xD9); //Set pre-charge period
	Write_Command(0x1F);
	Write_Command(0xDB); //VCOM deselect level mode
	Write_Command(0x40);
	Write_Command(0xA4); //Set Entire Display On/Off
	Write_Command(0xA6); //Set Normal Display
	Clear_Screen();
	Write_Command(0xAF); //Set Display On	 
}

void Clear_Screen()
{
   uchar page_number,column_number;
   for(page_number=0;page_number<8;page_number++)
   {
     Write_Command(0xb0+page_number);
     Write_Command(0x10);
     Write_Command(0x02);
     for(column_number=0;column_number<128;column_number++)
     {
        Write_Data(0x00);
     }
   }
}

void All_Screen()
{
  oled_init();
	//oled_refresh_gram(); //打开会有频闪
}


void oled_init()
{
	memset(OLED_GRAM,0x00,sizeof(OLED_GRAM));
}

void oled_powerOff(void)
{
	//HAL_GPIO_WritePin(SCL_GPIO_PORT,SCL_GPIO_PIN,GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(SDA_GPIO_PORT,SDA_GPIO_PIN,GPIO_PIN_RESET); 
	//HAL_GPIO_WritePin(GPIOC,RESET_Pin,GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(VOLED_GPIO_Port,VOLED_Pin,GPIO_PIN_RESET);
}

void oled_powerOn(void)
{
	HAL_GPIO_WritePin(VOLED_GPIO_Port,VOLED_Pin,GPIO_PIN_SET);
}

void  display_one_word(const uint8_t *DestBuf,uint8_t IconW,uint8_t IconH, uint8_t StartPageN,  uint8_t StartDsColN)
{
	uint16_t i,j;
	for(i=0;i<IconH/8;i++)
	{
		if(StartPageN+i>8)
			break;
		for(j=0;j<IconW;j++)
		{
				if(StartDsColN+j>128)
					break;
				OLED_GRAM[StartDsColN+j][StartPageN+i]=DestBuf[j+i*IconW];
		}
	}
}

void  display_one_word_const(const uint8_t *DestBuf,uint8_t IconW,uint8_t IconH, uint8_t StartPageN,  uint8_t StartDsColN)
{
	uint16_t i,j;
	for(i=0;i<IconH/8;i++)
	{
		if(StartPageN+i>8)
			break;
		for(j=0;j<IconW;j++)
		{
				if(StartDsColN+j>128)
					break;
				OLED_GRAM[StartDsColN+j][StartPageN+i]=~DestBuf[j+i*IconW];
		}
	}
}

// 请输入密码
void display_passwod_put(void)
{
	uint8_t i;
	All_Screen();
	for(i=0;i<5;i++)
	{
		display_one_word(&PASSWORD_PUT[i*32],16,16,0,i*16+24);
	}
	oled_refresh_gram();
}

// 请输入6到10位新密码
void display_passwod_warn()
{
	int i;
	All_Screen();
	for(i=0;i<3;i++)
	{
		display_one_word(&PASSWORD_WARN[i*32],16,16,0,i*16+12);
	}
	display_one_word(&PASSWORD_WARN[i*32],8,16,0,i*16+12);
	display_one_word(&PASSWORD_WARN[i*32+16],16,16,0,i*16+8+12);
	display_one_word(&PASSWORD_WARN[i*32+16+32],8,16,0,i*16+8+16+12);
	display_one_word(&PASSWORD_WARN[i*32+16+32+16],8,16,0,i*16+8+16+8+12);
	display_one_word(&PASSWORD_WARN[i*32+16+32+16+16],16,16,0,i*16+8+16+8+8+12);
	
	for(i=0;i<3;i++)
	{
		display_one_word(&PASSWORD_WARN[208+i*32],16,16,2,i*16+40);
	}
	oled_refresh_gram();
}
// 请再次输入6到10位新密码
void display_passwod_warn_again(void)
{
	int i;
	All_Screen();
	for(i=0;i<5;i++)
	{
		display_one_word(&PASSWORD_WARN_AGAIN[i*32],16,16,0,i*16+4);
	}
	
	display_one_word(&PASSWORD_WARN_AGAIN[160],8,16,0,84);
	display_one_word(&PASSWORD_WARN_AGAIN[160+16],16,16,0,84+8);
	display_one_word(&PASSWORD_WARN_AGAIN[160+16+32],8,16,0,84+8+16);
	display_one_word(&PASSWORD_WARN_AGAIN[160+16+32+16],8,16,0,84+8+16+8);
	
	for(i=0;i<4;i++)
	{
		display_one_word(&PASSWORD_WARN_AGAIN[240+i*32],16,16,2,32+i*16);
	}
	
	oled_refresh_gram();
}

void display_passwod_null(void)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&PASSWORD_NULL[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_passwod_admin(void)
{
	int i;
	All_Screen();
	for(i=0;i<6;i++)
	{
		display_one_word(&PASSWORD_ANDMIN[i*32],16,16,0,i*16+16);
	}
	oled_refresh_gram();
}

void display_passwod_exit(void)
{
	int i;
	All_Screen();
	for(i=0;i<5;i++)
	{
		display_one_word(&PASSWORD_EXIT[i*32],16,16,3,i*16+24);
	}
	oled_refresh_gram();
}

void display_password_full(void)
{
	int i;
	All_Screen();
	for(i=0;i<5;i++)
	{
		display_one_word(&PASSWORD_FULL[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_iphone_number(int num,int locate)
{
	if(locate<12)
	display_one_word(&NUMBER[num*16],8,16,4,locate*8+8);
	oled_refresh_gram_with_arg(4);
}

void display_number(int num,int locate)
{
	if(locate<11)
		display_one_word(&NUMBER[num*16],8,16,4,locate*8+24);
	oled_refresh_gram_with_arg(4);
}

void display_number_all(int num,int locate)
{
	if(locate<12)
	{
		display_one_word(&NUMBER[num*16],8,16,4,locate*8+16);
		oled_refresh_gram_with_arg(4);
	}
	else if(locate<24)
	{
		display_one_word(&NUMBER[num*16],8,16,6,(locate-12)*8+16);
		oled_refresh_gram_with_arg(6);
	}
		
}

void display_number_del(int locate)
{
	display_one_word(&NUMBER_DEL[0],8,16,4,(locate-1)*8+24);
	oled_refresh_gram_with_arg(3);
}
   
void display_mainmenu(void)
{
	int i;
	All_Screen();
	for(i=0;i<2;i++)
	{
		display_one_word(&ADD_USER[i*16],8,16,0,2+i*8);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&ADD_USER[i*32+32],16,16,0,2+16+i*16);
	}
	for(i=0;i<2;i++)
	{
		display_one_word(&DEL_USER[i*16],8,16,2,2+i*8);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&DEL_USER[i*32+32],16,16,2,2+16+i*16);
	}
	for(i=0;i<2;i++)
	{
		display_one_word(&SYS_SET[i*16],8,16,4,2+i*8);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&SYS_SET[i*32+32],16,16,4,2+16+i*16);
	}
	for(i=0;i<2;i++)
	{
		display_one_word(&SYS_INFO[i*16],8,16,6,2+i*8);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&SYS_INFO[i*32+32],16,16,6,2+16+i*16);
	}
	
	oled_refresh_gram();
}

void display_submenu_add(void)
{
	int i;
	All_Screen();
	for(i=0;i<2;i++)
	{
		display_one_word(&ADMIN[i*16],8,16,0,2+i*8);
	}
	for(i=0;i<3;i++)
	{
		display_one_word(&ADMIN[i*32+32],16,16,0,2+16+i*16);
	}
	for(i=0;i<2;i++)
	{
		display_one_word(&USER[i*16],8,16,2,2+i*8);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&USER[i*32+32],16,16,2,2+16+i*16);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&USER_SE[i*16],8,16,4,2+i*8);
	}
	for(i=0;i<5;i++)
	{
		display_one_word(&USER_SE[i*32+32],16,16,4,2+16+i*16);
	}
	
	oled_refresh_gram();
}

void display_subsubmenu_add(uint8_t button)
{
	int i;
	All_Screen();
	
	for(i=0;i<2;i++)
	{
		display_one_word(&ADD_FINGER[i*16],8,16,0,2+i*8);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&ADD_FINGER[i*32+32],16,16,0,2+16+i*16);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&ADD_PASSWORD[i*16],8,16,2,2+i*8);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&ADD_PASSWORD[i*32+32],16,16,2,2+16+i*16);
	}
	if(button>0)
	{
		for(i=0;i<2;i++)
		{
			display_one_word(&ADD_CARD[i*16],8,16,4,2+i*8);
		}
		for(i=0;i<5;i++)
		{
			display_one_word(&ADD_CARD[i*32+32],16,16,4,2+16+i*16);
		}
	}
	oled_refresh_gram();
}

void display_submenu_del(uint8_t button)
{
	int i;
	All_Screen();
	for(i=0;i<2;i++)
	{
		display_one_word(&DEL_FINGER[i*16],8,16,0,2+i*8);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&DEL_FINGER[i*32+32],16,16,0,2+16+i*16);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&DEL_PASSWORD[i*16],8,16,2,2+i*8);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&DEL_PASSWORD[i*32+32],16,16,2,2+16+i*16);
	}
	
	if(button>0)
	{
		for(i=0;i<2;i++)
		{
			display_one_word(&DEL_CARD[i*16],8,16,4,2+i*8);
		}
		for(i=0;i<5;i++)
		{
			display_one_word(&DEL_CARD[i*32+32],16,16,4,2+16+i*16);
		}
	}
	oled_refresh_gram();
}


void display_submenu_sys(void)
{
	int i;
	All_Screen();
	for(i=0;i<2;i++)
	{
		display_one_word(&VOICE_SET[i*16],8,16,0,2+i*8);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&VOICE_SET[i*32+32],16,16,0,2+16+i*16);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&WAN_SET[i*16],8,16,2,2+i*8);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&WAN_SET[i*32+32],16,16,2,2+16+i*16);
	}
		
	for(i=0;i<2;i++)
	{
		display_one_word(&DATA_SET[i*16],8,16,4,2+i*8);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&DATA_SET[i*32+32],16,16,4,2+16+i*16);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&FACTORY_SET[i*16],8,16,6,2+i*8);
	}
	for(i=0;i<6;i++)
	{
		display_one_word(&FACTORY_SET[i*32+32],16,16,6,2+16+i*16);
	}
	
	oled_refresh_gram();
}

void display_subsubmenu_sys(void)
{
	int i;
	All_Screen();
	for(i=0;i<2;i++)
	{
		display_one_word(&VOICE_HIGH[i*16],8,16,0,2+i*8);
	}
	for(i=0;i<3;i++)
	{
		display_one_word(&VOICE_HIGH[i*32+32],16,16,0,2+16+i*16);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&VOICE_MID[i*16],8,16,2,2+i*8);
	}
	for(i=0;i<3;i++)
	{
		display_one_word(&VOICE_MID[i*32+32],16,16,2,2+16+i*16);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&VOICE_LOW[i*16],8,16,4,2+i*8);
	}
	for(i=0;i<3;i++)
	{
		display_one_word(&VOICE_LOW[i*32+32],16,16,4,2+16+i*16);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&VOICE_OFF[i*16],8,16,6,2+i*8);
	}
	for(i=0;i<2;i++)
	{
		display_one_word(&VOICE_OFF[i*32+32],16,16,6,2+16+i*16);
	}
	
	oled_refresh_gram();
}

void display_sys_info_menu(void)
{
	int i;
	All_Screen();
	
	for(i=0;i<2;i++)
	{
		display_one_word(&DATE_LOG[i*16],8,16,0,i*8+2);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&DATE_LOG[i*32+32],16,16,0,i*16+16+2);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&DATE_LOG[i*16+32+128],8,16,2,i*8+2);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&DATE_LOG[i*32+32+128+32],16,16,2,i*16+16+2);
	}
		
	for(i=0;i<2;i++)
	{
		display_one_word(&DATE_LOG[i*16+32+128+32+128],8,16,4,i*8+2);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&DATE_LOG[i*32+32+128+32+128+32],16,16,4,i*16+16+2);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&DATE_LOG[i*16+32+128+32+128+32+128],8,16,6,i*8+2);
	}
	for(i=0;i<4;i++)
	{
		display_one_word(&DATE_LOG[i*32+32+128+32+128+32+128+32],16,16,6,i*16+16+2);
	}
	oled_refresh_gram();
}

void display_sys_host_info(void)
{
	int i,j=0;
	uint8_t index;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&SYS_VER[i*32],16,16,0,i*16+32);
	}
	
	for(i=0;i<3;i++)
	{
		display_one_word(&SYS_VER[i*32+128],16,16,2,i*16+2);
	}
	
	for(i=0;i<6;i++)
	{
		display_one_word(&SYS_VER[i*16+128+96],8,16,2,i*8+2+40);
	}
	
	for(i=0;i<3;i++)
	{
		display_one_word(&SYS_VER[i*32+128+96+96],16,16,4,i*16+2);
	}
	
	for(i=0;i<6;i++)
	{
		display_one_word(&SYS_VER[i*16+128+96+96+96],8,16,4,i*8+2+40);
	}
	
	for(i=0;i<4;i++)
	{
		display_one_word(&MAC[i*16],8,16,6,i*8+2);
	}
	
	if(strncmp((const char*)wifi_sys_info.wifi_mac,"FFFFFFFFFFFF",12)==NULL)
		wifi_get_mac(wifi_sys_info.wifi_mac);
	if(strncmp((const char*)wifi_sys_info.wifi_mac,"FFFFFFFFFFFF",12)==NULL)
	{
		for(i=0;i<12;i++)
		{
			display_one_word(&MAC[4*16],8,16,6,i*8+32+2);
		}
	}else{
		for(i=0;i<17;i++)
		{
			if(wifi_sys_info.wifi_mac[i]==':')
			{
				//display_one_word(&DOUBLE_SPOT[0],8,16,6,i*8+32+2);
				continue;
			}else{
				debug_printf("%c\r\n",wifi_sys_info.wifi_mac[i]);
				
				if(wifi_sys_info.wifi_mac[i]>='a'&&wifi_sys_info.wifi_mac[i]<='z')
				{
					index = wifi_sys_info.wifi_mac[i]-'a';
					display_one_word(&CHARACTER[index*16],8,16,6,j*8+32+2);
				}
				else if(wifi_sys_info.wifi_mac[i]>='0'&&wifi_sys_info.wifi_mac[i]<='9')
				{
					index = wifi_sys_info.wifi_mac[i]-'0';
					display_one_word(&NUMBER[index*16],8,16,6,j*8+32+2);
				}
				j++;
			}
		}
	}
	
	oled_refresh_gram();
}

void display_sys_battery_info(void)
{
	int i;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&BATTERY[i*32],16,16,0,i*16+32);
	}
	
	for(i=0;i<5;i++)
	{
		display_one_word(&BATTERY[i*32+128],16,16,4,i*16+2);
	}
	
	if(battery_class>70)  // 高
	{
		display_one_word(&BATTERY_CLASS[0],16,16,4,82);
	}
	else if(battery_class>10) // 中
	{
		display_one_word(&BATTERY_CLASS[32],16,16,4,82);
	}else{                   // 低
		display_one_word(&BATTERY_CLASS[64],16,16,4,82);
	}
	
//	if(battery_class==0)
//	{
//		display_one_word(&NUMBER[1*16],8,16,4,82);
//		display_one_word(&PERCENT[0],8,16,4,82+8);
//	}
//	else if(battery_class<10){
//		display_one_word(&NUMBER[battery_class*16],8,16,4,82);
//		display_one_word(&PERCENT[0],8,16,4,82+8);
//	}else if(battery_class<=99){
//		display_one_word(&NUMBER[battery_class/10*16],8,16,4,82);
//		display_one_word(&NUMBER[battery_class%10*16],8,16,4,82+8);
//		display_one_word(&PERCENT[0],8,16,4,82+8+8);
//	}else{
//		display_one_word(&NUMBER[1*16],8,16,4,82);
//		display_one_word(&NUMBER[0],8,16,4,82+8);
//		display_one_word(&NUMBER[0],8,16,4,82+8+8);
//		display_one_word(&PERCENT[0],8,16,4,82+8+8+8);
//	}
	
	oled_refresh_gram();
}

void display_sys_source_info(uint8_t fp_num,uint8_t keyboard_num,uint8_t card_num)
{
	int i;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&SOURCE_USED[i*32],16,16,0,i*16+32);
	}
	
	if(fp_num>=FINGER_NUM)
		fp_num = 100;
	for(i=0;i<2;i++)
	{
		display_one_word(&SOURCE_USED[i*32+128],16,16,2,i*16+2);
	}
	display_one_word(&SOURCE_USED[128+64],8,16,2,2+32);
	if(fp_num<=9)
	{
		display_one_word(&NUMBER[fp_num*16],8,16,2,2+32+8);
		for(i=2;i<6;i++)
		{
			display_one_word(&SOURCE_USED[i*16+128+64],8,16,2,i*8+2+32);
		}
	}
	else if(fp_num<FINGER_NUM)
	{
		display_one_word(&NUMBER[fp_num/10*16],8,16,2,2+32+8);
		display_one_word(&NUMBER[fp_num%10*16],8,16,2,2+32+8+8);
		for(i=2;i<6;i++)
		{
			display_one_word(&SOURCE_USED[i*16+128+64],8,16,2,i*8+2+32+8);
		}
	}else{
		display_one_word(&NUMBER[fp_num/100*16],8,16,2,2+32+8);
		display_one_word(&NUMBER[(fp_num/10)%10*16],8,16,2,2+32+8+8);
		display_one_word(&NUMBER[fp_num%10*16],8,16,2,2+32+8+8+8);
		for(i=2;i<6;i++)
		{
			display_one_word(&SOURCE_USED[i*16+128+64],8,16,2,i*8+2+32+8+8);
		}
	}
	
	// 密码：0/100
	if(keyboard_num>=KEYBOARD_NUM)
		keyboard_num = 100;
	for(i=0;i<2;i++)
	{
		display_one_word(&SOURCE_USED[i*32+128+64+96],16,16,4,i*16+2);
	}
	display_one_word(&SOURCE_USED[128+64+96+64],8,16,4,2+32);
	if(keyboard_num<=9)
	{
		display_one_word(&NUMBER[keyboard_num*16],8,16,4,2+32+8);
		for(i=2;i<6;i++)
		{
			display_one_word(&SOURCE_USED[i*16+128+64+96+64],8,16,4,i*8+2+32);
		}
	}
	else if(keyboard_num<KEYBOARD_NUM)
	{
		display_one_word(&NUMBER[keyboard_num/10*16],8,16,4,2+32+8);
		display_one_word(&NUMBER[keyboard_num%10*16],8,16,4,2+32+8+8);
		for(i=2;i<6;i++)
		{
			display_one_word(&SOURCE_USED[i*16+128+64+96+64],8,16,4,i*8+2+32+8);
		}
	}
	else{
		display_one_word(&NUMBER[keyboard_num/100*16],8,16,4,2+32+8);
		display_one_word(&NUMBER[(keyboard_num/10)%10*16],8,16,4,2+32+8+8);
		display_one_word(&NUMBER[keyboard_num%10*16],8,16,4,2+32+8+8+8);
		for(i=2;i<6;i++)
		{
			display_one_word(&SOURCE_USED[i*16+128+64],8,16,4,i*8+2+32+8+8);
		}
	}
	// 感应卡
	if(card_num>=CARD_NUM)
		card_num = 100;
	for(i=0;i<3;i++)
	{
		display_one_word(&SOURCE_USED[i*32+128+64+96+64+96],16,16,6,i*16+2);
	}
	display_one_word(&SOURCE_USED[128+64+96+64+96+96],8,16,6,2+48);
	if(card_num<=9)
	{
		display_one_word(&NUMBER[card_num*16],8,16,6,2+48+8);
		for(i=2;i<6;i++)
		{
			display_one_word(&SOURCE_USED[i*16+128+64+96+64+96+96],8,16,6,i*8+2+48);
		}
	}
	else if(card_num<CARD_NUM)
	{
		display_one_word(&NUMBER[card_num/10*16],8,16,6,2+48+8);
		display_one_word(&NUMBER[card_num%10*16],8,16,6,2+48+8+8);
		for(i=2;i<6;i++)
		{
			display_one_word(&SOURCE_USED[i*16+128+64+96+64+96+96],8,16,6,i*8+2+48+8);
		}
	}
	else{
		display_one_word(&NUMBER[card_num/100*16],8,16,6,2+48+8);
		display_one_word(&NUMBER[(card_num/10)%10*16],8,16,6,2+48+8+8);
		display_one_word(&NUMBER[card_num%10*16],8,16,6,2+48+8+8+8);
		for(i=2;i<6;i++)
		{
			display_one_word(&SOURCE_USED[i*16+128+64],8,16,6,i*8+2+48+8+8);
		}
	}

	oled_refresh_gram();
}

void display_spe_char(uint8_t locate)
{
	//debug_printf(" locate:%d\r\n",locate);
	if(locate<=10)
		display_one_word(&SPE_CHART[0],8,16,4,24+(locate-1)*8);
	else
		display_one_word(&SPE_CHART[0],8,16,6,24+(locate-10-1)*8);
	oled_refresh_gram();
}

void display_spe_char_factory(uint8_t ch,uint8_t locate)
{
	if(locate<12)
	{
		display_one_word(&SPE_CHART[ch*16],8,16,4,locate*8+16);
		oled_refresh_gram_with_arg(4);
	}
	else if(locate<24)
	{
		display_one_word(&SPE_CHART[ch*16],8,16,6,(locate-12)*8+16);
		oled_refresh_gram_with_arg(6);
	}
}

void display_spe_char_del(uint8_t locate)
{
	debug_printf("locate:%d\r\n",locate);
	if(locate<=10)
		display_one_word(&SPE_CHART_DEL[0],8,16,4,24+(locate-1)*8);
	else
		display_one_word(&SPE_CHART_DEL[0],8,16,6,24+(locate-10-1)*8);
	oled_refresh_gram();
}

void display_company_name(void)
{
	All_Screen();
	int i,j;

	for(i=0;i<8;i++)
	{
		for(j=0;j<128;j++)
		{
			OLED_GRAM[j][i]=company3[i*128+j];
		}
	}
}

void display_date_md(uint8_t month,uint8_t day)
{
	display_one_word(&NUMBER[month/10*16],8,16,6,44+8);
	display_one_word(&NUMBER[month%10*16],8,16,6,44+8+8);

	display_one_word(&LINE_D[0],8,16,6,44+8+8+8);
	
	display_one_word(&NUMBER[day/10*16],8,16,6,44+8+8+8+8);
	display_one_word(&NUMBER[day%10*16],8,16,6,44+8+8+8+8+8);
	
	oled_refresh_gram();
}

void display_date_ymd(uint8_t year,uint8_t month,uint8_t day)
{
	
	// year
	display_one_word(&NUMBER[2*16],8,16,0,2);
	display_one_word(&NUMBER[0],8,16,0,2+8*1);
	display_one_word(&NUMBER[year/10*16],8,16,0,2+8*2);
	display_one_word(&NUMBER[year%10*16],8,16,0,2+8*3);
	
	display_one_word(&LINE_D[0],8,16,0,2+8*4);

	if(month<10)
	{
		display_one_word(&NUMBER[month*16],8,16,0,2+8*5);
		display_one_word(&LINE_D[0],8,16,0,2+8*6);
		if(day<10)
		{
			display_one_word(&NUMBER[day*16],8,16,0,2+8*7);
		}else{
			display_one_word(&NUMBER[day/10*16],8,16,0,2+8*7);
			display_one_word(&NUMBER[day%10*16],8,16,0,2+8*8);
		}
	}else{
		display_one_word(&NUMBER[month/10*16],8,16,0,2+8*5);
		display_one_word(&NUMBER[month%10*16],8,16,0,2+8*6);
		display_one_word(&LINE_D[0],8,16,0,2+8*7);
		if(day<10)
		{
			display_one_word(&NUMBER[day*16],8,16,0,2+8*8);
		}else{
			display_one_word(&NUMBER[day/10*16],8,16,0,2+8*8);
			display_one_word(&NUMBER[day%10*16],8,16,0,2+8*9);
		}
	}
	oled_refresh_gram();
}

void display_date_hm(uint8_t hour,uint8_t min)
{
//	display_one_word(&NUMBER[hour/10*16],8,16,6,44+8);
//	display_one_word(&NUMBER[hour%10*16],8,16,6,44+8+8);

//	display_one_word(&DOUBLE_SPOT[0],8,16,6,44+8+8+8);
//	
//	display_one_word(&NUMBER[min/10*16],8,16,6,44+8+8+8+8);
//	display_one_word(&NUMBER[min%10*16],8,16,6,44+8+8+8+8+8);
	
	display_one_word(&NUMBER[hour/10*16],8,16,0,2);
	display_one_word(&NUMBER[hour%10*16],8,16,0,10);

	display_one_word(&DOUBLE_SPOT[0],8,16,0,18);
	
	display_one_word(&NUMBER[min/10*16],8,16,0,26);
	display_one_word(&NUMBER[min%10*16],8,16,0,34);
	
	oled_refresh_gram();
}

void display_check_failed(void)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&CHECK_FAILED[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_check_sucessful(uint8_t index,uint8_t permision,uint8_t type)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&CHECK_SUCESSFUL[i*32],16,16,0,i*16+32);
	}
	
	if(index<10)
	{
		display_one_word(&NUMBER[index*16],8,16,3,16);
		display_one_word(&NUM_CARD[0],16,16,3,16+8);
	}else{
		display_one_word(&NUMBER[index/10*16],8,16,3,8);
		display_one_word(&NUMBER[index%10*16],8,16,3,8+8);
		display_one_word(&NUM_CARD[0],16,16,3,8+8+8);
	}	

	if(permision==ANDMI)
	{
		display_one_word(&LOG_PERMISION[0],16,16,3,16+8+16);
		display_one_word(&LOG_PERMISION[32],16,16,3,16+8+16+16);
	}
	else if(permision==USUALLY)
	{
		for(i=0;i<2;i++)
			display_one_word(&LOG_PERMISION[64+i*32],16,16,3,16+8+16+i*16);
	}else{
		for(i=0;i<2;i++)
			display_one_word(&LOG_PERMISION[128+i*32],16,16,3,16+8+16+i*16);
	}
	
	if(type==KEYBOARD_TYPE)
	{
		display_one_word(&LOG_TYPE[0],16,16,3,16+8+16+16+16);
		display_one_word(&LOG_TYPE[32],16,16,3,16+8+16+16+16+16);
	}
	else if(type==CARD_TYPE)
	{
		display_one_word(&LOG_TYPE[64],16,16,3,16+8+16+16+16);
		display_one_word(&LOG_TYPE[96],16,16,3,16+8+16+16+16+16);
	}else{
		display_one_word(&LOG_TYPE[128],16,16,3,16+8+16+16+16);
		display_one_word(&LOG_TYPE[160],16,16,3,16+8+16+16+16+16);
	}
	oled_refresh_gram();
}

void display_check_exit(void)
{
	int i;
	All_Screen();
	for(i=0;i<5;i++)
	{
		display_one_word(&CHECK_EXIT[i*32],16,16,3,i*16+24);
	}
	oled_refresh_gram();
}

void display_operate_fail(void)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&OPERATE_F[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}


void display_operate_sucessful(void)
{
	int i;
	
	memcpy(OLED_GRAM_BAK,OLED_GRAM,sizeof(OLED_GRAM));
	
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&OPERATE_S[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
	
	sys_delay_us(300000);
	
	memcpy(OLED_GRAM,OLED_GRAM_BAK,sizeof(OLED_GRAM_BAK));
	
	oled_refresh_gram();
}

void display_del_sucessful(void)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&DEL_SUCESS[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_del_failed(void)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&DEL_FAILE[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_number_password_const(uint8_t number,uint8_t page,uint8_t lock_flag,uint8_t lock_num)
{
	int i;
	
	if(number<10)
	{
		display_one_word_const(&NUMBER[number*16],8,16,page,2);
		for(i=0;i<3;i++)
		{
			display_one_word_const(&NUM_PASSWORD[i*32],16,16,page,i*16+8+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word_const(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word_const(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word_const(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word_const(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	else if(number<100)
	{
		display_one_word_const(&NUMBER[number/10*16],8,16,page,2);
		display_one_word_const(&NUMBER[number%10*16],8,16,page,8+2);
		
		for(i=0;i<3;i++)
		{
			display_one_word_const(&NUM_PASSWORD[i*32],16,16,page,i*16+16+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word_const(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word_const(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word_const(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word_const(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	oled_refresh_gram();
}

void display_number_password(uint8_t number,uint8_t page,uint8_t lock_flag,uint8_t lock_num)
{
	int i;

	if(number<10)
	{
		display_one_word(&NUMBER[number*16],8,16,page,2);
		for(i=0;i<3;i++)
		{
			display_one_word(&NUM_PASSWORD[i*32],16,16,page,i*16+8+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	else if(number<100)
	{
		display_one_word(&NUMBER[number/10*16],8,16,page,2);
		display_one_word(&NUMBER[number%10*16],8,16,page,8+2);
		
		for(i=0;i<3;i++)
		{
			display_one_word(&NUM_PASSWORD[i*32],16,16,page,i*16+16+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	oled_refresh_gram();
}

void display_number_finger_const(uint8_t number,uint8_t page,uint8_t lock_flag,uint8_t lock_num)
{
	int i;
	//uint8_t number_0=0;
	//uint8_t number_9=10;
	//All_Screen();
	
	debug_printf("lock_flag：%d : %d  \r\n",lock_flag,lock_num);
	if(number<10)
	{
		display_one_word_const(&NUMBER[number*16],8,16,page,2);
		for(i=0;i<3;i++)
		{
			display_one_word_const(&NUM_FINGER[i*32],16,16,page,i*16+8+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word_const(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word_const(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word_const(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word_const(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	else if(number<100)
	{
		display_one_word_const(&NUMBER[number/10*16],8,16,page,2);
		display_one_word_const(&NUMBER[number%10*16],8,16,page,8+2);
		
		for(i=0;i<3;i++)
		{
			display_one_word_const(&NUM_FINGER[i*32],16,16,page,i*16+16+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word_const(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word_const(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word_const(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word_const(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	else{
		display_one_word_const(&NUMBER[number/100*16],8,16,page,2);
		display_one_word_const(&NUMBER[number/10%10*16],8,16,page,8+2);
		display_one_word_const(&NUMBER[number%10*16],8,16,page,8+8+2);
		for(i=0;i<3;i++)
		{
			display_one_word_const(&NUM_FINGER[i*32],16,16,page,i*16+16+8+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word_const(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word_const(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word_const(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word_const(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	oled_refresh_gram();
}

void display_number_card_const(uint8_t number,uint8_t page,uint8_t lock_flag,uint8_t lock_num)
{
	int i;
	uint8_t number_0=0;
	uint8_t number_9=9;
	//All_Screen();
	
	if(number>=number_0&&number<=number_9)
	{
		display_one_word_const(&NUMBER[number*16],8,16,page,2);
		for(i=0;i<3;i++)
		{
			display_one_word_const(&NUM_CARD[i*32],16,16,page,i*16+8+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word_const(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word_const(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word_const(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word_const(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	else if(number>=10&&number<=99)
	{
		display_one_word_const(&NUMBER[number/10*16],8,16,page,2);
		display_one_word_const(&NUMBER[number%10*16],8,16,page,8+2);
		
		for(i=0;i<3;i++)
		{
			display_one_word_const(&NUM_CARD[i*32],16,16,page,i*16+16+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word_const(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word_const(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word_const(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word_const(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	oled_refresh_gram();
}

void display_number_card(uint8_t number,uint8_t page,uint8_t lock_flag,uint8_t lock_num)
{
	int i;
	debug_printf("number: %d \r\n",number);
	if(number<10)
	{
		display_one_word(&NUMBER[number*16],8,16,page,2);
		for(i=0;i<3;i++)
		{
			display_one_word(&NUM_CARD[i*32],16,16,page,i*16+8+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	else if(number<100)
	{
		display_one_word(&NUMBER[number/10*16],8,16,page,2);
		display_one_word(&NUMBER[number%10*16],8,16,page,8+2);
		
		for(i=0;i<3;i++)
		{
			display_one_word(&NUM_CARD[i*32],16,16,page,i*16+16+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	oled_refresh_gram();
}

void display_number_finger(uint8_t number,uint8_t page,uint8_t lock_flag,uint8_t lock_num)
{
	int i;
	//uint8_t number_0=0;
	//uint8_t number_9=10;
	
	if(number<10)
	{
		display_one_word(&NUMBER[number*16],8,16,page,2);
		for(i=0;i<3;i++)
		{
			display_one_word(&NUM_FINGER[i*32],16,16,page,i*16+8+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	else if(number<100)
	{
		display_one_word(&NUMBER[number/10*16],8,16,page,2);
		display_one_word(&NUMBER[number%10*16],8,16,page,8+2);
		
		for(i=0;i<3;i++)
		{
			display_one_word(&NUM_FINGER[i*32],16,16,page,i*16+16+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	else
	{
		display_one_word(&NUMBER[number/100*16],8,16,page,2);
		display_one_word(&NUMBER[number/10%10*16],8,16,page,8+2);
		display_one_word(&NUMBER[number%10*16],8,16,page,8+8+2);
		for(i=0;i<3;i++)
		{
			display_one_word(&NUM_FINGER[i*32],16,16,page,i*16+16+8+2);
		}
		if(lock_flag&&number==lock_num)
		{
			display_one_word(&PASSWORD_LOCK[0],8,16,page,3*16+8+2);
			display_one_word(&PASSWORD_LOCK[16],16,16,page,3*16+8+2+8);
			display_one_word(&PASSWORD_LOCK[48],16,16,page,3*16+8+2+8+16);
			display_one_word(&PASSWORD_LOCK[80],8,16,page,3*16+8+2+8+16+16);
		}
	}
	oled_refresh_gram();
}

void display_finger_check_fail(void)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&FP_CHECK[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_finger_press(void)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&FP_PRESS[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_finger_collection(void)
{
	int i;
	All_Screen();
	for(i=0;i<6;i++)
	{
		display_one_word(&FP_COLLECT[i*32],16,16,3,i*16+16);
	}
	oled_refresh_gram();
}
void display_finger_sucess(void)
{
	int i;
	All_Screen();
	for(i=0;i<6;i++)
	{
		display_one_word(&FP_SUCESS[i*32],16,16,3,i*16+16);
	}
	oled_refresh_gram();
}

void display_finger_fail(void)
{
	int i;
	All_Screen();
	for(i=0;i<6;i++)
	{
		display_one_word(&FP_FAILE[i*32],16,16,3,i*16+16);
	}
	oled_refresh_gram();
}

void display_finger_exit(void)
{
	int i;
	All_Screen();
	for(i=0;i<5;i++)
	{
		display_one_word(&FP_EXIT[i*32],16,16,3,i*16+24);
	}
	oled_refresh_gram();
}

void display_finger_outtime(void)
{
	int i;
	All_Screen();
	for(i=0;i<6;i++)
	{
		display_one_word(&FP_OUTTIME[i*32],16,16,3,i*16+16);
	}
	oled_refresh_gram();
}

void display_finger_del_sucess(void)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&FP_DEL_SUCESS[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_finger_del_fail(void)
{
	int i;
	All_Screen();
	for(i=0;i<6;i++)
	{
		display_one_word(&FP_DEL_FAILE[i*32],16,16,3,i*16+16);
	}
	oled_refresh_gram();
}

void display_finger_non_exit(void)
{
	int i;
	All_Screen();
	for(i=0;i<5;i++)
	{
		display_one_word(&FP_DEL_NO_EXIT[i*32],16,16,3,i*16+24);
	}
	oled_refresh_gram();
}

void display_finger_null(void)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&FP_NULL[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_finger_realse(void)
{
	int i;
	All_Screen();
	for(i=0;i<5;i++)
	{
		display_one_word(&FP_REALSE[i*32],16,16,3,i*16+24);
	}
	oled_refresh_gram();
}

void display_finger_full(void)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&FP_FULL[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_set_sucess(void)
{
	int i;
	
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&SET_SUCESS[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_set_fail(void)
{
	int i;
	
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&SET_FAIL[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

// type:0->密码 1->卡片 2->指纹

void display_confirm(uint8_t type,uint8_t permision,uint8_t number)
{
	int i,j;
	uint8_t number_0=0;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&CONFIRM_SUCESS[i*32],16,16,0,i*16+32);
	}
	for(i=0;i<3;i++)
	{
		display_one_word(&CONFIRM_SUCESS[i*32+4*32],16,16,2,i*16+2);
	}
	if(type==KEYBOARD_TYPE)
	{
		for(i=0;i<2;i++)
		{
			display_one_word(&CONFIRM_SUCESS[i*32+7*32],16,16,2,i*16+42);
		}
	}
	else if(type==CARD_TYPE)
	{
		for(i=0;i<2;i++)
		{
			display_one_word(&CONFIRM_SUCESS[i*32+9*32],16,16,2,i*16+42);
		}
	}
	else if(type==FINGER_TYPE)
	{
		for(i=0;i<2;i++)
		{
			display_one_word(&CONFIRM_SUCESS[i*32+11*32],16,16,2,i*16+42);
		}
	}
	else if(type==APP_TYPE)
	{
		for(j=0;j<3;j++)
		{
			display_one_word(&APP_OPEN[j*16],8,16,2,j*8+42);
		}
		for(j=0;j<4;j++)
		{
			display_one_word(&APP_OPEN[48+j*32],16,16,2,j*16+42+24);
		}
	}
	// 用户：
	for(i=0;i<3;i++)
	{
		display_one_word(&CONFIRM_SUCESS[i*32+13*32],16,16,4,i*16+2);
	}
	
	if(permision==ANDMI)
	{
		for(i=0;i<3;i++)
		{
			display_one_word(&CONFIRM_SUCESS[i*32+16*32],16,16,4,i*16+42);
		}
		
	}
	else
	{
		for(i=0;i<4;i++)
		{
			display_one_word(&CONFIRM_SUCESS[i*32+19*32],16,16,4,i*16+42);
		}
	}
//	else{
//		for(i=0;i<5;i++)
//		{
//			display_one_word(&ANTI_HAK_SHOW[i*32],16,16,4,i*16+42);
//		}
//	}
	// 编号：
	for(i=0;i<3;i++)
	{
		display_one_word(&CONFIRM_SUCESS[i*32+23*32],16,16,6,i*16+2);
	}
	
	if(number>=number_0&&number<=9)
	{
		display_one_word(&NUMBER[number*16],8,16,6,42);
		
	}
	else if(number>=10&&number<=99)
	{
		display_one_word(&NUMBER[number/10*16],8,16,6,42);
		display_one_word(&NUMBER[number%10*16],8,16,6,50);
	}
	
	oled_refresh_gram();
}

void display_whether_factory(void)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&FACTORY_WHETHER[i*32],16,16,2,i*16+32);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&FACTORY_WHETHER[i*32+128],16,16,5,i*16+20);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&FACTORY_WHETHER[i*16+128+64],8,16,5,i*8+20+32);
	}
	
	for(i=0;i<2;i++)
	{
		display_one_word(&FACTORY_WHETHER[i*32+128+64+32],16,16,5,i*16+20+32+16);
	}
	for(i=0;i<1;i++)
	{
		display_one_word(&FACTORY_WHETHER[i*16+128+64+32+64],8,16,5,i*16+20+32+16+32);
	}
	
	oled_refresh_gram();
}

void display_date_set()
{
	
	int i;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&DATE_SHOW[i*32],16,16,0,i*16+32);
	}
	//YEAR
	display_one_word(&NUMBER[2*16],8,16,3,24);
	display_one_word(&NUMBER[0],8,16,3,24+8);
	display_one_word(&NUMBER[sdatestructure.Year/10*16],8,16,3,24+8*2);
	display_one_word(&NUMBER[sdatestructure.Year%10*16],8,16,3,24+8*3);
	display_one_word(&LINE[0],8,16,3,24+8*4);
	//MONTH
	display_one_word(&NUMBER[sdatestructure.Month/10*16],8,16,3,24+8*5);
	display_one_word(&NUMBER[sdatestructure.Month%10*16],8,16,3,24+8*6);
	display_one_word(&LINE[0],8,16,3,24+8*7);
	//DATE
	display_one_word(&NUMBER[sdatestructure.Date/10*16],8,16,3,24+8*8);
	display_one_word(&NUMBER[sdatestructure.Date%10*16],8,16,3,24+8*9);
	//HOUR
	display_one_word(&NUMBER[stimestructure.Hours/10*16],8,16,5,32);
	display_one_word(&NUMBER[stimestructure.Hours%10*16],8,16,5,32+8);
	display_one_word(&DOUBLE_SPOT[0],8,16,5,32+8*2);
	//MIN
	display_one_word(&NUMBER[stimestructure.Minutes/10*16],8,16,5,32+8*3);
	display_one_word(&NUMBER[stimestructure.Minutes%10*16],8,16,5,32+8*4);
	display_one_word(&DOUBLE_SPOT[0],8,16,5,32+8*5);
	//SECOND
	display_one_word(&NUMBER[stimestructure.Seconds/10*16],8,16,5,32+8*6);
	display_one_word(&NUMBER[stimestructure.Seconds%10*16],8,16,5,32+8*7);
	
	oled_refresh_gram();
}

void display_sys_log_null(void)
{
	int i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&LOG_NULL[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_sys_log_menu(int16_t cur_index)
{
	uint8_t i;
	
	for(i=0;i<9;i++)
			display_one_word(&LOG_HEAD[i*16],8,16,0,i*8+28);
	
	if(cur_index<LOG_NUM)
	{
		display_one_word(&NUMBER[cur_index/100*16],8,16,0,1*8+28);
		display_one_word(&NUMBER[(cur_index/10)%10*16],8,16,0,2*8+28);
		display_one_word(&NUMBER[cur_index%10*16],8,16,0,3*8+28);
		display_one_word(&NUMBER[(get_cur_info->cur_log_number)/100*16],8,16,0,5*8+28);
		display_one_word(&NUMBER[((get_cur_info->cur_log_number)/10)%10*16],8,16,0,6*8+28);
		display_one_word(&NUMBER[(get_cur_info->cur_log_number)%10*16],8,16,0,7*8+28);
	}
	
	cur_index -=1;
	
	// 用户属性	
	if(log_flash[cur_index].state==OPEN_DOOR)
	{
		display_one_word(&LOG_OPER[0],16,16,2,0);
		display_one_word(&LOG_OPER[32],16,16,2,16);
	}
	else if(log_flash[cur_index].state==ADD_USER_OPR)
	{
		display_one_word(&LOG_OPER[64],16,16,2,0);
		display_one_word(&LOG_OPER[96],16,16,2,16);
	}
	else if(log_flash[cur_index].state==DEL_USER_OPR)
	{
		display_one_word(&LOG_OPER[128],16,16,2,0);
		display_one_word(&LOG_OPER[160],16,16,2,16);
	}
	
	display_one_word(&LOG_STATE[64],8,16,2,32);
	
	display_one_word(&NUMBER[log_flash[cur_index].index/100*16],8,16,2,1*8+32);
	display_one_word(&NUMBER[(log_flash[cur_index].index/10)%10*16],8,16,2,2*8+32);
	display_one_word(&NUMBER[log_flash[cur_index].index%10*16],8,16,2,3*8+32);
	
	if(log_flash[cur_index].permision==ANDMI)
	{
		display_one_word(&LOG_PERMISION[0],16,16,2,32+24+8);
		display_one_word(&LOG_PERMISION[32],16,16,2,32+24+8+16);
	}
	else if(log_flash[cur_index].permision==USUALLY)
	{
		display_one_word(&LOG_PERMISION[64],16,16,2,32+24+8);
		display_one_word(&LOG_PERMISION[96],16,16,2,32+24+8+16);
	}
	else
	{
		display_one_word(&LOG_PERMISION[128],16,16,2,32+24+8);
		display_one_word(&LOG_PERMISION[160],16,16,2,32+24+8+16);
	}
	
	if(log_flash[cur_index].type==KEYBOARD_TYPE)
	{
		// 密码
		display_one_word(&LOG_TYPE[0],16,16,2,32+24+8+16+16);
		display_one_word(&LOG_TYPE[32],16,16,2,32+24+8+16+16+16);
	}
	else if(log_flash[cur_index].type==CARD_TYPE)
	{
		// 卡片
		display_one_word(&LOG_TYPE[64],16,16,2,32+24+8+16+16);
		display_one_word(&LOG_TYPE[96],16,16,2,32+24+8+16+16+16);
	}else{
		// 指纹
		display_one_word(&LOG_TYPE[128],16,16,2,32+24+8+16+16);
		display_one_word(&LOG_TYPE[160],16,16,2,32+24+8+16+16+16);
	}
	
	// 日期
	for(i=0;i<2;i++)
	{
		display_one_word(&LOG_DATE[i*32],16,16,4,i*16+2);
	}
	for(i=0;i<3;i++)
	{
		display_one_word(&LOG_DATE[i*16+64],8,16,4,i*8+2+32);
	}
	
	display_one_word(&NUMBER[log_flash[cur_index].date.Year/10*16],8,16,4,3*8+2+32);
	display_one_word(&NUMBER[log_flash[cur_index].date.Year%10*16],8,16,4,4*8+2+32);
	display_one_word(&LOG_DATE[144],8,16,4,5*8+2+32);
	display_one_word(&NUMBER[log_flash[cur_index].date.Month/10*16],8,16,4,6*8+2+32);
	display_one_word(&NUMBER[log_flash[cur_index].date.Month%10*16],8,16,4,7*8+2+32);
	display_one_word(&LOG_DATE[192],8,16,4,8*8+2+32);
	display_one_word(&NUMBER[log_flash[cur_index].date.Date/10*16],8,16,4,9*8+2+32);
	display_one_word(&NUMBER[log_flash[cur_index].date.Date%10*16],8,16,4,10*8+2+32);
	
	// 时间
	for(i=0;i<2;i++)
	{
		display_one_word(&LOG_TIME[i*32],16,16,6,i*16+2);
	}
	display_one_word(&LOG_TIME[64],8,16,6,2+32);

	display_one_word(&NUMBER[log_flash[cur_index].time.Hours/10*16],8,16,6,1*8+2+32);
	display_one_word(&NUMBER[log_flash[cur_index].time.Hours%10*16],8,16,6,2*8+2+32);
	display_one_word(&LOG_TIME[112],8,16,6,3*8+2+32);
	display_one_word(&NUMBER[log_flash[cur_index].time.Minutes/10*16],8,16,6,4*8+2+32);
	display_one_word(&NUMBER[log_flash[cur_index].time.Minutes%10*16],8,16,6,5*8+2+32);
	display_one_word(&LOG_TIME[160],8,16,6,6*8+2+32);
	display_one_word(&NUMBER[log_flash[cur_index].time.Seconds/10*16],8,16,6,7*8+2+32);
	display_one_word(&NUMBER[log_flash[cur_index].time.Seconds%10*16],8,16,6,8*8+2+32);
	
	
	oled_refresh_gram();
}

void display_sys_reset(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<6;i++)
	{
		display_one_word(&SYS_RESET[i*32],16,16,3,i*16+2);
	}
	for(i=0;i<3;i++)
	{
		display_one_word(&SYS_RESET[i*16+192],8,16,3,i*8+98);
	}
	oled_refresh_gram();
}

void display_sys_exit(void)
{
	uint8_t i;
	All_Screen();
	for(i=0;i<4;i++)
	{
		display_one_word(&SYS_EXIT[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_sys_config_wlan(void)
{
	uint8_t i;
	All_Screen();
	
	stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
	
	for(i=0;i<2;i++)
		display_one_word(&CONFIG_WLAN[i*16],8,16,0,i*8+2);
	for(i=0;i<4;i++)
		display_one_word(&CONFIG_WLAN[i*32+32],16,16,0,i*16+2+16);
	
	for(i=0;i<2;i++)
		display_one_word(&CONFIG_WLAN[i*16+32+128],8,16,2,i*8+2);
	for(i=0;i<3;i++)
		display_one_word(&CONFIG_WLAN[i*32+32+128+32],16,16,2,i*16+2+16);
	
	for(i=0;i<2;i++)
		display_one_word(&CONFIG_WLAN[i*16+32+128+32+96],8,16,4,i*8+2);
	for(i=0;i<3;i++)
		display_one_word(&CONFIG_WLAN[i*32+32+128+32+96+32],16,16,4,i*16+2+16);
	
	if(get_cur_info->wifi_button)
	{
		display_one_word(&HOOK[0],8,16,2,70);
	}else{
		display_one_word(&HOOK[0],8,16,4,70);
	}
	
	oled_refresh_gram();
}

void display_sys_config_wlan_countdown(uint8_t count)
{
	uint8_t i;

	for(i=0;i<5;i++)
	{
		display_one_word(&WLAN_COUNTDOWN[i*32],16,16,1,i*16+24);
	}
	
	if(count<10)
	{
		display_one_word(&SPE_CHART_DEL[0],8,16,4,56);
		display_one_word(&NUMBER[count*16],8,16,4,56+8);
	}else{
		display_one_word(&NUMBER[count/10*16],8,16,4,56);
		display_one_word(&NUMBER[count%10*16],8,16,4,56+8);
	}
	oled_refresh_gram();
}


void display_sys_time(uint8_t count)
{
	uint8_t i;

	for(i=0;i<7;i++)
	{
		display_one_word(&WLAN_TIME_SYS[i*32],16,16,1,i*16+6);
	}
	
	if(count<10)
	{
		display_one_word(&SPE_CHART_DEL[0],8,16,4,56);
		display_one_word(&NUMBER[count*16],8,16,4,56+8);
	}else{
		display_one_word(&NUMBER[count/10*16],8,16,4,56);
		display_one_word(&NUMBER[count%10*16],8,16,4,56+8);
	}
	oled_refresh_gram();
}

void display_sys_unlock_wlan_countdown(uint8_t count)
{
	uint8_t i;

	for(i=0;i<7;i++)
	{
		display_one_word(&WLAN_UNLOCK_COUNTDOWN[i*32],16,16,1,i*16+6);
	}
	
	if(count<10)
	{
		display_one_word(&SPE_CHART_DEL[0],8,16,4,56);
		display_one_word(&NUMBER[count*16],8,16,4,56+8);
	}else{
		display_one_word(&NUMBER[count/10*16],8,16,4,56);
		display_one_word(&NUMBER[count%10*16],8,16,4,56+8);
	}
	oled_refresh_gram();
}

void display_sys_config_sucessful(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&WLAN_SUCESSFUL[i*32],16,16,3,i*16+32);
	}
	
	oled_refresh_gram();
}

void display_sys_config_fail(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&WLAN_FAIL[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}
// wifi启动中...
void display_wifi_wait(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&WLAN_STARTUP[i*16],8,16,3,i*8+20);
	}
	for(i=0;i<2;i++)
	{
		display_one_word(&WLAN_STARTUP[i*32+64],16,16,3,i*16+20+32);
	}
	for(i=0;i<3;i++)
	{
		display_one_word(&WLAN_STARTUP[i*16+64+64],8,16,3,i*8+20+32+32);
	}
	
	oled_refresh_gram();
}

void display_wifi_factory_sucess()
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<12;i++)
	{
		display_one_word(&WLAN_SUCESS_F[i*16],8,16,0,i*8+16);
	}
	
	for(i=0;i<4;i++)
	{
		display_one_word(&KEY_FACTORY[i*32],16,16,2,i*16+16);
	}
	
	oled_refresh_gram();
}

void display_wifi_factory_fail()
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<9;i++)
	{
		display_one_word(&WLAN_FAIL_F[i*16],8,16,0,i*8+28);
	}
	
	for(i=0;i<4;i++)
	{
		display_one_word(&KEY_FACTORY[i*32],16,16,2,i*16+16);
	}
	
	oled_refresh_gram();
}

void display_factory_success(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&FACTORY_S[i*32],16,16,3,i*16+32);
	}
	
	oled_refresh_gram();
}

void display_factory_fail(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&FACTORY_F[i*32],16,16,3,i*16+32);
	}
	
	oled_refresh_gram();
}

void display_factory_sel(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<3;i++)
	{
		display_one_word(&FACTORY_SEL[i*32],16,16,0,i*16+44);
	}
	
	for(i=0;i<2;i++)
		display_one_word(&FACTORY_SEL[i*32+96],16,16,3,i*16+12);
	
	for(i=0;i<3;i++)
		display_one_word(&FACTORY_SEL[i*16+96+64],8,16,3,i*8+12+32);
	
	for(i=0;i<2;i++)
		display_one_word(&FACTORY_SEL[i*32+96+64+48],16,16,3,i*16+12+32+24);
	
	for(i=0;i<2;i++)
		display_one_word(&FACTORY_SEL[i*16+96+64+48+64],8,16,3,i*8+12+32+24+32);
	
	oled_refresh_gram();
}

static uint32_t pow_tool(uint8_t num,uint8_t number)
{
	uint8_t i;
	uint32_t result=1;
	
	for(i=0;i<number;i++)
	{
		result*=num;
	}
	return result;
}

void display_factory_hjs_number(uint32_t count)
{
	uint8_t i;
	uint8_t Bits=0;
	uint8_t numBit;
	uint32_t locCount;
	
	locCount = count;
	All_Screen();
	
	for(i=0;i<6;i++)
	{
		display_one_word(&HJS_COUNT[i*32],16,16,1,i*16+16);
	}
	
	debug_printf("count:%d \r\n",count);
	
	while(locCount!=0)
	{
		locCount/=10;
		Bits++;
	}

	locCount = count;
	for(i=1;i<Bits;i++)
	{
		numBit = count/pow_tool(10,(Bits-i));
		count%=pow_tool(10,(Bits-i));
		display_one_word(&NUMBER[numBit*16],8,16,4,i*8+(128-(Bits*8))/2-8);
	}
	display_one_word(&NUMBER[locCount%10*16],8,16,4,i*8+(128-(Bits*8))/2-8);
	
	oled_refresh_gram();
}

// 网络未连接，请用手机配置网络 
void display_wifi_statue(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<6;i++)
	{
		display_one_word(&WLAN_STATUE[i*32],16,16,2,i*16+16);
	}
	for(i=0;i<8;i++)
	{
		display_one_word(&WLAN_STATUE[i*32+192],16,16,5,i*16);
	}
	
	oled_refresh_gram();
}


void display_wlan_time_sys()
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<6;i++)
	{
		display_one_word(&TIME_SYS[i*32],16,16,3,i*16+4);
	}
	
	for(i=0;i<3;i++)
	{
		display_one_word(&TIME_SYS[i*16+192],8,16,3,i*8+4+96);
	}
	
	oled_refresh_gram();
}

// 该卡未注册
void display_card_non_exit(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<5;i++)
	{
		display_one_word(&CARD_NON_EXIT[i*32],16,16,3,i*16+24);
	}
	
	oled_refresh_gram();
}

void display_card_full(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&CARD_FULL[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}
// 请刷感应卡
void display_card_rigister(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<5;i++)
	{
		display_one_word(&CARD_RIGISTER[i*32],16,16,3,i*16+24);
	}
	oled_refresh_gram();
}
// 该卡已注册
void display_card_exit(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<5;i++)
	{
		display_one_word(&CARD_EXIT[i*32],16,16,3,i*16+24);
	}
	oled_refresh_gram();
}

// 卡片为空
void display_card_null(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&CARD_NULL[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_card_add_sucess(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<4;i++)
	{
		display_one_word(&CARD_ADD_SUC[i*32],16,16,3,i*16+32);
	}
	oled_refresh_gram();
}

void display_battery_update(uint8_t class)
{
	
	display_one_word(&BATTERY_UPDATE[class/10*16],16,8,0,108);  //108
}

void display_battery_warn(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<3;i++)
		display_one_word(&BATTERY_WARN[i*32],16,16,1,40+i*16);  
	for(i=0;i<7;i++)
		display_one_word(&BATTERY_WARN[i*32+96],16,16,4,8+i*16); 
	
	oled_refresh_gram();
}

void display_keyboard_count(uint16_t count)
{
	uint8_t i;
	All_Screen();
	
	debug_printf(" $$$$$$$$$$$$ count: %d\r\n",count);
	
	for(i=0;i<7;i++)
	{
		display_one_word(&KEY_LOCK[i*32],16,16,2,i*16+8);
	}
	if(count>=100)
	{
		display_one_word(&NUMBER[(count/100)*16],8,16,4,52);
		display_one_word(&NUMBER[((count%100)/10)*16],8,16,4,52+8);
		display_one_word(&NUMBER[(count%10)*16],8,16,4,52+8+8);
	}else
	{
		display_one_word(&NUMBER[(count/10)*16],8,16,4,56);
		display_one_word(&NUMBER[(count%10)*16],8,16,4,56+8);
	}
	
	oled_refresh_gram();
}

void display_iphone_num(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<6;i++)
	{
		display_one_word(&IPHONE_NUM[i*32],16,16,0,i*16+16);
	}

	oled_refresh_gram();
}

void display_iphone_result(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<7;i++)
	{
		display_one_word(&IPHONE_RESULT[i*32],16,16,3,i*16+16);
	}

	oled_refresh_gram();
}

void display_iphone_appoint(void)
{
	
	uint8_t i;
	All_Screen();
	
	for(i=0;i<7;i++)
	{
		display_one_word(&IPHONE_APPOINT[i*32],16,16,0,i*16+4);
	}
	display_one_word(&IPHONE_APPOINT[224],8,16,0,116);
	
	display_one_word(&IPHONE_APPOINT[240],16,16,3,16);
	display_one_word(&IPHONE_APPOINT[240+32],8,16,3,16+16);
	display_one_word(&IPHONE_APPOINT[240+32+16],8,16,3,16+16+8);
	display_one_word(&IPHONE_APPOINT[240+32+16+16],8,16,3,16+16+8+8);
	
	display_one_word(&IPHONE_APPOINT[240+32+16+16+16],16,16,3,16+16+8+8+8);
	display_one_word(&IPHONE_APPOINT[240+32+16+16+16+32],8,16,3,16+16+8+8+8+16);
	display_one_word(&IPHONE_APPOINT[240+32+16+16+16+32+16],8,16,3,16+16+8+8+8+16+8);
	
	oled_refresh_gram();
}


void display_iphone_nonexistent(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<7;i++)
	{
		display_one_word(&IPHONE_NONEXISTENT[i*32],16,16,3,i*16+8);
	}
	oled_refresh_gram();
}

void display_iphone_offline(void)
{
	uint8_t i;
	All_Screen();
	
	for(i=0;i<3;i++)
	{
		display_one_word(&IPHONE_OFFLINE[i*16],8,16,3,i*8+36);
	}
	for(i=0;i<2;i++)
	{
		display_one_word(&IPHONE_OFFLINE[i*32+48],16,16,3,i*16+36+24);
	}
	oled_refresh_gram();
}

void oled_refresh_gram()
{
	uchar page_number,column_number;
  for(page_number=0;page_number<8;page_number++)
	{  
		Write_Command(0xb0+page_number);
    Write_Command(0x10);
    Write_Command(0x00);      
		for(column_number=0;column_number<128;column_number++)
			Write_Data(OLED_GRAM[column_number][page_number]);
	}		
}

void oled_refresh_gram_with_arg(uint8_t page)
{
	uchar page_number,column_number;
  for(page_number=page;page_number<8;page_number++)
	{  
		Write_Command(0xb0+page_number);
    Write_Command(0x10);
    Write_Command(0x00);      
		for(column_number=0;column_number<128;column_number++)
			Write_Data(OLED_GRAM[column_number][page_number]);
	}   
}

void oled_refresh_gram_with_multi_arg(uint8_t page,uint8_t start_col,uint8_t end_col,uint8_t contrast_sel)
{
	uchar page_number,column_number;
	
	//debug_printf("page:%d\r\n",page);
	for(page_number=page;page_number<=page+1;page_number++)
	{
		Write_Command(0xb0+page_number);
		Write_Command(0x10);
		Write_Command(0x00);      
		for(column_number=0;column_number<128;column_number++)
		{
			if(column_number>=start_col&&column_number<=end_col&&contrast_sel)
				Write_Data(~OLED_GRAM[column_number][page_number]);
			else
				Write_Data(OLED_GRAM[column_number][page_number]);
		}
	}
}

void display_date_update(uint8_t page,uint8_t start_col,uint8_t end_col,uint8_t number)
{
	uchar page_number,column_number;
	uint8_t count=0;
	
	for(page_number=page;page_number<=page+1;page_number++)
	{
		Write_Command(0xb0+page_number);
		Write_Command(0x10);
		Write_Command(0x00);      
		for(column_number=0;column_number<128;column_number++)
		{
			if(column_number>=start_col&&column_number<end_col)
			{
				OLED_GRAM[column_number][page_number]=NUMBER[number*16+count++];
				Write_Data(OLED_GRAM[column_number][page_number]);
			}
			else
			{
				Write_Data(OLED_GRAM[column_number][page_number]);
			}
		}
	}
}




