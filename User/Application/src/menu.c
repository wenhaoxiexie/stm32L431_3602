#include "menu.h"
#include "hsj08.h"
#include "adc.h"
#include "fpcst90x.h"
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "ssd1305.h"
#include "i2c.h"
#include "tim.h"
#include "iwdg.h"

extern KEY_STATE get_cur_number;
extern uint8_t finger_press_flag;
extern uint8_t try_count;
extern uint8_t fp_power_flag;
extern uint8_t facteryReset;
extern uint32_t battery_value;
extern int8_t battery_class;
extern uint8_t prying_flag;
extern uint8_t stopModeFlag;
extern uint8_t OLED_GRAM[128][8];

static RTC_DateTypeDef date_record;
static RTC_TimeTypeDef time_record;
static RTC_DateTypeDef date_record_update;
static RTC_TimeTypeDef time_record_update;
static	uint16_t delay_count;
extern uint8_t factoryFlag;
extern uint8_t prying_flag;

 static void UsartConfig_STOP(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	// 配置串口1
//	__HAL_RCC_USART1_CLK_ENABLE();

//  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
//	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(USART1_IRQn);
//	MX_USART1_UART_Init();
	
	// 配置串口2
	__HAL_RCC_USART2_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
	MX_USART2_UART_Init();
	
	// 配置串口3
	__HAL_RCC_USART3_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART3_IRQn);
	MX_USART3_UART_Init();
}

static void IICConfig_STOP(void)
{
	__HAL_RCC_I2C1_CLK_ENABLE();
	MX_I2C1_Init();
	HAL_I2C_MspInit(&hi2c1);
	
	__HAL_RCC_I2C3_CLK_ENABLE();
	MX_I2C3_Init();
	HAL_I2C_MspInit(&hi2c3);
}

static void SPIConfig_STOP(void)
{
	__HAL_RCC_SPI2_CLK_ENABLE();
	MX_SPI2_Init();
	HAL_SPI_MspInit(&hspi2);
}


static void ADCConfig_STOP(void)
{
	__HAL_RCC_ADC_CLK_ENABLE();
	MX_ADC1_Init();
	HAL_ADC_MspInit(&hadc1);
	ADC_Enable(&hadc1);
}

//static void RTCConfig_STOP(void)
//{
//	//__HAL_RCC_RTC_ENABLE();
//	MX_RTC_Init();
//	HAL_RTC_MspInit(&hrtc);
//}

void voice_repeat(uint8_t addr)
{
	sys_delay_us(5000);
	
	set_win6170(0XF3);
	sys_delay_us(2000);
	set_win6170(addr);
	
	sys_delay_us(5000);
	
	set_win6170(0XF3);
	sys_delay_us(2000);
	set_win6170(48);
	
	sys_delay_us(5000);
	
	set_win6170(0XF3);
	sys_delay_us(2000);
	set_win6170(47);
	
	sys_delay_us(5000);
}

void multi_mode_init(void)
{
	uint16_t status;
	
  // 延时函数，精确到us 时钟源32khz
	//delay_init(32); 
	
	//喇叭上电
	horn_powerOn();
	// oled屏上电
	oled_powerOn();
	// led上电
	led_powerOn();
	
	// LED灯接口
	send_bytes(KEY_ALL_ON);
	
	// OLED显示屏接口
	Reset_IC();
	init_ic();
	oled_init();

	// 触摸板接口函数
	APT_Init();
	
	// flash 初始化
	flash_init();
	//factory_reset();
	
	// NFC9883 init 初始化失败会进入死循环
	status =nfc9883_init();
	// 可能出现多次校验
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	debug_printf("NFC9883 init status :%x\r\n",status);
	
	LOCK();
	display_menu();
	set_win6170(0x01);
	
	// 开启中断
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_All);

	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	// 指纹上电
	fp_power_on();
	// 指纹第一次上电后进入睡眠模式，中断使能 
	fp_sleep();
	// wifi 初始化
	wifi_init();
}

static uint8_t factory_key_count(uint8_t *buff,uint8_t len)
{
	uint8_t i,j;
	uint8_t keyTemplete[12]={0,1,2,3,4,5,6,7,8,9,10,11};
	
	for(i=0;i<len;i++)
		printf("%d ",buff[i]);
	
	printf("\r\n");
	
	for(i=0;i<12;i++)
	{
		for(j=0;j<len;j++)
		{
			if(keyTemplete[i]==buff[j])
			{
				break;
			}
		}
		if(j==len)
			return LOCK_ERROR;
	}
	
	return LOCK_OK;
}

static uint8_t factory_test(void)
{
	uint8_t mac[16]="FFFFFFFFFFFFFFF";
	uint8_t reverse=0;
	
	// 按键、指纹、NFC测试
	uint16_t pLen;
	uint8_t pResult;
	uint8_t pPakege[128];
	uint16_t delayCount=20000;
	FP_StatusTypeDef reValue;
	uint16_t factory_state=0;
	uint8_t keyIndex=0;
	uint8_t keyValue[255]={0xff};
	
	//      测试项
	// 产测：1 老化：2
	display_factory_sel();
	do{
		if(get_cur_number.key_prees)
		{
			get_cur_number.key_prees=0;
			if(get_cur_number.number==1)
				break;
			else if(get_cur_number.number==2)
			{
				// 开启定时器7
				HAL_TIM_Base_Start_IT(&htim7);
			}
		}
	}while(1);
	
	// 产测标志
	factoryFlag =1;
	// 开启定时器7
	HAL_TIM_Base_Start_IT(&htim7);
	// 产测开始
	set_win6170(66);
	// 1. 打开wifi电源开关
	wifi_power_on();
	
	// 显示屏、LED测试 、wifi结果查询
	do{
		if(get_cur_number.key_prees)
		{
			get_cur_number.key_prees=0;
			
			if(wifi_get_mac(mac)==HAL_OK&&strncmp((char*)mac,"FFFFFFFFFFFFFFF",6)!=NULL)
			{
				display_wifi_factory_sucess();
				set_win6170(0xf3);
				sys_delay_us(2000);
				set_win6170(72);
				sys_delay_us(5000);
				set_win6170(0xf3);
				sys_delay_us(2000);
				set_win6170(67);
				factory_state |=0x08;
				
			}else{
				display_wifi_factory_fail();
				set_win6170(0xf3);
				sys_delay_us(2000);
				set_win6170(72);
				sys_delay_us(5000);
				set_win6170(0xf3);
				sys_delay_us(2000);
				set_win6170(68);
			}
			debug_printf("mac: %s\r\n",mac);
			break;
		}
		if(reverse%2)
		{
			send_bytes(KEY_ALL_OFF);
			memset(OLED_GRAM,0x0,sizeof(OLED_GRAM));
			oled_refresh_gram();
		}else{
			send_bytes(KEY_ALL_ON);
			memset(OLED_GRAM,0xff,sizeof(OLED_GRAM));
			oled_refresh_gram();
		}
		reverse++;
		sys_delay_us(1000000);
	}while(1);
	
	send_bytes(KEY_ALL_ON);

	do{	
		if(finger_press_flag)
		{
		  finger_press_flag=0;
			pLen = 0;
			memset(pPakege,0x00,128);
			reValue = fp_tarns_recv_command(MASK_INS, PS_GetImage, NULL, &pLen, pPakege, &pResult,500);
			if(reValue==FP_OK&&pResult==0x00) // 检测到手指
			{
				display_operate_sucessful();
				
				set_win6170(0xf3);
				sys_delay_us(2000);
				set_win6170(70);
				sys_delay_us(5000);
				set_win6170(0xf3);
				sys_delay_us(2000);
				set_win6170(67);
				
				factory_state |=0x01;
				debug_printf("factory_state:%x \r\n",factory_state);
				if(factory_state==0x1f)
					return LOCK_OK;
			}
			fp_sleep();
		}
		
		if(nfc_temp.irq_flag)
		{
			debug_printf(" len:%d \r\n",nfc_temp.len);
			nfc_temp.irq_flag=0;
			if(nfc_temp.len>0) // 检测到NFC卡片
			{
				display_operate_sucessful();
				
				set_win6170(0xf3);
				sys_delay_us(2000);
				set_win6170(71);
				sys_delay_us(5000);
				set_win6170(0xf3);
				sys_delay_us(2000);
				set_win6170(67);
				
				factory_state |=0x02;
				debug_printf("factory_state:%x \r\n",factory_state);
				if(factory_state==0x1f)
					return LOCK_OK;
			} 
		}
		
		if(get_cur_number.key_prees)
		{
			get_cur_number.key_prees=0;
			
			delayCount = 20000;
			
			keyValue[keyIndex]=get_cur_number.number;

			if(keyValue[keyIndex]==10)
			{
				display_spe_char_factory(0,keyIndex);
			}
			else if(keyValue[keyIndex]==11)
			{
				display_spe_char_factory(1,keyIndex);
			}else{
				display_number_all(keyValue[keyIndex],keyIndex);
			}
			keyIndex++;
			debug_printf(">>>>>>>>> keyIndex: %d \r\n",keyIndex);
			if(keyIndex>=12)
			{
				if(factory_key_count(keyValue,keyIndex)==LOCK_OK)
				{
					display_operate_sucessful();
					
					set_win6170(0xf3);
					sys_delay_us(2000);
					set_win6170(69);
					sys_delay_us(5000);
					set_win6170(0xf3);
					sys_delay_us(2000);
					set_win6170(67);
					
					factory_state |=0x04;
					debug_printf("factory_state:%x \r\n",factory_state);
					if(factory_state==0x1f)
						return LOCK_OK;
				}else{
					debug_printf("************key board test failed***************\r\n");
				}
			}
			
		}
		if(prying_flag)
		{
			factory_state |= 0x10;
			if(factory_state==0x1f)
				return LOCK_OK;
		}
		
		sys_delay_us(1000);
	}while(--delayCount);
	
	return LOCK_ERROR;
}

uint8_t keyboard_login(uint8_t* permision)	
{
	uint8_t status;
	uint8_t reValue;
	uint8_t number_cur=0;
	uint8_t password[32]={0};
	
	// 第一次按压唤醒主芯片
	if(get_cur_number.key_prees==1||facteryReset)
	{
		
			// 键盘锁定
			REVAL_CHECK_SUCCESS_FCT(status,keyboard_lock(&try_count));
			
			delay_count=50000;
		
			facteryReset=0;
			stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
			// 第一个用户只能是管理员身份
			if(get_cur_info->cur_index_number<1)
			{
				set_win6170(53);
				display_passwod_admin();
			}
			else
				display_passwod_put();
			
			do{
				if(nfc_temp.irq_flag||finger_press_flag)
					return LOCK_ERROR;
				// 记录按键值
				if(get_cur_number.key_prees==1)
				{
					delay_count=50000;
					get_cur_number.key_prees=0;
				
					if(get_cur_number.number==10)
					{
						debug_printf("number_cur: %d  \r\n",number_cur);
						if(number_cur<=0)
						{
							display_menu();
							return LOCK_ERROR;
						}
						// 回删
						display_spe_char_del(number_cur--);

					}
					else if(get_cur_number.number==11)
					{
						//产测函数
						
						if(password[0]==6&&password[1]==6&&password[2]==6&&get_cur_info->cur_index_number<1)
						{
							debug_printf(" factory start...\r\n");
							reValue = factory_test();
							// 关闭定时器7
							HAL_TIM_Base_Stop_IT(&htim7);
							factoryFlag =0;
							prying_flag=0;
							if(reValue==LOCK_OK)
							{
								sys_delay_us(2000000);
								set_win6170(67);
								display_factory_success();
							}else{
								sys_delay_us(2000000);
								set_win6170(68);
								display_factory_fail();
							}
							return LOCK_ERROR;
						}
						
						//密码验证函数
						status = keyboard_password_confirm(password,number_cur,permision);
						if(status==LOCK_OK)
						{
							HAL_NVIC_DisableIRQ(EXTI2_IRQn);
								// 开锁
							UNLOCK();
							__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_All);
							HAL_NVIC_EnableIRQ(EXTI2_IRQn);
							Tim2_Start();
							if(*permision==ANDMI)
								set_win6170(46);
							else
								set_win6170(40);
							try_count=0;
							sys_delay_us(1000000);
							return LOCK_OK;
						}
						else if(status==LOCK_EXIT)
						{
								return LOCK_ERROR;
						}
						else
						{
							try_count++;
							set_win6170(41);
							display_check_failed();
							sys_delay_us(1000000);
							display_menu();
							
							if(try_count==4)
							{
								HAL_RTC_GetTime(&hrtc, &time_record, RTC_FORMAT_BIN);HAL_RTC_GetDate(&hrtc, &date_record, RTC_FORMAT_BIN);
							}
							else if(try_count==5)
							{
								HAL_RTC_GetTime(&hrtc, &time_record_update, RTC_FORMAT_BIN);HAL_RTC_GetDate(&hrtc, &date_record_update, RTC_FORMAT_BIN);
								debug_printf("%d-%d-%d \r\n",date_record_update.Year,date_record_update.Month,date_record_update.Date);
								debug_printf("%d:%d:%d \r\n",time_record_update.Hours,time_record_update.Minutes,time_record_update.Seconds);
								if(date_record_update.Year==date_record.Year&&date_record_update.Month==date_record_update.Month&&date_record_update.Date==date_record.Date)
								{
									if(time_record_update.Hours==time_record.Hours)
									{
										if((time_record_update.Minutes*60+time_record_update.Seconds)-(time_record.Minutes*60+time_record.Seconds)>300)
											try_count=0;
									}
								}
							}
							else if(try_count==6)
								try_count=1;
							return LOCK_ERROR;
						}
					}
					else
					{
						// 6~10位密码
						if(number_cur<20)
						{
							password[number_cur++] = get_cur_number.number;
							display_spe_char(number_cur);
						}
					}
				}
				sys_delay_us(100);
			}while(--delay_count);
			display_menu();
	}
	return LOCK_ERROR;
}

uint8_t finger_login(uint8_t* permision)
{
	uint8_t i;
	uint8_t pPakege[128];
	uint8_t pResult;
	uint8_t conCount;
	uint8_t fp_index;
	uint8_t wifi_index;
	char lock_time[32];
	uint16_t pLen;
	FP_StatusTypeDef reValue;

	
	if(finger_press_flag==1)
	{
		debug_printf(" finger_press_flag:%d \r\n",finger_press_flag);
		finger_press_flag=0;

		if(get_cur_info->cur_index_number<1)
		{
			return LOCK_ERROR;
		}
		APT_KeyEnableSel(KDR0_CH_ALL_DISA,KDR1_CH_ALL_DISA);
		
		stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
		stm_flash_read(ADDR_FLASH_PAGE_58,(uint8_t*)finger_flash,FINGER_SIZE);
		
		fp_led_config();
		fp_led_red();
		
		conCount=0;
		do{
				pLen = 0;
				memset(pPakege,0x00,128);
				reValue = fp_tarns_recv_command(MASK_INS, PS_GetImage, NULL, &pLen, pPakege, &pResult,500);
				if(reValue==FP_OK&&pResult==0x00) // 检测到手指
					break;
				conCount++;
		}while(conCount<3);
		if(conCount>=3)
			goto ERROR;
		// 提取特征值
		pPakege[0]=1;
		pLen=1;
		reValue = fp_tarns_recv_command(MASK_INS, PS_GenChar, pPakege, &pLen, pPakege, &pResult,1000);
		if(reValue!=FP_OK||pResult!=0)
				goto ERROR;
		// 搜索指纹
		pPakege[0]= 1;
    pPakege[1]= 0;
    pPakege[2]= 0;
    pPakege[3]= (1024>>8);
    pPakege[4]= (1024&0xff);
    pLen=5;
		reValue = fp_tarns_recv_command(MASK_INS,0x1B, pPakege,&pLen, pPakege,&pResult,2000);
		debug_printf("reValue: %d  %d [ %02x ] \r\n",reValue,pResult,pPakege[11]);
		if(reValue!=FP_OK||pResult!=0)
				goto ERROR;
		else{
			
				fp_led_green();
			
				fp_index = pPakege[11];
				for(i=0;i<FINGER_NUM;i++)
				{
					if(finger_flash[i].del_flag==0)
					{
						if(finger_flash[i].index==fp_index)
							break;
					}
				}
				if(i>=FINGER_NUM)
				{
					goto ERROR;
				}
				*permision =finger_flash[i].user_permision;
				set_win6170(0XFE);
				sys_delay_us(2000);
				HAL_NVIC_DisableIRQ(EXTI2_IRQn);
				// 开锁
				UNLOCK();
				__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_All);
				HAL_NVIC_EnableIRQ(EXTI2_IRQn);
				Tim2_Start();
				
				display_confirm(FINGER_TYPE,*permision,fp_index);
				
					// 记录用户操作
				if(get_cur_info->cur_log_number>=LOG_NUM)
				{
					get_cur_info->cur_log_number=0;
				}
				HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
				HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
				sys_log_record(fp_index,OPEN_DOOR,*permision,FINGER_TYPE,get_cur_info->cur_log_number);
				
				get_cur_info->cur_log_number++;
				
				// 电池电量统计
				battery_value = sys_battery_get();
				battery_class = wifi_upload_battery(battery_value);
				
				get_cur_info->cur_battery = battery_class;
				
				stm_flash_erase_write(ADDR_FLASH_PAGE_100,(uint64_t*)get_cur_info,CUR_INDEX_SIZE);
				// 开锁上报信息 
				stm_flash_read(ADDR_FLASH_PAGE_70,(uint8_t*)wifi_flash,WIFI_SIZE);
				wifi_power_on();
				
				wifi_index =wifi_flash->index;
				wifi_flash[wifi_index].exist=1;
					
				sprintf(lock_time,"20%02d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year,sdatestructure.Month,
																							sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);
				if(battery_class<=10)
				{
					set_win6170(62);
					display_battery_warn();
					sys_delay_us(3000000);
					sprintf(wifi_flash[wifi_index].upload_info,"%d%%%d%%%s%%",WARN_UPLOAD,LOW_POWER_WARN,lock_time);
					display_confirm(FINGER_TYPE,*permision,fp_index);
				}else																
					sprintf(wifi_flash[wifi_index].upload_info,"%d%%%d%%%s%%",BATTERY_UPLOAD,battery_class,lock_time);
				if(wifi_flash->index<99)
				{
					wifi_flash->index++;
					wifi_index++;
				}
				if(*permision==ANTI_HAK)
				{
						wifi_flash[wifi_index].exist=1;
						sprintf(wifi_flash[wifi_index].upload_info,"%d%%%d%%%s%%",WARN_UPLOAD,
																							ANTI_HIJACK_WARN,lock_time);
						if(wifi_flash->index<99)
							wifi_flash->index++;
				}else{
					if(get_cur_info->wifi_button)
					{
						wifi_flash[wifi_index].exist=1;
						sprintf(wifi_flash[wifi_index].upload_info,"%d%%%d%%%d%%%d%%%d%%%s%%",OPEN_UPLOAD,
																								fp_index,FINGER_TYPE,*permision,OPEN_DOOR,lock_time);
						if(wifi_flash->index<99)
							wifi_flash->index++;
					}
				}
	
				// 保存上报信息到flash
				stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);
							
				prying_flag=0;
				set_win6170(get_cur_info->cur_vol_size);
				try_count=0;
				if(*permision==ANDMI)
					set_win6170(46);
				else
					set_win6170(40);
				sys_delay_us(1500000);
				APT_KeyEnableSel(KDR0_CH_ALL_EN,KDR1_CH_ALL_EN);
				fp_led_white();
				fp_led_shutdown();
				
				return LOCK_OK;
		}
ERROR:
		
			fp_led_white();
			fp_led_shutdown();
		
			set_win6170(41);
			display_finger_check_fail();
			sys_delay_us(1000000);
			display_menu();
			fp_sleep();
			APT_KeyEnableSel(KDR0_CH_ALL_EN,KDR1_CH_ALL_EN);
			return LOCK_ERROR;
	}
	return LOCK_ERROR;
}

uint8_t card_login(uint8_t* permision)
{
	uint8_t i,j;
	uint8_t reValue;
	char lock_time[32];
	
	if(nfc_temp.irq_flag==1)
	{
		//HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
		if(get_cur_info->cur_index_number<1)
		{
			nfc_temp.irq_flag=0;
			//HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
			return LOCK_ERROR;
		}
		
		//send_bytes(KEY_ALL_OFF);
		APT_KeyEnableSel(KDR0_CH_ALL_DISA,KDR1_CH_ALL_DISA);
		
		nfc_temp.irq_flag=0;
		debug_printf(" len:%d \r\n",nfc_temp.len);
		// 读取本地保存的卡片数据
		stm_flash_read(ADDR_FLASH_PAGE_60,(uint8_t*)nfc_flash,CARD_SIZE);
		// 读取本地全局区数据
		stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
		
		// 遍历是否有相同的卡片id
		for(i=0;i<CARD_NUM/*get_cur_info->cur_card_index*/;i++)
		{
			if(nfc_flash[i].del_flag==0)
			{
				if(nfc_flash[i].uid_len==nfc_temp.len)
				{
					for(j=0;j<nfc_temp.len;j++)
					{
						if(nfc_flash[i].uid[j]!=nfc_temp.guid[j])
							break;
					}
				}
				if(j==nfc_temp.len)
				{
					*permision =nfc_flash[i].user_permision;
					
					display_confirm(CARD_TYPE,*permision,nfc_flash[i].index);
					HAL_NVIC_DisableIRQ(EXTI2_IRQn);
					// 开锁
					UNLOCK();
					__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_All);
					HAL_NVIC_EnableIRQ(EXTI2_IRQn);
					Tim2_Start();
					
					// 记录用户操作
					if(get_cur_info->cur_log_number>=LOG_NUM)
					{
						get_cur_info->cur_log_number=0;
					}
				
					HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
					HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
					
					sys_log_record(nfc_flash[i].index,OPEN_DOOR,*permision,CARD_TYPE,get_cur_info->cur_log_number);
						
					get_cur_info->cur_log_number++;
					
					// 电池电量统计
					battery_value = sys_battery_get();
					battery_class = wifi_upload_battery(battery_value);
					
					get_cur_info->cur_battery = battery_class;
					
					stm_flash_erase_write(ADDR_FLASH_PAGE_100,(uint64_t*)get_cur_info,CUR_INDEX_SIZE);
					// 开锁上报信息 
					stm_flash_read(ADDR_FLASH_PAGE_70,(uint8_t*)wifi_flash,WIFI_SIZE);
					wifi_power_on();
					
					reValue =wifi_flash->index;
					wifi_flash[reValue].exist=1;
					debug_printf("reValue:%d  \r\n",reValue);
					
					sprintf(lock_time,"20%02d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year,sdatestructure.Month,
																								sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);
					if(battery_class<=10)
					{
						set_win6170(62);
						display_battery_warn();
						sys_delay_us(3000000);
						// 低电量报警
						sprintf(wifi_flash[reValue].upload_info,"%d%%%d%%%s%%",WARN_UPLOAD,LOW_POWER_WARN,lock_time);
						display_confirm(CARD_TYPE,*permision,nfc_flash[i].index);
					}else
						sprintf(wifi_flash[reValue].upload_info,"%d%%%d%%%s%%",BATTERY_UPLOAD,battery_class,lock_time);
					
					if(wifi_flash->index<99)
					{
						reValue++;
						wifi_flash->index++;
					}
					
					if(get_cur_info->wifi_button)
					{
						wifi_flash[reValue].exist=1;
						sprintf(wifi_flash[reValue].upload_info,"%d%%%d%%%d%%%d%%%d%%%s%%",OPEN_UPLOAD,
																								nfc_flash[i].index,CARD_TYPE,*permision,OPEN_DOOR,lock_time);
						if(wifi_flash->index<99)
							wifi_flash->index++;
					}
					
					// 保存上报信息到flash
					stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);
				
					prying_flag=0;
					set_win6170(get_cur_info->cur_vol_size);
					try_count=0;
					if(*permision==ANDMI)
						set_win6170(46);
					else
						set_win6170(40);
					sys_delay_us(1000000);
					return LOCK_OK;
				}
			}
		}
		set_win6170(41);
		display_card_non_exit();
		sys_delay_us(1000000);
		display_menu();
		APT_KeyEnableSel(KDR0_CH_ALL_EN,KDR1_CH_ALL_EN);
	}
	return LOCK_ERROR;
}

uint8_t main_menu()
{
	
START:
	display_mainmenu();
	send_bytes(KEY_MAIN_MENU);
	APT_KeyEnableSel(KDR0_CH_1_2_3_4_10_11_EN,KDR1_CH_1_2_3_4_10_11_EN);
	
RECALL:
	
	voice_repeat(12);
	
	delay_count=1300;
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			delay_count=1300;
			switch(get_cur_number.number){
				case 1:
					// 添加用户
					if(user_add_menu()==LOCK_OK)
						goto START;
					else
						return LOCK_OUTTIME;
				case 2:
					// 删除用户
					if(user_delete_menu()==LOCK_OK)
						goto START;
					else
						return LOCK_OUTTIME;
				case 3:
					// 系统设置
					if(sys_set_menu()==LOCK_OK)
						goto START;
					else
						return LOCK_OUTTIME;
				case 4:
					// 系统信息
					if(sys_info_menu()==LOCK_OK)
						goto START;
					else
						return LOCK_OUTTIME;
				case 10:
					return LOCK_OK;
				case 11:
					goto RECALL;
				default:
					break;
			}
		}
		sys_delay_us(10000);
	}while(--delay_count);

	return LOCK_OUTTIME;
}

float sys_battery_get(void)
{
		uint32_t adc_value;
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 50);
		if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
		{
			adc_value = HAL_ADC_GetValue(&hadc1);
			
			return (adc_value*2+0.7)*3600/4096;
		}
		sys_delay_us(1000000);
		return 0;
}

uint8_t keyboard_lock(uint8_t *try_count)
{
	
	uint32_t d_value;
	
	debug_printf("try_count: %d \r\n",*try_count);
	if(*try_count>=5)
	{
		get_cur_number.key_prees=0;
		debug_printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& LOCK &&&&&&&&&&&&&&&&&&&&&&&&&&&\r\n");
		HAL_RTC_GetTime(&hrtc, &time_record, RTC_FORMAT_BIN);HAL_RTC_GetDate(&hrtc, &date_record, RTC_FORMAT_BIN);
		
		if(date_record.Year==date_record_update.Year&&date_record.Month==date_record_update.Month&&date_record.Date==date_record_update.Date)
		{
			if(time_record.Hours==time_record_update.Hours)
			{
		
				d_value = (time_record.Minutes*60+time_record.Seconds)-(time_record_update.Minutes*60+time_record_update.Seconds);
				debug_printf(" d_value: %d\r\n",d_value);
				if(d_value>300)
				{
					*try_count=0;
					return LOCK_OK;	
				}else{
					set_win6170(64);
				}
				display_keyboard_count(300-d_value);
				sys_delay_us(1000000);
				display_menu();
				if(*try_count==5)
					wifi_upload_tryErr_info();
				*try_count+=1;
				return LOCK_ERROR;
			}
		}
	}
	return LOCK_OK;
}

void display_menu(void)
{
	All_Screen();
	
	HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
	debug_printf("20%02d-%02d-%02d %02d:%02d:%02d \r\n",sdatestructure.Year, sdatestructure.Month, sdatestructure.Date,
																										 stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);
//	battery_value = sys_battery_get();
//	debug_printf(" battery_value:%d  \r\n",battery_value);
//	battery_class = wifi_upload_battery(battery_value);
	
//	debug_printf(" battery_class:%d  \r\n",battery_class);
//	if(battery_class<=10)
//	{
//		set_win6170(62);
//		display_battery_warn();
//		sys_delay_us(3000000);
//	}
	display_company_name();  // 显示公司log
	display_battery_update(battery_class); // 显示电量
	display_date_ymd(sdatestructure.Year,sdatestructure.Month,sdatestructure.Date); // 显示月份和天数
}

void SYSCLKConfig_STOP(uint16_t GPIO_Pin)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

  uint32_t pFLatency = 0;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Get the Oscillators configuration according to the internal RCC registers */
  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Get the Clocks configuration according to the internal RCC registers */
  HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &pFLatency);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, pFLatency) != HAL_OK)
  {
    Error_Handler();
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
	MX_GPIO_Init();
	UsartConfig_STOP();
	IICConfig_STOP();
	SPIConfig_STOP();
	ADCConfig_STOP();
	//RTCConfig_STOP();
	
	APT_Init();
	
		//喇叭上电
	horn_powerOn();
	// oled屏上电
	oled_powerOn();
	// led上电
	led_powerOn();

	// OLED显示屏接口
	Reset_IC();
	init_ic();

	flash_init();
	
	if(get_cur_info->cur_index_number<1)
	{
		facteryReset=1;
	}
	
	// LED灯接口
	send_bytes(KEY_ALL_ON);
	debug_printf(" sys voice :%d \r\n",get_cur_info->cur_vol_size);
	set_win6170(get_cur_info->cur_vol_size);
	sys_delay_us(2000);
	if(GPIO_Pin != GPIO_PIN_9&&GPIO_Pin != GPIO_PIN_12&&facteryReset!=1)
		set_win6170(0x01);
	
	if(get_cur_info->cur_index_number>=1&&(GPIO_Pin==GPIO_PIN_3||GPIO_Pin==GPIO_PIN_4||GPIO_Pin==GPIO_PIN_12))
		display_menu();
	
	// nfc init
	//nfc9883_init();
	
	// 喂狗
	HAL_IWDG_Refresh(&hiwdg);
	
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
		// 防撬报警
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)!=1)
	{
		debug_printf("anti_hijk  ...\r\n");
		__HAL_GPIO_EXTI_GENERATE_SWIT(GPIO_PIN_4);
	}
	
	sys_delay_us(200000);
	
	debug_printf("################# wakeup \r\n");
}

void sleep_mode(void)
{
	
	display_sys_exit();
	APT_KeyEnableSel(KDR0_CH_ALL_DISA,KDR1_CH_ALL_DISA);
	// 系统退出
	set_win6170(59);
	
	// 本地信息上报
	wifi_upload_all_info();
	
	// oled清屏断电
	//Reset_IC();
	All_Screen();
	oled_refresh_gram();
	oled_powerOff();
	
	// 模块断电
	//wifi 断电
	wifi_power_off();
	// 指纹断电
	if(HAL_GPIO_ReadPin(FPPower_GPIO_Port,FPPower_Pin)==GPIO_PIN_SET)
	{
		debug_printf(" finger power off \r\n");
		finger_press_flag=0;
		fp_sleep();
	}

	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);	
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);	
	

	APT_WORK_125HZ_VAL();
	
	// led断电
	led_powerOff();
	
	// 喇叭断电
	horn_powerOff();

	// 关闭ADC
	HAL_ADC_Stop(&hadc1);
	ADC_Disable(&hadc1);
	
	// 关闭外设Spi2
	__HAL_SPI_DISABLE(&hspi2);
	HAL_SPI_MspDeInit(&hspi2);
	
	// 关闭IIC1
	__HAL_I2C_DISABLE(&hi2c1);
	HAL_I2C_MspDeInit(&hi2c1);
	
	__HAL_I2C_DISABLE(&hi2c3);
	HAL_I2C_MspDeInit(&hi2c3);

  // 关闭串口1
  //__HAL_UART_DISABLE(&huart1);
	//HAL_UART_MspDeInit(&huart1);

//  关闭串口2
  __HAL_UART_DISABLE(&huart2);
  HAL_UART_MspDeInit(&huart2);

//  关闭串口3
  __HAL_UART_DISABLE(&huart3);
	HAL_UART_MspDeInit(&huart3);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 初始化GPIOA 引脚
//	GPIO_InitStructure.Pin = GPIO_PIN_9|GPIO_PIN_10;  // IIC1 外部上拉
//	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;  // GPIO_MODE_INPUT
//	GPIO_InitStructure.Pull = GPIO_NOPULL;       // GPIO_PULLUP
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	GPIO_InitStructure.Pin = GPIO_PIN_11; // 电源开关 外部下拉
//	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	GPIO_InitStructure.Pin = GPIO_PIN_1; // 电源开关 外部下拉
//	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==0)
	{
		GPIO_InitStructure.Pin = GPIO_PIN_4; // 防撬外部中断
		GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
		GPIO_InitStructure.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	
	GPIO_InitStructure.Pin = GPIO_PIN_All;
	GPIO_InitStructure.Pin &= (~(/*GPIO_PIN_1|*/GPIO_PIN_4/*|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14*/|GPIO_PIN_12));//  中断引脚( P04:防撬  P12: nfc )
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 初始化GPIOB 引脚
//	GPIO_InitStructure.Pin = GPIO_PIN_1|GPIO_PIN_4;        // 电源开关 外部下拉
//	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = GPIO_PIN_2;        // 指纹电源开关 外部下拉 
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;  // GPIO_MODE_INPUT
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;    // GPIO_PULLDOWN
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	uint8_t value;
//		value = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12);
//	debug_printf("value12:%d \r\n",value);
//	
//		value = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13);
//	debug_printf("value13:%d \r\n",value);
//	
//		value = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14);
//	debug_printf("value14:%d \r\n",value);
//	
//		value = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15);
//	debug_printf("value15:%d \r\n",value);
	
	GPIO_InitStructure.Pin = GPIO_PIN_12;  // spi引脚
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;  // GPIO_MODE_ANALOG  GPIO_MODE_OUTPUT_PP
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;  // spi引脚
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;  // GPIO_MODE_ANALOG 
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;      // GPIO_NOPULL
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	// 关闭NFC 的引脚配置
//	GPIO_InitStructure.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;  // spi引脚
//	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
//	GPIO_InitStructure.Pull = GPIO_PULLUP;
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
		
//	debug_printf("ENTER STOP2 MODE... \r\n");
//	uint8_t revalue1,revalue2;
//	revalue1 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6);
//	revalue2 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7);
//	debug_printf(" [%d:%d] \r\n",revalue1,revalue2);
	
	GPIO_InitStructure.Pin = GPIO_PIN_All;
	GPIO_InitStructure.Pin &= ~(GPIO_PIN_2|/*GPIO_PIN_1|*/GPIO_PIN_3/*|GPIO_PIN_4*/|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_13|GPIO_PIN_15);  
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// 初始化GPIOC 引脚
//	GPIO_InitStructure.Pin = GPIO_PIN_12|GPIO_PIN_13;  // 电源开关  外部下拉
//	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
//	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = GPIO_PIN_All;
	GPIO_InitStructure.Pin &= ~(GPIO_PIN_9/*|GPIO_PIN_12|GPIO_PIN_13*/);  // 中断按键
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// 初始化GPIOD 引脚
	GPIO_InitStructure.Pin = GPIO_PIN_All;
	GPIO_InitStructure.Pin &= ~GPIO_PIN_2; 
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

	stopModeFlag=1;
	
ENTER_STOP:
	RTC_Alarm_Set(5);
	HAL_IWDG_Refresh(&hiwdg);

	/* Enter STOP 2 mode */ 
	HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);

	// 判断是否为闹钟唤醒
	if(__HAL_RTC_ALARM_GET_IT(&hrtc,RTC_IT_ALRA)&&stopModeFlag)
	{
		goto ENTER_STOP;
	}

	// 唤醒显示log
	if(get_cur_number.key_prees)
	{
		get_cur_number.key_prees=0;
	}
	
	debug_printf("######&&&&&&&&&!!!!!!!!!!!!!!!!!!!!$$$$$$$$$$$$$$$$$$\r\n");
}
