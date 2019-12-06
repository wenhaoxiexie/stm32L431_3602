#include "nfc.h"
#include "global.h"
#include "flash.h"
#include "user_operator.h"
#include "usart.h"
#include "wifi_8266.h"
#include "win6170.h"
#include "apt8l16.h"

nfc_data nfc_temp;

extern KEY_STATE get_cur_number;

uint8_t nfc_add(uint8_t permision)
{
	uint8_t i,j;
	uint32_t delay_count;
	uint8_t Revalue;
	char lock_time[32];
	
	send_bytes(KEY10_ON);
	APT_KeyEnableSel(KDR0_CH_10_EN,KDR1_CH_10_EN);
	
	delay_count=800;
	// 获取当前最新数据
	stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
	if(get_cur_info->cur_card_index>=CARD_NUM)
		return LOCK_MAX;
	
	set_win6170(0XF3);
	sys_delay_us(2000);
	set_win6170(25);
	display_card_rigister();
	debug_printf(" wait for card ... \r\n");
	//HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	nfc_temp.irq_flag=0;
	do{
		if(get_cur_number.key_prees)
		{
			get_cur_number.key_prees=0;
			if(get_cur_number.number==10)
			{
				//HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
				return LOCK_OK;
			}
		}
		if(nfc_temp.irq_flag==1)
		{
			debug_printf("nfc_temp.irq_flag:%d \r\n",nfc_temp.irq_flag);
			nfc_temp.irq_flag=0;
			//HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

			//验证是否有数据
			if(nfc_temp.len<1)
			{
				return LOCK_ERROR;
			}
				
			// 验证该卡是否已注册
			stm_flash_read(ADDR_FLASH_PAGE_60,(uint8_t*)nfc_flash,CARD_SIZE);
			for(i=0;i<CARD_NUM;i++)
			{
				if(nfc_flash[i].del_flag==0)
				{
					if(nfc_flash[i].uid_len==nfc_temp.len)
					{
						for(j=0;j<nfc_temp.len;j++)
						{	
							debug_printf("NFC COMPARE: %02x:%02x \r\n",nfc_temp.guid[j],nfc_flash[i].uid[j]);
							if(nfc_flash[i].uid[j]!=nfc_temp.guid[j])
								break;
						}
						if(j==nfc_temp.len)
						{
							return LOCK_EXIT;
						}	
					}
				}
			}
			// 搜索卡片空闲位置
			for(i=0;i<get_cur_info->cur_card_index;i++)
			{
				if(nfc_flash[i].del_flag==1)
					break;
			}
			debug_printf("the %d nfc is coming  \r\n",i);
			// 保存卡片数据到指定位置
			nfc_flash[i].index=i;
			nfc_flash[i].user_permision=permision;
			nfc_flash[i].user_type=CARD_TYPE;
			nfc_flash[i].del_flag=0;
			nfc_flash[i].uid_len=nfc_temp.len;
			for(j=0;j<nfc_temp.len;j++)
				nfc_flash[i].uid[j]=nfc_temp.guid[j];
			stm_flash_erase_write(ADDR_FLASH_PAGE_60,(uint64_t*)nfc_flash,CARD_SIZE);
			
			get_cur_info->cur_card_index++;
			get_cur_info->cur_index_number++;
			
			if(get_cur_info->cur_log_number>=LOG_NUM)
				get_cur_info->cur_log_number=0;
		
			HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
			sys_log_record(i,ADD_USER_OPR,permision,CARD_TYPE,get_cur_info->cur_log_number);
				
			get_cur_info->cur_log_number++;
			
			stm_flash_erase_write(ADDR_FLASH_PAGE_100,(uint64_t*)get_cur_info,CUR_INDEX_SIZE);
			// 新用户上报信息 
			stm_flash_read(ADDR_FLASH_PAGE_70,(uint8_t*)wifi_flash,WIFI_SIZE);
			wifi_power_on();			
			Revalue =wifi_flash->index;
			debug_printf("Revalue: %d \r\n",Revalue);
			

			sprintf(lock_time,"20%02d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year,sdatestructure.Month,sdatestructure.Date,
																																							 stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);
			
			wifi_flash[Revalue].exist=1;
			sprintf(wifi_flash[Revalue].upload_info,"%d%%%d%%%d%%%d%%%d%%%s%%",ADD_USER_UPLOAD,i,CARD_TYPE,permision,
																																					 ADD_USER_OPR,lock_time);
			if(wifi_flash->index<99)
				wifi_flash->index++;
		
			// 保存上报信息到flash
			stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);
			set_win6170(42);
			display_check_sucessful(i,permision,CARD_TYPE);
			debug_printf(" card rigister sucessful \r\n");
			sys_delay_us(1000000);
			return LOCK_OK;
		}
		sys_delay_us(10000);
	}while(--delay_count);	
	//HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
	return LOCK_OUTTIME;
}
uint8_t nfc_delete(uint8_t index,uint8_t permision)
{
	uint8_t i;
	uint8_t Revalue;
	char lock_time[32];
	
	stm_flash_read(ADDR_FLASH_PAGE_60,(uint8_t*)nfc_flash,CARD_SIZE);
	
	stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
	
	// 遍历所有的卡片
	for(i=0;i<CARD_NUM/*get_cur_info->cur_card_index*/;i++)
	{
		if(nfc_flash[i].del_flag==0)
		{
			if(index==nfc_flash[i].index)
			{
				debug_printf(" del index:%d\r\n",index);
				memset(nfc_flash[i].uid,0xff,sizeof(nfc_flash[i].uid));
				nfc_flash[i].del_flag=1;
				stm_flash_erase_write(ADDR_FLASH_PAGE_60,(uint64_t*)nfc_flash,CARD_SIZE);
				
				get_cur_info->cur_index_number--;
				get_cur_info->cur_card_index--;
				
				if(get_cur_info->cur_log_number>=LOG_NUM)
					get_cur_info->cur_log_number=0;
			
				HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
				HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
				sys_log_record(index,DEL_USER_OPR,permision,CARD_TYPE,get_cur_info->cur_log_number);
					
				get_cur_info->cur_log_number++;
				
				stm_flash_erase_write(ADDR_FLASH_PAGE_100,(uint64_t*)get_cur_info,CUR_INDEX_SIZE);
				// 删除上报信息 
				stm_flash_read(ADDR_FLASH_PAGE_70,(uint8_t*)wifi_flash,WIFI_SIZE);
				wifi_power_on();			
				Revalue = wifi_flash->index;
				debug_printf("Revalue: %d \r\n",Revalue);
			
				sprintf(lock_time,"20%02d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year,sdatestructure.Month,sdatestructure.Date,
																																			           stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);
				wifi_flash[Revalue].exist=1;
				sprintf(wifi_flash[Revalue].upload_info,"%d%%%d%%%d%%%d%%%d%%%s%%",DELETE_USER_UPLOAD,index,
																																						 CARD_TYPE,permision,DEL_USER_OPR,lock_time);
				if(wifi_flash->index<99)
					wifi_flash->index++;
				// 保存上报信息到flash
				stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);
				set_win6170(42);
				display_del_sucessful();
				sys_delay_us(1000000);
				return LOCK_OK;
			}
		}
	}
	return LOCK_ERROR;
}

