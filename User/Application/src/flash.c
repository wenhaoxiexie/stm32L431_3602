#include "flash.h"
#include "usart.h"
#include "win6170.h"
#include "fpcst90x.h"
#include "wifi_8266.h"
#include "apt8l16.h"

keyboard_store_t    *key_board_flash=NULL;
finger_store_t      *finger_flash=NULL;
cur_index_t         *get_cur_info=NULL;
nfc_store_t         *nfc_flash=NULL;
log_store_t         *log_flash=NULL; 
wifi_upload_store_t *wifi_flash=NULL;

extern uint8_t facteryReset;
extern int8_t battery_class;

void flash_init()
{	
		uint16_t i;
	
		flash_exit();
		key_board_flash=(keyboard_store_t*)calloc(KEYBOARD_NUM,sizeof(keyboard_store_t));
		
		if(key_board_flash==NULL)
		{
			debug_printf(" calloc key_board_flash failed \r\n");
			return ;
		}
		
		finger_flash=(finger_store_t*)calloc(FINGER_NUM,sizeof(finger_store_t));
		if(finger_flash==NULL)
		{
			debug_printf(" calloc finger_flash failed \r\n");
			return ;
		}
		
		nfc_flash=(nfc_store_t*)calloc(CARD_NUM,sizeof(nfc_store_t));
		if(nfc_flash==NULL)
		{
			debug_printf(" calloc nfc_flash flash failed \r\n");
			return ;
		}
		
		get_cur_info=(cur_index_t*)calloc(1,sizeof(cur_index_t));
		if(get_cur_info==NULL)
		{
			debug_printf("calloc get_cur_info flash failed \r\n");
			return ;
		}
		
		log_flash=(log_store_t*)calloc(LOG_NUM,sizeof(log_store_t));
		if(log_flash==NULL)
		{
			debug_printf(" calloc log_flash failed \r\n");
			return ;
		}
		wifi_flash=(wifi_upload_store_t*)calloc(100,sizeof(wifi_upload_store_t));
		if(wifi_flash==NULL)
		{
			debug_printf(" calloc wifi_flash failed \r\n");
			return ;
		}
		// 读取全局区数据
		stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);

		if(get_cur_info->cur_index_number>=1&&get_cur_info->cur_index_number<300)
		{
			// 读取flash 中所有的密码
			stm_flash_read(ADDR_FLASH_PAGE_56,(uint8_t*)key_board_flash,KEYBOARD_SIZE);
	
			// 读取flash中所有的指纹
			stm_flash_read(ADDR_FLASH_PAGE_58,(uint8_t*)finger_flash,FINGER_SIZE);

			// 读取flash 中所有的卡片信息
			stm_flash_read(ADDR_FLASH_PAGE_60,(uint8_t*)nfc_flash,CARD_SIZE);

			//读取log信息
			stm_flash_read(ADDR_FLASH_PAGE_102,(uint8_t*)log_flash,LOG_SIZE);
			
			// 读取wifi 模块未上报数据
			stm_flash_read(ADDR_FLASH_PAGE_70,(uint8_t*)wifi_flash,WIFI_SIZE);
			
			// 恢复用户音量设置
			set_win6170(get_cur_info->cur_vol_size);
			battery_class = get_cur_info->cur_battery;
		}else{
			debug_printf(" the first erase... \r\n");
			
			// 初始化密码动态空间
			for(i=0;i<KEYBOARD_NUM;i++)
			{
				key_board_flash[i].user_permision=0xff;
				key_board_flash[i].user_type=KEYBOARD_TYPE;
				key_board_flash[i].index=0xff;
				key_board_flash[i].data_len=0;
				key_board_flash[i].del_flag=0xff;
				memset(key_board_flash[i].data,0xff,sizeof(key_board_flash[i].data));
			}
			stm_flash_erase_write(ADDR_FLASH_PAGE_56,(uint64_t*)key_board_flash,KEYBOARD_SIZE);
			
			// 初始化指纹动态空间
			for(i=0;i<FINGER_NUM;i++)
			{
				finger_flash[i].index=0xff;
				finger_flash[i].user_type=FINGER_TYPE;
				finger_flash[i].user_permision=0xff;
				finger_flash[i].del_flag=0xff;
			}
			stm_flash_erase_write(ADDR_FLASH_PAGE_58,(uint64_t*)finger_flash,FINGER_SIZE);
			
			// 初始化卡片空间
			for(i=0;i<CARD_NUM;i++)
			{
				nfc_flash[i].index=0xff;
				nfc_flash[i].user_permision=0xff;
				nfc_flash[i].user_type=CARD_TYPE;
				nfc_flash[i].del_flag=0xff;
				nfc_flash[i].uid_len=0;
				memset(nfc_flash[i].uid,0xff,sizeof(nfc_flash[i].uid));
			}
			stm_flash_erase_write(ADDR_FLASH_PAGE_60,(uint64_t*)nfc_flash,CARD_SIZE);
			
			// 初始化全局
			get_cur_info->cur_card_index=0;
			get_cur_info->cur_fingerprint_index=0;
			get_cur_info->cur_index_number=0;
			get_cur_info->cur_keyboard_index=0;
			get_cur_info->cur_vol_size=0xef; 
			get_cur_info->cur_log_number=0;
			get_cur_info->wifi_button=1;
			get_cur_info->cur_battery=100;
			//strcpy((char*)get_cur_info->wifi_mac,"FFFFFFFFFFFF");
		
			stm_flash_erase_write(ADDR_FLASH_PAGE_100,(uint64_t*)get_cur_info,CUR_INDEX_SIZE);
			
			
			// 初始化操作日志
			for(i=0;i<LOG_NUM;i++)
			{
				log_flash[i].index=0;
				log_flash[i].state=OPERATOR_MAX;
				log_flash[i].permision=ANDMI;
				log_flash[i].type=FINGER_TYPE;
				log_flash[i].date.Year=18;
				log_flash[i].date.Month=1;
				log_flash[i].date.Date=1;
				log_flash[i].time.Hours=0;
				log_flash[i].time.Minutes=0;
				log_flash[i].time.Seconds=0;
			}
			
			stm_flash_erase_write(ADDR_FLASH_PAGE_102,(uint64_t*)log_flash,LOG_SIZE);
				
			// 初始化wifi上传buf
			wifi_flash->exist=0;
			wifi_flash->index=0;
			//memset(wifi_flash->lock_time,0,sizeof(wifi_flash->lock_time));
			memset(wifi_flash->upload_info,0,sizeof(wifi_flash->upload_info));
			
			stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);
			debug_printf("flash init finish...\r\n");
			// 初始化语音音量大小
			set_win6170(0xEF);
		}
}


void flash_exit(void)
{
	if(key_board_flash!=NULL)
	{
		free(key_board_flash);
		key_board_flash=NULL;
	}
	
	if(finger_flash!=NULL)
	{
		free(finger_flash);
		finger_flash=NULL;
	}
	
	if(get_cur_info!=NULL)
	{
		free(get_cur_info);
		get_cur_info=NULL;
	}
	if(log_flash!=NULL)
	{
		free(log_flash);
		log_flash=NULL;
	}
	if(nfc_flash!=NULL)
	{
		free(nfc_flash);
		nfc_flash=NULL;
	}
	if(wifi_flash!=NULL)
	{
		free(wifi_flash);
		wifi_flash=NULL;
	}
}

void factory_reset(void)
{
	uint8_t pPakege[128];
	uint8_t pResult;
	uint16_t pLen;
	FP_StatusTypeDef reValue;
	uint8_t count;
	
	
	display_sys_reset();
	
	fp_power_on();		

	// wifi模块
	HAL_UART_Transmit(&huart3, (uint8_t*)AT_RSTORE, sizeof(AT_RSTORE),100);
	
	stm_flash_erase(ADDR_FLASH_PAGE_56,KEYBOARD_SIZE);
	stm_flash_erase(ADDR_FLASH_PAGE_58,FINGER_SIZE);
	stm_flash_erase(ADDR_FLASH_PAGE_60,CARD_SIZE);
	stm_flash_erase(ADDR_FLASH_PAGE_100,CUR_INDEX_SIZE);
	stm_flash_erase(ADDR_FLASH_PAGE_102,LOG_SIZE);
	stm_flash_erase(ADDR_FLASH_PAGE_70,WIFI_SIZE);
	
	// 清空指纹模块数据
	do{
		pLen=0;
		reValue = fp_tarns_recv_command(MASK_INS, PS_Empty, NULL, &pLen, pPakege, &pResult,500);
		if(reValue==FP_OK&&pResult==0)
			break;
		count++;
	}while(count<3);
	
	//清空wifi模块
	flash_init();
	
	fp_sleep();
	
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	set_win6170(0xef);
	sys_delay_us(2000);
	get_cur_info->cur_vol_size=0xef;
	stm_flash_erase_write(ADDR_FLASH_PAGE_100,(uint64_t*)get_cur_info,CUR_INDEX_SIZE);
	facteryReset=1;
	
	send_bytes(KEY_ALL_ON);
	APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
	
	MX_RTC_Init();
}

