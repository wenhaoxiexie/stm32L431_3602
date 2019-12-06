#include "keyboard.h"
#include "flash.h"
#include "user_operator.h"
#include "usart.h"
#include "wifi_8266.h"
#include "hsj08.h"
#include "global.h"
#include "win6170.h"
#include "menu.h"

extern uint32_t battery_value;
extern int8_t battery_class;
extern uint8_t prying_flag;

uint8_t keyboard_password_save(uint8_t *password,uint8_t len,uint8_t permision)
{
	uint8_t i,j;
	uint8_t Revalue;
	char lock_time[32];
	
	stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
	
	// 验证是否超过最大数
	if(get_cur_info->cur_keyboard_index>=KEYBOARD_NUM)
	{
		return LOCK_MAX;
	}
	
	stm_flash_read(ADDR_FLASH_PAGE_56,(uint8_t*)key_board_flash,KEYBOARD_SIZE);
	
	// 保存数据到空位置flash中
	
	for(i=0;i<get_cur_info->cur_keyboard_index;i++)
	{
		if(key_board_flash[i].del_flag==1)
			break;
	}
	debug_printf(" the %d user is coming \r\n",i);
	
	// 复制密码并保存
	
	for(j=0;j<len;j++)
	{
		key_board_flash[i].data[j]=password[j];
	}
	key_board_flash[i].index=i;
	key_board_flash[i].user_type=KEYBOARD_TYPE;
	key_board_flash[i].user_permision=permision;
	key_board_flash[i].del_flag=0;
	key_board_flash[i].data_len=len;
	
	set_win6170(42);
	display_check_sucessful(i,permision,KEYBOARD_TYPE);
	sys_delay_us(1000000);
	
	stm_flash_erase_write(ADDR_FLASH_PAGE_56,(uint64_t*)key_board_flash,KEYBOARD_SIZE);
	
	// 更新当前记录值
	
	get_cur_info->cur_keyboard_index++;
	get_cur_info->cur_index_number++;
	
	stm_flash_erase_write(ADDR_FLASH_PAGE_100,(uint64_t*)get_cur_info,CUR_INDEX_SIZE);
	
	if(get_cur_info->cur_log_number>=LOG_NUM)
		get_cur_info->cur_log_number=0;
	
	
	HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
	sys_log_record(i,ADD_USER_OPR,permision,KEYBOARD_TYPE,get_cur_info->cur_log_number);
	
	get_cur_info->cur_log_number++;
	
	stm_flash_erase_write(ADDR_FLASH_PAGE_100,(uint64_t*)get_cur_info,CUR_INDEX_SIZE);
	// 新用户上报信息 
	stm_flash_read(ADDR_FLASH_PAGE_70,(uint8_t*)wifi_flash,WIFI_SIZE);
	wifi_power_on();	
	
	Revalue=wifi_flash->index;
	
	
	sprintf(lock_time,"20%02d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year,sdatestructure.Month,sdatestructure.Date,
																																 stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);
	
	wifi_flash[Revalue].exist=1;
	sprintf(wifi_flash[Revalue].upload_info,"%d%%%d%%%d%%%d%%%d%%%s%%",ADD_USER_UPLOAD,i,KEYBOARD_TYPE,permision,
																																				 ADD_USER_OPR,lock_time);
	if(wifi_flash->index<99)
		wifi_flash->index++;

	// 保存上报信息到flash
	stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);
	
	return LOCK_OK;
}


uint8_t keyboard_password_del(uint8_t index,uint8_t permision)
{
	uint8_t i;
	uint8_t Revalue;
	char lock_time[32];
	
	stm_flash_read(ADDR_FLASH_PAGE_56,(uint8_t*)key_board_flash,KEYBOARD_SIZE);
	
	stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
	
	for(i=0;i<KEYBOARD_NUM;i++)
	{
		if(i==index)
		{
				debug_printf(" del index:%d\r\n",index);
				memset(key_board_flash[i].data,0xff,sizeof(key_board_flash[i].data));
				key_board_flash[i].del_flag=1;
				key_board_flash[i].data_len=0;
				stm_flash_erase_write(ADDR_FLASH_PAGE_56,(uint64_t*)key_board_flash,KEYBOARD_SIZE);
				
			
				get_cur_info->cur_index_number--;
				get_cur_info->cur_keyboard_index--;
				
				if(get_cur_info->cur_log_number>=LOG_NUM)
					get_cur_info->cur_log_number=0;
				
				HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
				HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
				sys_log_record(index,DEL_USER_OPR,permision,KEYBOARD_TYPE,get_cur_info->cur_log_number);
				
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
				sprintf(wifi_flash[Revalue].upload_info,"%d%%%d%%%d%%%d%%%d%%%s%%",DELETE_USER_UPLOAD,i,KEYBOARD_TYPE,permision,DEL_USER_OPR,
																																						 lock_time);
				if(wifi_flash->index<99)
					wifi_flash->index++;
				
				// 保存上报信息到flash
				stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);
				
				return LOCK_OK;
		}
	}
	return LOCK_ERROR;
}

uint8_t keyboard_password_confirm(uint8_t *password,uint8_t len,uint8_t* permision)
{
	int8_t Revalue=-1;
//	int8_t reValue_app=-1;
	uint8_t permision_app;
	uint8_t index_app;
	uint8_t app_upload_flag=0;
	uint8_t key_cur=0,lock_key_cur=0,flash_cur=0;
	uint8_t count=0;
	char lock_time[32];
	
	HAL_StatusTypeDef reValue;
//	char remoteAppUpload[64]={0};
	
	// 参数验证
	if(((len<5&&len!=4)||len>=32))
		return LOCK_ERROR;
	//远程开锁
	if(len==4)
	{
		debug_printf("########### remote open \r\n");
		reValue = wifi_remote_open(password,&len,&app_upload_flag,&permision_app,&index_app);
		debug_printf(" ################## reValue: %d \r\n",reValue);
		if(reValue== HAL_ERROR)
		{
			display_menu();
			return LOCK_EXIT;
		}
	}
		
	// 查询是否注册过
	stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
	
	// 验证出厂密码 888888
	if(get_cur_info->cur_index_number<1)
	{
		if(len!=6)
			return LOCK_ERROR;
		
		for(uint8_t index=0;index<6;index++)
		{
			if(password[index]!=8)
				return LOCK_ERROR;
		}
		*permision=ANDMI;
		display_confirm(KEYBOARD_TYPE,ANDMI,0);
		prying_flag=0;
		set_win6170(get_cur_info->cur_vol_size);
		return LOCK_OK;
	}
	
	//验证本地密码
	if(get_cur_info->cur_keyboard_index<1)
	{
		set_win6170(31);
		return LOCK_ERROR;
	}
	
	stm_flash_read(ADDR_FLASH_PAGE_56,(uint8_t*)key_board_flash,KEYBOARD_SIZE);
	
	debug_printf("get_cur_info->cur_keyboard_index:%d \r\n",get_cur_info->cur_keyboard_index);
	
	// 校验本地密码
	for(flash_cur=0;flash_cur<KEYBOARD_NUM;flash_cur++)
	{
		if(key_board_flash[flash_cur].del_flag==0)
		{
			//debug_printf("key_board_flash[flash_cur].index:%d\r\n",key_board_flash[flash_cur].index);
			for(key_cur=0;key_cur<len;key_cur++)
			{
				count=0;
				for(lock_key_cur=0;lock_key_cur<key_board_flash[flash_cur].data_len;lock_key_cur++)
				{
					if(password[key_cur]==key_board_flash[flash_cur].data[lock_key_cur])
					{
						//printf(" [%d : %d],",key_cur,lock_key_cur);
						key_cur++;
						count++;
					}
				}
				//debug_printf("len: %d : %d : %d\r\n",key_board_flash[flash_cur].data_len,lock_key_cur,key_cur);
				if(count==key_board_flash[flash_cur].data_len)
					break;
			}
			//debug_printf("count:%d \r\n",count);
			if(count!=0&&count==key_board_flash[flash_cur].data_len)
				break;
		}
	}
	//debug_printf("key_board_flash[flash_cur].index:%d %d :%d \r\n",flash_cur,key_board_flash[flash_cur].index,key_board_flash[flash_cur].data_len);
	//debug_printf("count:%d \r\n",count);
	if(count!=key_board_flash[flash_cur].data_len||count==0)
	{
			debug_printf(" certifi failed \r\n");
			return LOCK_ERROR;
	}
	// 记录Log
	*permision=key_board_flash[flash_cur].user_permision;
	if(get_cur_info->cur_log_number>=LOG_NUM)
	{
		get_cur_info->cur_log_number=0;
	}
	
	HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
	if(app_upload_flag)
	{
		*permision=permision_app;
		sys_log_record(flash_cur,OPEN_DOOR,*permision,APP_TYPE,get_cur_info->cur_log_number);
	}else{
		sys_log_record(flash_cur,OPEN_DOOR,*permision,KEYBOARD_TYPE,get_cur_info->cur_log_number);
	}
	get_cur_info->cur_log_number++;
	
	// 电池电量统计
	battery_value = sys_battery_get();
	battery_class = wifi_upload_battery(battery_value);
	
	get_cur_info->cur_battery = battery_class;
	
	stm_flash_erase_write(ADDR_FLASH_PAGE_100,(uint64_t*)get_cur_info,CUR_INDEX_SIZE);
	
	// 保存要上报的信息到缓存区
	stm_flash_read(ADDR_FLASH_PAGE_70,(uint8_t*)wifi_flash,WIFI_SIZE);
	
	wifi_power_on();
	Revalue =wifi_flash->index;
	wifi_flash[Revalue].exist=1;
	
	sprintf(lock_time,"20%02d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year,sdatestructure.Month,
																				sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);
	if(battery_class<=10)
	{
		set_win6170(62);
		display_battery_warn();
		sys_delay_us(3000000);
		sprintf(wifi_flash[Revalue].upload_info,"%d%%%d%%%s%%",WARN_UPLOAD,LOW_POWER_WARN,lock_time);
	}else
		sprintf(wifi_flash[Revalue].upload_info,"%d%%%d%%%s%%",BATTERY_UPLOAD,battery_class,lock_time);
	
	debug_printf("wifi_button: %d \r\n",get_cur_info->wifi_button);
	if(wifi_flash->index<99)
	{
		Revalue++;
		wifi_flash->index++;
	}
	
	if(*permision==ANTI_HAK)
	{
			wifi_flash[Revalue].exist=1;
			sprintf(wifi_flash[Revalue].upload_info,"%d%%%d%%%s%%",WARN_UPLOAD,
																				ANTI_HIJACK_WARN,lock_time);
			if(wifi_flash->index<99)
				wifi_flash->index++;
	}else{
		if(get_cur_info->wifi_button)
		{
			wifi_flash[Revalue].exist=1;
			sprintf(wifi_flash[Revalue].upload_info,"%d%%%d%%%d%%%d%%%d%%%s%%",OPEN_UPLOAD,
																				flash_cur,KEYBOARD_TYPE,*permision,OPEN_DOOR,lock_time);
			if(wifi_flash->index<99)
				wifi_flash->index++;
		}
	}

	// 保存上报信息到flash
	if(app_upload_flag!=1)
		stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);

	// 显示开锁界面
	if(app_upload_flag)
	{
		display_confirm(APP_TYPE,permision_app,index_app);
	}else{
		display_confirm(KEYBOARD_TYPE,*permision,flash_cur);
	}
	prying_flag=0;
	set_win6170(get_cur_info->cur_vol_size);
	return LOCK_OK;
}
