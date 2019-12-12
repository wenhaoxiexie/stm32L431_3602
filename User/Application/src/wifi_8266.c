#include "wifi_8266.h"
#include "stdio.h"
#include "string.h"
#include "delay.h"
#include "ssd1305.h"
#include "global.h"
#include "win6170.h"
#include "flash.h"
#include "menu.h"

extern KEY_STATE get_cur_number;

extern uint8_t alarmUploadFlag;
extern uint8_t powerOffFlag;

wifi_sys_info_t wifi_sys_info={"FFFFFFFFFFFF",};

void wifi_init(void)
{
	HAL_UART_Receive_IT(&huart3, usart3_rx, 1);
	// 上电
	//wifi_power_on();
//	sys_delay_us(200000);
//	
//	wifi_get_mac(wifi_sys_info.wifi_mac);
//	
//	debug_printf("wifi_mac:%s \r\n",wifi_sys_info.wifi_mac);
//	
//	// 断电
//	wifi_power_off();
}

uint8_t wifi_smart_config(uint8_t* at_command,uint8_t len)
{
	
	 char* pItem=NULL;
	 uint16_t locLen;
	 uint32_t delay_count=60;
	 uint8_t smartOrServer=0;
	 HAL_StatusTypeDef reValue;
	
	
	 All_Screen();
	 wifi_power_on();
	 display_wifi_wait();
	 sys_delay_us(200000);
	 set_win6170(38);
	
		 // 停止stop模式
	 HAL_WIFI_STATUE_CHECK(HAL_OK, wifi_trans_recev_command((uint8_t*)AT_CWSTOPSMART,sizeof(AT_CWSTOPSMART),OK_KEYWORDS,NULL,30));

	 debug_printf("******************set smart_config************************** \r\n");
	 // 设置配网方式
	 HAL_WIFI_STATUE_CHECK(HAL_OK, wifi_trans_recev_command((uint8_t*)AT_CWMODE,sizeof(AT_CWMODE),OK_KEYWORDS,NULL,30));
	
	 debug_printf("******************set time_zone************************** \r\n");
	 // 设置时区
	 HAL_WIFI_STATUE_CHECK(HAL_OK, wifi_trans_recev_command((uint8_t*)AT_CIPSNTPCFG,sizeof(AT_CIPSNTPCFG),OK_KEYWORDS,NULL,30));
	
	 // ssl 设置
	 wifi_trans_recev_command((uint8_t*)AT_CIPSSLSIZE,sizeof(AT_CIPSSLSIZE),OK_KEYWORDS,NULL,30);
	 wifi_trans_recev_command((uint8_t*)AT_CIPSSLCCONF,sizeof(AT_CIPSSLCCONF),OK_KEYWORDS,NULL,30);
	
	 All_Screen();
	 display_sys_config_wlan_countdown(delay_count);
	
//	 sys_delay_us(200000);
	 
	 // smartConfig模式
	 //HAL_WIFI_STATUE_CHECK(HAL_OK, wifi_trans_recev_command(at_command,sizeof(at_command),OK_KEYWORDS,NULL,30));
	 
	 wifi_rxBuff_clear();
	 HAL_WIFI_STATUE_CHECK(HAL_OK,HAL_UART_Transmit(&huart3, at_command, len,100));
	 locLen = usart3_rx_len;
	 do{
		  debug_printf("usart3_rx_buf[%d]:%s \r\n",usart3_rx_len,usart3_rx_buf);
			if(locLen!=usart3_rx_len&&(smartOrServer==0))
			{
				locLen = usart3_rx_len;
				pItem = strstr((char*)usart3_rx_buf,"smartconfig connected wifi");
				if(pItem!=NULL)
				{
					smartOrServer =1;
					debug_printf("########## smart config sucessful\r\n");
					//return HAL_OK;
				}
			}
			if(1==smartOrServer)
			{
					if(delay_count%3==0)
					{
						  reValue = wifi_trans_recev_command((uint8_t*)AT_TCP_SERVER,sizeof(AT_TCP_SERVER),SERVER_KEYWORDS,NULL,10);
							if(reValue==HAL_OK)
								return HAL_OK;
					}else{
							sys_delay_us(1000000);
					}
			}
			if(smartOrServer==0)
				sys_delay_us(1000000);
			--delay_count;
			if(delay_count>0)
				display_sys_config_wlan_countdown(delay_count);
			if(get_cur_number.key_prees==1)
			{
				get_cur_number.key_prees=0;
				if(get_cur_number.number==10)
				{
					delay_count=0;
				}	
			}
	 }while(delay_count);
	
	 HAL_WIFI_STATUE_CHECK(HAL_OK, wifi_trans_recev_command((uint8_t*)AT_CWSTOPSMART,sizeof(AT_CWSTOPSMART),OK_KEYWORDS,NULL,30));
	return HAL_ERROR;
}

void wifi_upload_user_info(uint8_t *userList,uint8_t *mac)
{
	uint8_t serachIndex;
	char *keyboardUserList=NULL;
	char *fpUserList=NULL;
	char *cardUserList=NULL;
	
	char buff_s[12];
	
	stm_flash_read(ADDR_FLASH_PAGE_56,(uint8_t*)key_board_flash,KEYBOARD_SIZE);
	stm_flash_read(ADDR_FLASH_PAGE_58,(uint8_t*)finger_flash,FINGER_SIZE);
	stm_flash_read(ADDR_FLASH_PAGE_60,(uint8_t*)nfc_flash,CARD_SIZE);
	
	keyboardUserList=(char*)calloc(KEYBOARD_NUM*10,sizeof(char));
	cardUserList=(char*)calloc(CARD_NUM*10,sizeof(char));
	fpUserList=(char*)calloc(FINGER_NUM*10,sizeof(char));
	
	for(serachIndex=0;serachIndex<KEYBOARD_NUM;serachIndex++)
	{
		if(key_board_flash[serachIndex].del_flag==0)
		{
			sprintf(buff_s,"%d%%%d%%%d%%",key_board_flash[serachIndex].index,key_board_flash[serachIndex].user_type,
																																						 key_board_flash[serachIndex].user_permision);
			strcat(keyboardUserList,buff_s);
		}
	}
	
	debug_printf(" keyboardUserList: %s \r\n",keyboardUserList);
	for(serachIndex=0;serachIndex<FINGER_NUM;serachIndex++)
	{
		if(finger_flash[serachIndex].del_flag==0)
		{
			sprintf(buff_s,"%d%%%d%%%d%%",finger_flash[serachIndex].index,finger_flash[serachIndex].user_type,
																																						 finger_flash[serachIndex].user_permision);
			strcat(fpUserList,buff_s);
		}
	}
	debug_printf(" fpUserList: %s \r\n",fpUserList);
	for(serachIndex=0;serachIndex<CARD_NUM;serachIndex++)
	{
		if(nfc_flash[serachIndex].del_flag==0)
		{
			sprintf(buff_s,"%d%%%d%%%d%%",nfc_flash[serachIndex].index,nfc_flash[serachIndex].user_type,
																																						 nfc_flash[serachIndex].user_permision);
			strcat(cardUserList,buff_s);
		}
	}
	debug_printf(" cardUserList: %s \r\n",cardUserList);
	sprintf((char*)userList,"%%%s%%6000%%%s%s%s#",mac,keyboardUserList,fpUserList,cardUserList);
	
	debug_printf("userList:%s \r\n",userList);
	
	if(keyboardUserList!=NULL)
	{
		free(keyboardUserList);
		keyboardUserList=NULL;
	}
	if(fpUserList!=NULL)
	{
		free(fpUserList);
		fpUserList=NULL;
	}
	if(cardUserList!=NULL)
	{
		free(cardUserList);
		cardUserList=NULL;
	}
}


static void wifi_server_time_syn()
{
	uint8_t upload_info[256]={0};
	uint16_t locLen;
	char send_command[32];
	uint8_t server_t[64]="Tue Apr  9 14:32:01 2019";
	
	sprintf((char*)upload_info,"%%%s%%%d%%xx%%xx%%#",wifi_sys_info.wifi_mac,SERVER_TIME_UPLOAD);

	// 启动透传模式
	locLen = strlen((char*)upload_info);
	sprintf(send_command,"AT+CIPSEND=%d\r\n",locLen);
	wifi_trans_recev_command((uint8_t*)send_command,sizeof(send_command),OK_KEYWORDS,NULL,30);
	
	// 获取服务器时间	
	if(wifi_trans_recev_command(upload_info,strlen((char*)upload_info),TIME_KEYWORDS,server_t,30)==HAL_OK)
	{
		powerOffFlag=1;
		wifi_parase_time((char*)server_t);
	}
	debug_printf(" server_t:%s \r\n",server_t);

	//关闭透传
	wifi_trans_recev_command((uint8_t *)AT_END,sizeof(AT_END),END_KEYWORDS,NULL,30);
}

uint8_t wifi_upload_all_info(void)
{
	uint8_t reValue;
	uint8_t upload_index;
	uint32_t len;
	
	uint8_t reStatus;
	uint8_t reBuff[1];
	
	char userList[8192];
	
	reValue = HAL_GPIO_ReadPin(GPIOC,WIFI_POWER_Pin); 
	if(reValue==0)
	{
		debug_printf(" wifi unpower \r\n");
		sys_delay_us(1200000);
		return LOCK_ERROR;
	}
	
	stm_flash_read(ADDR_FLASH_PAGE_70,(uint8_t*)wifi_flash,WIFI_SIZE);
	
	debug_printf("index:%d \r\n",wifi_flash->index);
	
	if((wifi_flash->index==0)||(wifi_flash->index==255))
	{
		debug_printf(" Nothing Upload \r\n");
		sys_delay_us(1000000);
		wifi_flash->index=0;
		stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);
		return LOCK_ERROR;
	}

	HAL_WIFI_STATUE_CHECK(HAL_OK, wifi_trans_recev_command((uint8_t*)AT_STATUS,sizeof(AT_STATUS),STATUS_KEYWORDS,reBuff,30));
	reStatus = atoi((char*)reBuff);

	if(reStatus==2)
	{
		debug_printf(" esp8266 get ap \r\n");
		// 连接服务器
		HAL_WIFI_STATUE_CHECK(HAL_OK, wifi_trans_recev_command((uint8_t*)AT_CIPSSLSIZE,sizeof(AT_CIPSSLSIZE),OK_KEYWORDS,NULL,30));
		HAL_WIFI_STATUE_CHECK(HAL_OK, wifi_trans_recev_command((uint8_t*)AT_CIPSSLCCONF,sizeof(AT_CIPSSLCCONF),OK_KEYWORDS,NULL,30));
		HAL_WIFI_STATUE_CHECK(HAL_OK, wifi_trans_recev_command((uint8_t*)AT_TCP_SERVER,sizeof(AT_TCP_SERVER),SERVER_KEYWORDS,NULL,70));
	}
	else if(reStatus==3)
	{
		debug_printf(" esp8266 built ssl  \r\n");
	}else{
		debug_printf(" esp8266 unconnnect \r\n");
		sys_delay_us(1000000);
		return LOCK_ERROR;
	}
	
	if(strncmp((char*)wifi_sys_info.wifi_mac,"FFFFFFFFFFFF",12)==NULL)
	{
				wifi_get_mac(wifi_sys_info.wifi_mac);
	}
	debug_printf(" mac: %s \r\n",wifi_sys_info.wifi_mac);
	
	// 用户列表上报
	for(upload_index=0;upload_index<WIFI_UPLOAD_NUM;upload_index++)
	{
		if(wifi_flash[upload_index].exist)
		{
			if(strstr(wifi_flash[upload_index].upload_info,"3001")!=NULL||strstr(wifi_flash[upload_index].upload_info,"3002")!=NULL)
			{
				wifi_upload_user_info((uint8_t*)userList,wifi_sys_info.wifi_mac); 
				wifi_upload_send((uint8_t*)userList,strlen((char*)userList));
				memset(userList,0,sizeof(userList));
				break;
			}
		}
	}
	// 操作记录上报
	strcpy((char*)userList,"%");
	strcat((char*)userList,(char*)wifi_sys_info.wifi_mac);
	strcat((char*)userList,"%");
	strcat((char*)userList,"7000");
	strcat((char*)userList,"%");
	for(upload_index=0;upload_index<wifi_flash->index;upload_index++)
	{
		if(wifi_flash[upload_index].exist)
		{
			wifi_flash[upload_index].exist=0;
			strcat((char*)userList,wifi_flash[upload_index].upload_info);
			memset(wifi_flash[upload_index].upload_info,0,sizeof(wifi_flash[upload_index].upload_info));
		}
	}
	wifi_flash->index=0;
	stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);
	
	strcat((char*)userList,"#");
	
	// 测试数据
	//strcpy((char*)userList,"%84:f3:eb:79:c1:a7%7000%4000%51%2019-03-15 09:03:02%1000%1%3%1%1%2019-03-15 09:03:02%4000%51%2019-03-15 09:03:45%1000%1%3%1%1%2019-03-15 09:03:45%4000%50%2019-03-15 09:00:21%1000%1%3%1%1%2019-03-15 09:00:21%4000%50%2019-03-15 09:00:49%1000%1%3%1%1%2019-03-15 09:00:49%4000%50%2019-03-15 09:00:29%1000%1%3%1%1%2019-03-15 09:00:29%4000%49%2019-03-15 09:00:54%1000%1%3%1%1%2019-03-15 09:00:54%4000%49%2019-03-15 09:01:32%1000%1%3%1%1%2019-03-15 09:01:32%4000%50%2019-03-15 09:03:26%1000%1%3%1%1%2019-03-15 09:03:26%4000%50%2019-03-15 09:03:45%1000%1%3%1%1%2019-03-15 09:03:45%4000%50%2019-03-15 09:04:24%1000%1%3%1%1%2019-03-15 09:04:24%4000%50%2019-03-15 09:05:33%1000%1%3%1%1%2019-03-15 09:05:33%4000%50%2019-03-15 09:06:01%1000%1%3%1%1%2019-03-15 09:06:01%4000%50%2019-03-15 09:06:41%1000%1%3%1%1%2019-03-15 09:06:41%4000%50%2019-03-15 09:09:46%1000%1%3%1%1%2019-03-15 09:09:46%4000%50%2019-03-15 09:10:47%1000%1%3%1%1%2019-03-15 09:10:47%4000%50%2019-03-15 09:11:46%1000%1%3%1%1%2019-03-15 09:11:46%4000%50%2019-03-15 09:00:18%1000%1%3%1%1%2019-03-15 09:00:18%4000%50%2019-03-15 09:01:06%1000%1%3%1%1%2019-03-15 09:01:06%4000%49%2019-03-15 09:01:21%1000%1%3%1%1%2019-03-15 09:01:21%4000%50%2019-03-15 09:01:37%1000%1%3%1%1%2019-03-15 09:01:37%4000%50%2019-03-15 09:00:14%1000%1%3%1%1%2019-03-15 09:00:14%4000%50%2019-03-15 09:00:31%1000%1%3%1%1%2019-03-15 09:00:31%4000%50%2019-03-15 09:00:46%1000%1%3%1%1%2019-03-15 09:00:46%4000%50%2019-03-15 09:00:13%1000%1%3%1%1%2019-03-15 09:00:13%4000%50%2019-03-15 09:00:13%1000%1%3%1%1%2019-03-15 09:00:13%4000%50%2019-03-15 09:00:30%1000%1%3%1%1%2019-03-15 09:00:30%4000%50%2019-03-15 09:05:23%1000%1%3%1%1%2019-03-15 09:05:23%4000%50%2019-03-15 09:00:41%1000%1%3%1%1%2019-03-15 09:00:41%4000%50%2019-03-15 09:00:58%1000%1%3%1%1%2019-03-15 09:00:58%4000%50%2019-03-15 09:01:12%1000%1%3%1%1%2019-03-15 09:01:12%4000%51%2019-03-15 09:02:30%1000%1%3%1%1%2019-03-15 09:02:30%4000%50%2019-03-15 09:02:47%1000%1%3%1%1%2019-03-15 09:02:47%4000%50%2019-03-15 09:03:16%1000%1%3%1%1%2019-03-15 09:03:16%4000%50%2019-03-15 09:03:30%1000%1%3%1%1%2019-03-15 09:03:30%4000%50%2019-03-15 09:03:44%1000%1%3%1%1%2019-03-15 09:03:44%4000%50%2019-03-15 09:04:53%1000%1%3%1%1%2019-03-15 09:04:53%4000%50%2019-03-15 09:05:07%1000%1%3%1%1%2019-03-15 09:05:07%4000%50%2019-03-15 09:05:34%1000%1%2%1%1%2019-03-15 09:05:34%4000%50%2019-03-15 09:05:48%1000%1%3%1%1%2019-03-15 09:05:48%4000%48%2019-03-15 09:06:02%1000%1%3%1%1%2019-03-15 09:06:02%4000%51%2019-03-15 09:06:26%1000%1%3%1%1%2019-03-15 09:06:26%4000%50%2019-03-15 09:06:50%1000%1%3%1%1%2019-03-15 09:06:50%4000%48%2019-03-15 09:07:04%1000%1%3%1%1%2019-03-15 09:07:04%4000%48%2019-03-15 09:07:18%1000%1%3%1%1%2019-03-15 09:07:18%4000%50%2019-03-15 09:07:34%1000%1%3%1%1%2019-03-15 09:07:34%4000%51%2019-03-15 09:07:48%1000%1%3%1%1%2019-03-15 09:07:48%4000%50%2019-03-15 09:08:01%1000%1%3%1%1%2019-03-15 09:08:01%4000%50%2019-03-15 09:08:14%1000%1%3%1%1%2019-03-15 09:08:14%4000%50%2019-03-15 09:08:27%1000%1%3%1%1%2019-03-15 09:08:27%4000%50%2019-03-15 09:08:43%#");
	
	len = strlen(userList);
	
	debug_printf(" userList:%s \r\n",userList);
	
	// 发送数据
	wifi_upload_send((uint8_t*)userList,len);
		
	//服务器时间同步设置
	if(powerOffFlag==0)
	{
		wifi_server_time_syn();
	}
	
	// 断开服务器
	HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*)AT_CIPCLOSE,sizeof(AT_CIPCLOSE),OK_KEYWORDS,NULL,30));
	
	return HAL_OK;
}

uint8_t wifi_upload_tryErr_info(void)
{
	
	uint8_t index;
	char lock_time[32];
	
	stm_flash_read(ADDR_FLASH_PAGE_70,(uint8_t*)wifi_flash,WIFI_SIZE);
	wifi_power_on();
	
	index =wifi_flash->index;
	wifi_flash[index].exist=1;
		
	HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
	sprintf(lock_time,"20%02d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year,sdatestructure.Month,
																				sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);
//	if(strcmp((char*)wifi_sys_info.wifi_mac,"FFFFFFFFFFFF")!=NULL)
//		wifi_get_mac(wifi_sys_info.wifi_mac);
	sprintf((char*)wifi_flash[index].upload_info,"%d%%%d%%%s%%",WARN_UPLOAD,TRY_ERR_WARN,lock_time);
	
	debug_printf(" \r\n");
	
	if(wifi_flash->index<99)
		wifi_flash->index++;
	stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);

	return  LOCK_OK;
}

uint8_t wifi_upload_alarm_info(void)
{
	char lock_time[32]={0};
	
	uint8_t wifi_index;
	
	HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
	sprintf(lock_time,"20%02d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year,sdatestructure.Month,
																								      sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);
	
//	if(strncmp((char*)wifi_sys_info.wifi_mac,"FFFFFFFFFFFF",12)==NULL)
//		wifi_get_mac(wifi_sys_info.wifi_mac);
//	
//	debug_printf("################ wifi_sys_info.wifi_mac:%s \r\n",wifi_sys_info.wifi_mac);
	

#if 0
	HAL_StatusTypeDef reValue;
	uint8_t upload_info[256]={0};
	
	reValue = wifi_trans_recev_command((uint8_t*)AT_CIPSSLSIZE,sizeof(AT_CIPSSLSIZE),OK_KEYWORDS,NULL,30);
	reValue |= wifi_trans_recev_command((uint8_t*)AT_CIPSSLCCONF,sizeof(AT_CIPSSLCCONF),OK_KEYWORDS,NULL,30);
	reValue |=wifi_trans_recev_command((uint8_t*)AT_TCP_SERVER,sizeof(AT_TCP_SERVER),SERVER_KEYWORDS,NULL,50);
	
	if(reValue==HAL_ERROR)
	{
		stm_flash_read(ADDR_FLASH_PAGE_70,(uint8_t*)wifi_flash,WIFI_SIZE);
		wifi_index = wifi_flash->index;
		wifi_flash[wifi_index].exist=1;
	
		sprintf((char*)wifi_flash->upload_info,"%d%%%d%%%s%%",WARN_UPLOAD,ANTI_PRY_WARN,lock_time);
		
		if(wifi_flash->index<99)
			wifi_flash->index++;
	
		// 保存上报信息到flash
		stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);
		
		return LOCK_ERROR;
	}
	
	sprintf((char*)upload_info,"%%%s%%%d%%%d%%%s%%#",wifi_sys_info.wifi_mac,WARN_UPLOAD,ANTI_PRY_WARN,lock_time);
	
	// 发送数据
	wifi_upload_send(upload_info,strlen((char*)upload_info));
	
	// 断开服务器
	//HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*)AT_CIPCLOSE,sizeof(AT_CIPCLOSE),OK_KEYWORDS,NULL,30));
	
#else
		stm_flash_read(ADDR_FLASH_PAGE_70,(uint8_t*)wifi_flash,WIFI_SIZE);
		wifi_index = wifi_flash->index;
		wifi_flash[wifi_index].exist=1;
	
		sprintf((char*)wifi_flash[wifi_index].upload_info,"%d%%%d%%%s%%",WARN_UPLOAD,ANTI_PRY_WARN,lock_time);
		
		if(wifi_flash->index<99)
			wifi_flash->index++;
	
		// 保存上报信息到flash
		stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);

#endif
	
	return LOCK_OK;
	
}

uint8_t wifi_upload_send(uint8_t* str,uint32_t length_s)
{
	
	uint8_t pageNum,pageIndex;
	 
	uint32_t pageCont; 
	
	char send_command[16]={0};
	
	debug_printf("length_s：%d \r\n",length_s);
	
	if(length_s<=2048)
	{
	  sprintf(send_command,"AT+CIPSEND=%d\r\n",length_s);
		
		// 启动透传模式
		HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) send_command,strlen(send_command),OK_KEYWORDS,NULL,30));
		
		// 上报数据
		HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) str,length_s,SEND_KEYWORDS,NULL,30));
		
		// 退出透传模式
		HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) AT_END,sizeof(AT_END),END_KEYWORDS,NULL,30));
				
		sys_delay_us(1400000);
		
	}else{
		pageNum = length_s/2048;
		pageCont = length_s%2048;
		
		debug_printf("pageNum:%d  pageCont:%d \r\n",pageNum,pageCont);
		for(pageIndex=0;pageIndex<pageNum;pageIndex++)
		{
			sprintf(send_command,"AT+CIPSEND=%d\r\n",2048);
			
			// 启动透传模式
			HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) send_command,strlen(send_command),OK_KEYWORDS,NULL,30));
		
			memset(send_command,0,sizeof(send_command));
			
			sys_delay_us(10000);
			
			str+=pageIndex*2048;
			
			//上报数据
			HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) str,2048,SEND_KEYWORDS,NULL,30));
			
			// 退出透传模式
			HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) AT_END,sizeof(AT_END),END_KEYWORDS,NULL,30));
			
			
		}
		if(pageCont>0)
		{
			sprintf(send_command,"AT+CIPSEND=%d\r\n",pageCont);
			
			// 启动透传模式
			HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) send_command,strlen(send_command),OK_KEYWORDS,NULL,30));
	
			str+=pageIndex*2048;
			
			//上报数据
			HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) str,pageCont,SEND_KEYWORDS,NULL,30));
	
			// 退出透传模式
			HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) AT_END,sizeof(AT_END),END_KEYWORDS,NULL,30));
		}
	}
	return LOCK_OK;
}

int8_t wifi_upload_battery(uint32_t value)
{
	int8_t batteryClass;
	
	if(value>6000)
			value=6000;
	
	batteryClass = 100-(6000 - value)/15;
	
	if(batteryClass>=100)
	{
		batteryClass=100;
	}
	else if(batteryClass<=0)
	{
		batteryClass=0;
	}
	return batteryClass;
}


void wifi_power_on(void)
{
		HAL_GPIO_WritePin(WIFI_POWER_GPIO_Port, WIFI_POWER_Pin, GPIO_PIN_SET);
		HAL_UART_Receive_IT(&huart3, usart3_rx, 1);
}

void wifi_power_off(void)
{
	HAL_GPIO_WritePin(WIFI_POWER_GPIO_Port, WIFI_POWER_Pin, GPIO_PIN_RESET);
}

uint8_t wifi_get_mac(uint8_t * mac)
{
	uint8_t locMac[64];
	
	HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*)AT_MAC,sizeof(AT_MAC),MAC_KEYWORDS,locMac,30));
	
	double_point_parse(locMac,mac);
	
	return HAL_OK;
}

uint8_t wifi_get_password(uint8_t* reBuff,uint8_t* password)
{
	char* pIndex=NULL;
	
	if(reBuff==NULL||password==NULL)
		return HAL_ERROR;
	
	pIndex = strstr((char*)reBuff,":");
	if(pIndex==NULL)
		return NULL;
	pIndex++;
	
	while(*pIndex!='\0')
	{
		*password++ = *pIndex++;
	}
	return HAL_OK;
}

HAL_StatusTypeDef wifi_remote_open(uint8_t *password,uint8_t* len,uint8_t* remote_flag,uint8_t *permision,uint8_t* index)
{
	uint8_t i,j;
	static uint8_t reBuff[64];
	uint8_t password_get[32]={0};
	char remoteRequestInfo[128]={0};
	char lock_time[32]={0};
	uint8_t number_cur=0;
	char iphoneNum[11]={0};
  char ch[1];
	uint16_t delay_count;
	uint8_t iphoneFlag=0;
	char send_command[16]={0};
	uint16_t length_s;
	HAL_StatusTypeDef reValue;
	
	if(password[0]!=0||password[1]!=0||password[2]!=0||password[3]!=0)
		return HAL_ERROR;
	
	wifi_power_on();
	
START:
	display_iphone_appoint();
	
	delay_count=60000;
	do{
		if(get_cur_number.key_prees)
		{
			get_cur_number.key_prees=0;
			if(get_cur_number.number==1)
			{
				iphoneFlag=1;
				break;
			}
			else if(get_cur_number.number==2)
			{
				iphoneFlag=0;
				break;
			}
			else if(get_cur_number.number==10)
			{
				return HAL_ERROR;
			}
		}
		sys_delay_us(100);
	}while(--delay_count);
	if(delay_count==0)
		return HAL_ERROR;
	
LOOP:
	if(iphoneFlag)
	{
			delay_count=8000;
			number_cur=0;
			display_iphone_num();
			do{
				if(get_cur_number.key_prees)
				{
					delay_count=8000;
					get_cur_number.key_prees=0;
					if(get_cur_number.number==11)
					{
						if(number_cur!=11)
						{
							display_iphone_result();
							sys_delay_us(2000000);
							goto LOOP;
						}
							break;
					}
					else if(get_cur_number.number==10)
					{
						if(number_cur<=0)
							goto START;		
							//continue;
						// 回删
						display_spe_char_del(--number_cur);
					}else{
						password_get[number_cur++] = get_cur_number.number;
						display_iphone_number(get_cur_number.number,number_cur);
					}
				}
				sys_delay_us(1000);
			}while(--delay_count);
			if(delay_count==0)
				return HAL_ERROR;
			
			debug_printf("number_cur:%d \r\n",number_cur);
			for(i=0;i<number_cur;i++)
			{
				sprintf(ch,"%d",password_get[i]);
				strcat(iphoneNum,ch);
			}
			debug_printf("iphoneNum:%s \r\n",iphoneNum);
	}

	set_win6170(54);
	display_wifi_wait();
	sys_delay_us(5000000);
	
	stm_flash_read(ADDR_FLASH_PAGE_56,(uint8_t*)key_board_flash,KEYBOARD_SIZE);
	
	HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
	sprintf(lock_time,"20%02d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year,sdatestructure.Month,
																								sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);
	if(strncmp((char*)wifi_sys_info.wifi_mac,"FFFFFFFFFFFF",12)==NULL)
			wifi_get_mac(wifi_sys_info.wifi_mac);	

	debug_printf(" mac: %s\r\n",wifi_sys_info.wifi_mac);
	if(iphoneFlag)
		sprintf(remoteRequestInfo,"%%%s%%0000%%%s%%#",wifi_sys_info.wifi_mac,iphoneNum);
	else
		sprintf(remoteRequestInfo,"%%%s%%0000%%#",wifi_sys_info.wifi_mac);
	
	memset(iphoneNum,0,sizeof(iphoneNum));
	
	reValue = wifi_trans_recev_command((uint8_t*)AT_CIPSSLSIZE,sizeof(AT_CIPSSLSIZE),OK_KEYWORDS,NULL,30);
	
	reValue |= wifi_trans_recev_command((uint8_t*)AT_CIPSSLCCONF,sizeof(AT_CIPSSLCCONF),OK_KEYWORDS,NULL,30);
	
	if(reValue!=HAL_OK)
	{
		display_operate_fail();
		debug_printf("communicate failed\r\n");
		set_win6170(43);
		sys_delay_us(2000000);
		return HAL_ERROR;
	}
	
	// 链接服务器
	reValue = wifi_trans_recev_command((uint8_t*)AT_TCP_SERVER,sizeof(AT_TCP_SERVER),SERVER_KEYWORDS,NULL,50);
	
	if(reValue!=HAL_OK)
	{
		debug_printf("server connect failed\r\n");
		set_win6170(50);
		sys_delay_us(2000000);
		return HAL_ERROR;
	}

	debug_printf("remoteRequestInfo:%s \r\n",remoteRequestInfo);
	
	// 发送数据
	length_s = strlen(remoteRequestInfo);
	
	sprintf(send_command,"AT+CIPSEND=%d\r\n",length_s);
	
	//wifi_upload_send((uint8_t*)remoteRequestInfo,strlen(remoteRequestInfo));
	
	// 启动透传模式
	HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) send_command,strlen(send_command),OK_KEYWORDS,NULL,30));
		
	// 上报数据
	HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) remoteRequestInfo,length_s,SEND_KEYWORDS,NULL,30));
	
	delay_count=60;
	All_Screen();
	display_sys_unlock_wlan_countdown(delay_count);
	
	do{
		if(get_cur_number.key_prees)
		{
			get_cur_number.key_prees=0;
			if(get_cur_number.number==10)
			{
					// 断开服务器
					HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*)AT_CIPCLOSE,sizeof(AT_CIPCLOSE),OK_KEYWORDS,NULL,30));
					// 退出透传模式
					HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) AT_END,sizeof(AT_END),END_KEYWORDS,NULL,30));
					return HAL_ERROR;
			}
		}
		reValue = usart3_search_item(PASSWORD_KEYWORDS,reBuff,10);
		if(reValue==HAL_OK)
		{
			memset(password_get,0,sizeof(password_get));
			wifi_get_password(reBuff,password_get);
			debug_printf("%s \r\n",password_get);
			break;
		}
		display_sys_unlock_wlan_countdown(delay_count);
	}while(delay_count--);

	// 退出透传模式
	HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*) AT_END,sizeof(AT_END),END_KEYWORDS,NULL,30));
	
	if(reValue!=HAL_OK)
	{
		sprintf(remoteRequestInfo,"%%%s%%%d%%%d%%%d%%%d%%%d%%%s%%#",wifi_sys_info.wifi_mac,OPEN_UPLOAD,0,APP_TYPE,0,0,lock_time);
		debug_printf(" remoteRequestInfo:%s\r\n",remoteRequestInfo);
		wifi_upload_send((uint8_t *)remoteRequestInfo,strlen(remoteRequestInfo));
		return HAL_ERROR;
	}

	memset(password,0,*len);
	
	*len = strlen((char*)password_get);
	
	debug_printf("password_get:%s  len : %d\r\n",password_get,*len);
	
	// 判断接收失败的字符
	if(strstr((const char*)password_get,"No mobile exists")!=NULL)
	{
		display_iphone_nonexistent();
		// 断开服务器
		HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*)AT_CIPCLOSE,sizeof(AT_CIPCLOSE),OK_KEYWORDS,NULL,30));
		sys_delay_us(2000000);
	
		debug_printf("  ###########################No mobile exists######################### \r\n");
		
		return HAL_ERROR;
	}
	else if(strstr((const char*)password_get,"App offline")!=NULL)
	{
		display_iphone_offline();
		// 断开服务器
		HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*)AT_CIPCLOSE,sizeof(AT_CIPCLOSE),OK_KEYWORDS,NULL,30));
		
		sys_delay_us(2000000);
		
		debug_printf("  ###########################App offline######################### \r\n");
	
		return HAL_ERROR;
	}
	
	for(uint8_t i=0;i<*len;i++)
	{
		password[i]=password_get[i]-'0';
		printf(" %d ",password[i]);
	}
	debug_printf("\r\n");
	
	// 验证密码是否已存在
	for(i=0;i<KEYBOARD_NUM;i++)
	{
		if(key_board_flash[i].del_flag==0)
		{
			if(key_board_flash[i].data_len!=*len)
			continue;
			for(j=0;j<*len;j++)
			{
				if(password[j]!=key_board_flash[i].data[j])
					break;
			}
			if(j==*len)
			{
				break;
			}
		}
	}
	if(i==KEYBOARD_NUM)
	{
		sprintf(remoteRequestInfo,"%%%s%%%d%%%d%%%d%%%d%%%d%%%s%%#",wifi_sys_info.wifi_mac,OPEN_UPLOAD,0,APP_TYPE,0,0,lock_time);
		debug_printf(" remoteRequestInfo:%s\r\n",remoteRequestInfo);
		wifi_upload_send((uint8_t *)remoteRequestInfo,strlen(remoteRequestInfo));
		HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*)AT_CIPCLOSE,sizeof(AT_CIPCLOSE),OK_KEYWORDS,NULL,30));
		return HAL_ERROR;
	}else{
		sprintf(remoteRequestInfo,"%%%s%%%d%%%d%%%d%%%d%%%d%%%s%%#",wifi_sys_info.wifi_mac,OPEN_UPLOAD,
																				i,APP_TYPE,key_board_flash[i].user_permision,OPEN_DOOR,lock_time);
		debug_printf(" remoteRequestInfo:%s\r\n",remoteRequestInfo);
		wifi_upload_send((uint8_t *)remoteRequestInfo,strlen(remoteRequestInfo));
	}
	//*permision=key_board_flash[i].user_permision;
	*permision=USUALLY; // 远程开门只做普通用户
	*index =i;
	*remote_flag=1;
	
	// 断开服务器
	HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*)AT_CIPCLOSE,sizeof(AT_CIPCLOSE),OK_KEYWORDS,NULL,30));
		
	return HAL_OK;
}

// Tue Apr  9 14:32:01 2019
// Wed Jan 23 15:40:26 2019
uint8_t wifi_parase_time(char* time)
{
	char weekend[][3]={"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
	char month[][3]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	char* p_time=NULL;
	uint8_t index;
	
	if(time==NULL)
		return LOCK_ERROR;
	
	p_time = time;
	debug_printf("p_time :%s \r\n",p_time);
	
	// 查找星期
	for(index=0;index<7;index++)
	{
		if(strncmp(p_time,weekend[index],3)==NULL)
		{
			sdatestructure.WeekDay= index+1;
			debug_printf("WeekDay:%d \r\n",sdatestructure.WeekDay);
		}
	}
	p_time+=4;
	debug_printf("%c \r\n",*p_time);
	// 查找月份
	for(index=0;index<12;index++)
	{
		if(strncmp(p_time,month[index],3)==NULL)
		{
			sdatestructure.Month= index+1;
			debug_printf("Month:%d \r\n",sdatestructure.Month);
		}
	}
	p_time+=4;
	debug_printf("%c \r\n",*p_time);
	// 解析日期
	if(*p_time==' ')
		sdatestructure.Date=0;
	else
		sdatestructure.Date =*p_time-'0';
	p_time++;
	sdatestructure.Date=sdatestructure.Date*10+*p_time-'0';
	debug_printf(" Date:%02d\r\n",sdatestructure.Date);
	p_time+=2;
	stimestructure.Hours=*p_time-'0';
	p_time++;
	stimestructure.Hours=stimestructure.Hours*10+*p_time-'0';
	debug_printf("Hours:%d\r\n",stimestructure.Hours);
	p_time+=2;
	stimestructure.Minutes=*p_time-'0';
	p_time++;
	stimestructure.Minutes=stimestructure.Minutes*10+*p_time-'0';
	debug_printf("Minutes:%d \r\n",stimestructure.Minutes);
	p_time+=2;
	stimestructure.Seconds=*p_time-'0';
	p_time++;
	stimestructure.Seconds=stimestructure.Seconds*10+*p_time-'0';
	debug_printf("Seconds:%d \r\n",stimestructure.Seconds);
	//解析年
	p_time+=4;
	debug_printf("%c \r\n",*p_time);
	sdatestructure.Year=*p_time-'0';
	p_time++;
	sdatestructure.Year=sdatestructure.Year*10+*p_time-'0';
	debug_printf("Year:%d \r\n",sdatestructure.Year);
	
	/* Set the RTC current Time ,must get time first*/
	HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
	/* Set the RTC current Date */
	HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
	
	return HAL_OK;
}

// 去除双引号
void double_point_parse(uint8_t* str,uint8_t* des)
{
	uint8_t i;
	uint8_t *pTem =des;

	for(i=1;i<strlen((char*)str)-1;i++)
	{
			*pTem = str[i];
			pTem++;
	}
}

HAL_StatusTypeDef usart3_search_item(const char* itemStr,uint8_t * reBuff,uint16_t timeout)
{
	uint16_t locTime;
	uint16_t locLen;
	
	char* pItem=NULL;
	uint8_t* locBuff =NULL;
	uint8_t locCoun=0;
	
	locBuff = reBuff;
	locLen = strlen(itemStr);
	locTime = timeout;
	do{
		debug_printf("%s\r\n",usart3_rx_buf);
		pItem = strstr((char*)usart3_rx_buf,itemStr);
		if(pItem!=NULL)
		{
			if(reBuff==NULL)
				return HAL_OK;
			pItem+=locLen;
			while((pItem[0]!=0x0d) && (pItem[1]!=0x0a)&&(locCoun<32))
			{
				*locBuff++ = *pItem++;
				locCoun++;
			}
			*locBuff = '\0';
			debug_printf("%s \r\n",reBuff);
			return HAL_OK;
		}
		sys_delay_us(100000);
	}while(locTime--);
	
	return HAL_TIMEOUT;
}

HAL_StatusTypeDef usart3_recv_parse(const char* itemStr,uint8_t* reBuff,uint16_t timeout)
{
	uint16_t reLen;
	uint8_t itemLen;

	
	char* pItem=NULL;
	uint8_t* locBuff=NULL;
	
	locBuff = reBuff;
	itemLen = strlen(itemStr);
	
	reLen = usart3_rx_len;
	do{
		//debug_printf("usart3_rx_buf[%d]:%s \r\n",usart3_rx_len,usart3_rx_buf);
		if(reLen!=usart3_rx_len)
		{
			reLen = usart3_rx_len;
			pItem = strstr((char*)usart3_rx_buf,itemStr);
			if(pItem != NULL)
			{
				debug_printf("%s \r\n",pItem);
				if(locBuff==NULL)
					return HAL_OK;
				pItem+=itemLen;
				while((pItem[0]!=0x0d) && (pItem[1]!=0x0a))
				{
					*locBuff++ = *pItem++;
				}
				*locBuff = '\0';
				debug_printf("%s \r\n",reBuff);
				
				return HAL_OK;
			}
		}
		sys_delay_us(100000);
	}while(timeout--);
	
	return HAL_TIMEOUT;
}

void wifi_rxBuff_clear(void)
{
	uint16_t i;
	
	for(i=0;i<USART3_REC_LEN;i++)
		usart3_rx_buf[i]=0;
	
	usart3_rx_len =0;
}

HAL_StatusTypeDef wifi_trans_recev_command(uint8_t* command,uint16_t cLen,const char* itemStr,uint8_t* reBuff,uint16_t timeout)
{
	
	HAL_StatusTypeDef reValue;
	
	wifi_rxBuff_clear();
	
	reValue = HAL_UART_Transmit(&huart3,command,cLen,1000);
	if(reValue!=HAL_OK)
		debug_printf(" ********************HAL_UART_Transmit failed************* \r\n");
	
	HAL_WIFI_STATUE_CHECK(HAL_OK,usart3_recv_parse(itemStr,reBuff,timeout));
	
	return HAL_OK;
}

