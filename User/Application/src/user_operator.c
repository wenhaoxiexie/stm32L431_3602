#include "user_operator.h"
#include "global.h"
#include "ssd1305.h"
#include "flash.h"
#include "fpcst90x.h"
#include "menu.h"
#include "win6170.h"
#include "keyboard.h"
#include "rtc.h"
#include "nfc.h"
#include "74hc595.h"
#include "wifi_8266.h"
#include "usart.h"

static uint32_t delay_count;

extern KEY_STATE get_cur_number;

extern RTC_DateTypeDef sdatestructure;
extern RTC_TimeTypeDef stimestructure;
extern uint8_t powerOffFlag;

uint8_t user_add_menu(void)
{
	
START:
	display_submenu_add();
	send_bytes(KEY_ADD_MENU);
	APT_KeyEnableSel(KDR0_CH_1_2_3_10_11_EN,KDR1_CH_1_2_3_10_11_EN);
	
RECALL:
	delay_count=1300;

	voice_repeat(13);
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			delay_count=1300;
			switch(get_cur_number.number){
				case 1:
					// 管理员
					if(user_add_menu_sub(ANDMI)==LOCK_OK)
						goto START;
					else
						return LOCK_OUTTIME;
				case 2:
					// 普通用户
					if(user_add_menu_sub(USUALLY)==LOCK_OK)
						goto START;
					else
						return LOCK_OUTTIME;
				case 3:
					// 防劫持用户
					if(user_add_hak_menu_sub(ANTI_HAK)==LOCK_OK)
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

uint8_t user_add_menu_sub(uint8_t permision)
{
	uint8_t reValue;
START:
	display_subsubmenu_add(1);
	send_bytes(KEY_ADD_MENU);
	APT_KeyEnableSel(KDR0_CH_1_2_3_10_11_EN,KDR1_CH_1_2_3_10_11_EN);
	
RECALL:

	voice_repeat(14);

	delay_count=1300;
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			delay_count=1300;
			switch(get_cur_number.number){
				case 1:
					// 添加指纹
					reValue = fp_add(permision);
					fp_led_white();
					fp_led_shutdown();
					if(reValue == LOCK_OK)
						goto START;
					else if(reValue == LOCK_EXIT)
					{
						set_win6170(21);
						display_finger_exit();
						sys_delay_us(1000000);
						goto START;
					}
					else if(reValue == LOCK_ERROR){
						set_win6170(43);
						display_finger_fail();
						sys_delay_us(1000000);
						goto START;
					}else if(reValue == LOCK_MAX){
						set_win6170(29);
						display_finger_full();
						sys_delay_us(1000000);
						goto START;
					}else if(reValue == LOCK_OUTTIME_FP){
						goto START;
					}else{
						goto START;
					}
				case 2:
					// 添加密码
					user_add_keyboard_menu(permision);
					goto START;
				case 3:
					// 添加感应卡
					reValue = nfc_add(permision);
					if(reValue == LOCK_OK)
						goto START;
					else if(reValue == LOCK_MAX){
						set_win6170(35);
						display_card_full();
						sys_delay_us(1000000);
						goto START;
					}
					else if(reValue == LOCK_EXIT){
						set_win6170(26);
						display_card_exit();
						sys_delay_us(1000000);
						goto START;
					}
					else if(reValue == LOCK_ERROR){
						set_win6170(43);
						display_operate_fail();
						sys_delay_us(1000000);
						goto START;
					}else{
						goto START;
					}
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

uint8_t user_add_hak_menu_sub(uint8_t permision)
{
	uint8_t reValue;
START:
	display_subsubmenu_add(0);
	send_bytes(KEY_ADD_ANTI_MENU);
	APT_KeyEnableSel(KDR0_CH_1_2_10_11_EN,KDR1_CH_1_2_10_11_EN);
	
RECALL:

	voice_repeat(15);

	delay_count=1300;
	
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			delay_count=1300;
			switch(get_cur_number.number){
				case 1:
						// 添加指纹
					reValue = fp_add(permision);
					fp_led_white();
					fp_led_shutdown();
					if(reValue == LOCK_OK)
						goto START;
					else if(reValue == LOCK_EXIT)
					{
						set_win6170(21);
						display_finger_exit();
						sys_delay_us(1000000);
						goto START;
					}
					else if(reValue == LOCK_ERROR){
						set_win6170(43);
						display_finger_fail();
						sys_delay_us(1000000);
						goto START;
					}else if(reValue == LOCK_MAX){
						set_win6170(29);
						display_finger_full();
						sys_delay_us(1000000);
						goto START;
					}else if(reValue == LOCK_OUTTIME_FP){
						goto START;
					}else{
						goto START;
					}
				case 2:
					// 添加密码
					user_add_keyboard_menu(permision);
						goto START;
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

uint8_t user_delete_menu(void)
{
	
START:
	display_submenu_add();
	send_bytes(KEY_ADD_MENU);
	APT_KeyEnableSel(KDR0_CH_1_2_3_10_11_EN,KDR1_CH_1_2_3_10_11_EN);
RECALL:

	voice_repeat(13);
	
	delay_count=1300;
	
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			delay_count=1300;
			switch(get_cur_number.number){
				case 1:
					// 管理员
					if(user_delete_menu_sub(ANDMI)==LOCK_OK)
						goto START;
					else
						return LOCK_OUTTIME;
				case 2:
					// 普通用户
					if(user_delete_menu_sub(USUALLY)==LOCK_OK)
						goto START;
					else
						return LOCK_OUTTIME;
				case 3:
					// 防劫持用户
					if(user_delete_hak_menu_sub(ANTI_HAK)==LOCK_OK)
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

uint8_t user_delete_menu_sub(uint8_t permision)
{

START:
	display_submenu_del(1);
	send_bytes(KEY_ADD_MENU);
	APT_KeyEnableSel(KDR0_CH_1_2_3_10_11_EN,KDR1_CH_1_2_3_10_11_EN);
RECALL:

	voice_repeat(16);

	delay_count=1300;
	
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			delay_count=1300;
			switch(get_cur_number.number)
			{
				case 1:
					//删除指纹
					if(user_delete_fp_menu(permision)==LOCK_OK)
						goto START;
					else
						return LOCK_OUTTIME;
				case 2:
					//删除密码
					if(user_delete_password_menu(permision)==LOCK_OK)
						goto START;
					else
						return LOCK_OUTTIME;
				case 3:
					//删除卡片
					if(user_delete_card_menu(permision)==LOCK_OK)
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

uint8_t user_delete_hak_menu_sub(uint8_t permision)
{

START:
	display_submenu_del(0);
	send_bytes(KEY_ADD_ANTI_MENU);
	APT_KeyEnableSel(KDR0_CH_1_2_10_11_EN,KDR1_CH_1_2_10_11_EN);
RECALL:

	voice_repeat(17);
	
	delay_count=1300;
	
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			delay_count=1300;
			switch(get_cur_number.number)
			{
				case 1:
					//删除指纹
					if(user_delete_fp_menu(permision)==LOCK_OK)
						goto START;
					else
						return LOCK_OUTTIME;
				case 2:
					//删除密码
					if(user_delete_password_menu(permision)==LOCK_OK)
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

uint8_t user_add_keyboard_menu(uint8_t permision)
{
	uint8_t number_cur;
	uint8_t number_cur_again;
	uint8_t password[10]={0};
	uint8_t password_again[10]={0};
	uint8_t input_count=0;
	uint8_t check_flag=0;
	uint8_t i,j,k;
	uint8_t reValue;
	
LOOP_MAIN:
	input_count=0;
	check_flag=0;
	display_passwod_warn();
	set_win6170(0XF3);
	sys_delay_us(2000);
	set_win6170(22);
	send_bytes(KEY_ALL_ON);
	APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
START:
	number_cur=0;
	delay_count=80000;

	do{
		if(get_cur_number.key_prees==1)
		{
			delay_count=80000;
			get_cur_number.key_prees=0;
		
			if(get_cur_number.number==10)
			{
				if(number_cur==0)
					return LOCK_ERROR;
				display_number_del(number_cur--);
			}
			else if(get_cur_number.number==11)
			{
				if(number_cur<6||check_flag==1||number_cur>10)
				{
					display_check_failed();
					sys_delay_us(100);
					delay_count=0;
					return LOCK_ERROR;
				}else{
					input_count++;
					if(input_count<2)
					{
						stm_flash_read(ADDR_FLASH_PAGE_56,(uint8_t*)key_board_flash,KEYBOARD_SIZE);
						// 验证密码是否已存在
						for(i=0;i<KEYBOARD_NUM/*get_cur_info->cur_keyboard_index*/;i++)
						{
							if(key_board_flash[i].del_flag==0)
							{
								if(key_board_flash[i].data_len!=number_cur)
								continue;
								for(j=0;j<number_cur;j++)
								{
									if(password[j]!=key_board_flash[i].data[j])
										break;
								}
								if(j==number_cur)
								{
									display_passwod_exit();
									set_win6170(24);
									sys_delay_us(1000000);
									input_count--;
									goto LOOP_MAIN;
									
								}
							}
						}
						display_passwod_warn_again();
						set_win6170(23);
						
						for(k=0;k<number_cur;k++)
							password_again[k] = password[k];
						//strncpy((char*)password_again,(char*)password,number_cur);
						
						number_cur_again =number_cur;
						goto START;
					}
					debug_printf("[%d %d] \r\n",number_cur,number_cur_again);
				
					// 验证前后输入密码是否一致
					for(i=0;i<number_cur;i++)
					{
						if(password_again[i]!=password[i]||number_cur_again!=number_cur)
							break;
					}
					if(i!=number_cur)
					{
						set_win6170(43);
						display_operate_fail();
						sys_delay_us(1000000);
						goto LOOP_MAIN;
					}
					// 将数据保存在flash中
					reValue = keyboard_password_save(password,number_cur,permision);
					if(reValue==LOCK_OK)
					{
						return LOCK_OK;
					}
					else if(reValue==LOCK_MAX)
					{
						set_win6170(32);	
						display_password_full();
						sys_delay_us(500000);
						return LOCK_OK;
					}
				}
			}
			else
			{
				if(number_cur<10)
				{
						password[number_cur] = get_cur_number.number;
						display_number(get_cur_number.number,number_cur);
						number_cur++;
				}
			}
			
		}
		sys_delay_us(100);
	}while(--delay_count);
	
	return LOCK_OUTTIME;
}


uint8_t user_delete_password_menu(uint8_t permision)
{

	uint8_t press_count;
	uint8_t search_count;
	uint8_t temp[100];
	int8_t i;
	uint8_t lock_num;
	uint8_t lock_flag=0;
	uint8_t log_search;
	
	
LOOP_MAIN:
	press_count=0;
	search_count=0;
	memset(temp,0xff,100);
	send_bytes(KEY_TURN_PAGE);
	APT_KeyEnableSel(KDR0_CH_2_8_10_11_EN,KDR1_CH_2_8_10_11_EN);

	stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
	stm_flash_read(ADDR_FLASH_PAGE_56,(uint8_t*)key_board_flash,KEYBOARD_SIZE);
	stm_flash_read(ADDR_FLASH_PAGE_102,(uint8_t*)log_flash,LOG_SIZE);
	
	debug_printf("get_cur_info->cur_keyboard_index:%d \r\n",get_cur_info->cur_keyboard_index);
	debug_printf("get_cur_info->cur_log_number:%d \r\n",get_cur_info->cur_log_number);
	
	//查找最近一次开锁
	for(log_search=get_cur_info->cur_log_number;log_search>0;log_search--)
	{
		if(log_flash[log_search].state==OPEN_DOOR)
			break;
	}
	
	if(log_flash[log_search].type==KEYBOARD_TYPE)
	{
		lock_flag=1;
		lock_num=log_flash[log_search].index;
		debug_printf("keyboard open the door [%d]\r\n", lock_num);
	}
	
	debug_printf(" get_cur_info->cur_keyboard_index:%d \r\n",get_cur_info->cur_keyboard_index);
	if(get_cur_info->cur_keyboard_index>0)
	{	
		// 统计所有的密码到数组中
		for(i=0;i<KEYBOARD_NUM;i++)
		{
			if(key_board_flash[i].del_flag==0&&key_board_flash[i].user_permision==permision)
			{
				debug_printf(" !!!!!!!!!!! i:%d\r\n",i);
				temp[search_count++]=key_board_flash[i].index;
			}
		}
		// 未找到
		if(search_count==0)
		{
			set_win6170(31);
			display_passwod_null();
			sys_delay_us(1000000);
			return LOCK_OK;
		}
		if(search_count>=1)
			set_win6170(39);

		// 显示起始页
		All_Screen();
		for(i=0;i<search_count;i++)
		{
			if(i==0)
				display_number_password_const(temp[i],i*2,lock_flag,lock_num);
			else{
				if(i>3)
					break;
				display_number_password(temp[i],i*2,lock_flag,lock_num);
			}
		}
LOOP:
		//向下翻页
		
		if(get_cur_number.number==8)
		{
			if(press_count%4==0)
			{	
				debug_printf(" ############### search_count:%d\r\n",search_count);
				All_Screen();
				for(i=press_count;i<search_count;i++)
				{
					debug_printf(" ############### i:%d\r\n",i);
					if(i==press_count)
					{
						display_number_password_const(temp[press_count],press_count%4*2,lock_flag,lock_num);
					}
					else{
						if(i-press_count==4)
							break;
						display_number_password(temp[i],i%4*2,lock_flag,lock_num);
					}
				}
			}else{
				debug_printf(" $$$$$$$$$$$$$$$$$ \r\n");
				display_number_password(temp[press_count-1],(press_count-1)%4*2,lock_flag,lock_num);
				display_number_password_const(temp[press_count],press_count%4*2,lock_flag,lock_num);
			}
		}
		else if(get_cur_number.number==2)
		{
			// 向上翻页
			
			if(search_count<3)
			{
				debug_printf(" ^^^^^^^^^^ press_count:%d search_count:%d\r\n",press_count,search_count);
				if(press_count+1==search_count)
				{
					display_number_password(temp[0],0,lock_flag,lock_num);
					display_number_password_const(temp[press_count],press_count%4*2,lock_flag,lock_num);
				}
				else{
					display_number_password(temp[press_count+1],(press_count+1)%4*2,lock_flag,lock_num);
					display_number_password_const(temp[press_count],press_count%4*2,lock_flag,lock_num);
				}
			}else{
				if((press_count-3)%4==0)
				{
					debug_printf(" *********************** \r\n");
					All_Screen();
					for(i=press_count;i>=0;i--)
					{
						if(i==press_count)
							display_number_password_const(temp[press_count],press_count%4*2,lock_flag,lock_num);
						else{
							if(press_count-i==4)
								break;
							display_number_password(temp[i],i%4*2,lock_flag,lock_num);
						}
					}
				}
				else{
					debug_printf(" &&&&&&&&&&&&&&&&&&&&&&& \r\n");
					display_number_password(temp[press_count+1],(press_count+1)%4*2,lock_flag,lock_num);
					display_number_password_const(temp[press_count],press_count%4*2,lock_flag,lock_num);
				}
			}
		}
		delay_count=50000;
		do{
			if(get_cur_number.key_prees==1)
			{
					get_cur_number.key_prees=0;
					if(get_cur_number.number==2)
					{
						if(press_count==0)
							press_count=search_count;
						press_count--;
						debug_printf(" press_count:%d \r\n",press_count);
						goto LOOP;
					}
					else if(get_cur_number.number==8)
					{
						press_count++;
						if(press_count==search_count)
							press_count=0;
						debug_printf(" press_count:%d \r\n",press_count);
						goto LOOP;
					}
					else if(get_cur_number.number==11)
					{
						if(lock_flag&&temp[press_count]==lock_num)
						{
							set_win6170(51);
							display_del_failed();
							sys_delay_us(1800000);
							goto LOOP_MAIN;
						}
							
						// 删除操作
						if(keyboard_password_del(temp[press_count],permision)==LOCK_OK)
						{
							set_win6170(42);
							display_del_sucessful();
							sys_delay_us(800000);
						}else{
							set_win6170(43);
							display_del_failed();
							sys_delay_us(800000);
						}
						goto LOOP_MAIN;
					}
					else if(get_cur_number.number==10)
					{
						return LOCK_OK;
					}
			}
			sys_delay_us(100);
		}while(--delay_count);
		
		return LOCK_OUTTIME;
	}else{
		display_passwod_null();
		set_win6170(0XF3);
		sys_delay_us(2000);
		set_win6170(31);
		sys_delay_us(1000000);
		return LOCK_OK;
	}
}

uint8_t user_delete_fp_menu(uint8_t permision)
{
	uint8_t press_count;
	uint8_t search_count;
	uint8_t temp[100];
	uint16_t delay_count;
	int8_t i;
	uint8_t lock=0;
	uint8_t lock_num=0;
	uint8_t search_log;
	
LOOP_MAIN:
	press_count=0;
	search_count=0;
	memset(temp,0xff,100);
	send_bytes(KEY_TURN_PAGE);
	APT_KeyEnableSel(KDR0_CH_2_8_10_11_EN,KDR1_CH_2_8_10_11_EN);

	stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
	stm_flash_read(ADDR_FLASH_PAGE_58,(uint8_t*)finger_flash,FINGER_SIZE);
	stm_flash_read(ADDR_FLASH_PAGE_102,(uint8_t*)log_flash,LOG_SIZE);
	
	debug_printf("get_cur_info->cur_fingerprint_index:%d \r\n",get_cur_info->cur_fingerprint_index);
	debug_printf("get_cur_info->cur_log_number:%d \r\n",get_cur_info->cur_log_number);
	debug_printf("type:%d \r\n",log_flash->type);
	
	//查找最近一次开锁
	for(search_log=get_cur_info->cur_log_number;search_log>0;search_log--)
	{
			if(log_flash[search_log].state==OPEN_DOOR)
				break;
	}
	debug_printf("search_log:%d \r\n",search_log);
	if(log_flash[search_log].type==FINGER_TYPE&&log_flash[search_log].state==OPEN_DOOR)
	{
		lock=1;
		lock_num = log_flash[search_log].index;
	}
	if(get_cur_info->cur_fingerprint_index>0)
	{	
		// 统计所有的指纹到数组中
		for(i=0;i</*get_cur_info->cur_fingerprint_index*/FINGER_NUM;i++)
		{
			if(finger_flash[i].del_flag==0&&finger_flash[i].user_permision==permision)
			{
				debug_printf(" !!!!!!!!!!! i:%d\r\n",i);
				temp[search_count++]=finger_flash[i].index;
			}
		}
		if(search_count==0)
		{
			set_win6170(28);
			display_finger_null();
			sys_delay_us(1000000);
			return LOCK_OK;
		}
		if(search_count>=1)
			set_win6170(39);
		// 显示起始页
		All_Screen();
		for(i=0;i<search_count;i++)
		{
			if(i==0)
				display_number_finger_const(temp[i],i*2,lock,lock_num);
			else{
				if(i>3)
					break;
				display_number_finger(temp[i],i*2,lock,lock_num);
			}
		}
		
LOOP:
		//向下翻页
		if(get_cur_number.number==8)
		{
			if(press_count%4==0)
			{	
				debug_printf(" ############### search_count:%d\r\n",search_count);
				All_Screen();
				for(i=press_count;i<search_count;i++)
				{
					debug_printf(" ############### i:%d\r\n",i);
					if(i==press_count)
					{
						display_number_finger_const(temp[press_count],press_count%4*2,lock,lock_num);
					}
					else{
						if(i-press_count==4)
							break;
						display_number_finger(temp[i],i%4*2,lock,lock_num);
					}
				}
			}else{
				debug_printf(" $$$$$$$$$$$$$$$$$ \r\n");
				display_number_finger(temp[press_count-1],(press_count-1)%4*2,lock,lock_num);
				display_number_finger_const(temp[press_count],press_count%4*2,lock,lock_num);
			}
		}
		else if(get_cur_number.number==2)
		{
			// 向上翻页
			
			if(search_count<3)
			{
				debug_printf(" ^^^^^^^^^^ press_count:%d search_count:%d\r\n",press_count,search_count);
				if(press_count+1==search_count)
				{
					display_number_finger(temp[0],0,lock,lock_num);
					display_number_finger_const(temp[press_count],press_count%4*2,lock,lock_num);
				}
				else{
					display_number_finger(temp[press_count+1],(press_count+1)%4*2,lock,lock_num);
					display_number_finger_const(temp[press_count],press_count%4*2,lock,lock_num);
				}
			}else{
				if((press_count-3)%4==0)
				{
					debug_printf(" *********************** \r\n");
					All_Screen();
					for(i=press_count;i>=0;i--)
					{
						if(i==press_count)
							display_number_finger_const(temp[press_count],press_count%4*2,lock,lock_num);
						else{
							if(press_count-i==4)
								break;
							display_number_finger(temp[i],i%4*2,lock,lock_num);
						}
					}
				}
				else{
					debug_printf(" &&&&&&&&&&&&&&&&&&&&&&& \r\n");
					display_number_finger(temp[press_count+1],(press_count+1)%4*2,lock,lock_num);
					display_number_finger_const(temp[press_count],press_count%4*2,lock,lock_num);
				}
			}
		}
		delay_count=50000;
		do{
			if(get_cur_number.key_prees==1)
			{
					get_cur_number.key_prees=0;
					if(get_cur_number.number==2)
					{
						if(press_count==0)
							press_count=search_count;
						press_count--;
						debug_printf(" press_count:%d \r\n",press_count);
						goto LOOP;
					}
					else if(get_cur_number.number==8)
					{
						press_count++;
						if(press_count==search_count)
							press_count=0;
						debug_printf(" press_count:%d \r\n",press_count);
						goto LOOP;
					}
					else if(get_cur_number.number==11)
					{
						if(temp[press_count]==lock_num&&lock)
						{
							set_win6170(51);
							display_finger_del_fail();
							sys_delay_us(1800000);
							goto LOOP_MAIN;
						}
						// 删除操作
						fp_delete(temp[press_count], permision);
						
						goto LOOP_MAIN;
					}
					else if(get_cur_number.number==10)
					{
						return LOCK_OK;
					}
			}
			sys_delay_us(100);
		}while(--delay_count);
		
		return LOCK_OUTTIME;
	}else{
		set_win6170(28);
		display_finger_null();
		sys_delay_us(500000);
		return LOCK_OK;
	}
}

uint8_t user_delete_card_menu(uint8_t permision)
{
	uint8_t press_count;
	uint8_t search_count;
	uint8_t temp[100];
	uint16_t delay_count;
	int8_t i;
	uint8_t lock_num=200;
	uint8_t lock_flag=0;
	uint8_t log_search;
	
LOOP_MAIN:
	press_count=0;
	search_count=0;
	memset(temp,0xff,100);
	send_bytes(KEY_TURN_PAGE);
	APT_KeyEnableSel(KDR0_CH_2_8_10_11_EN,KDR1_CH_2_8_10_11_EN);
	
	stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
	stm_flash_read(ADDR_FLASH_PAGE_60,(uint8_t*)nfc_flash,CARD_SIZE);
	stm_flash_read(ADDR_FLASH_PAGE_102,(uint8_t*)log_flash,LOG_SIZE);
	
	debug_printf("get_cur_info->cur_fingerprint_index:%d \r\n",get_cur_info->cur_fingerprint_index);
	debug_printf("get_cur_info->cur_log_number:%d \r\n",get_cur_info->cur_log_number);
	
	// 查找最近开锁
	for(log_search=get_cur_info->cur_log_number;log_search>0;log_search--)
	{
			if(log_flash[log_search].state==OPEN_DOOR)
				break;
	}
	if(log_flash[log_search].type==CARD_TYPE)
	{
		lock_flag=1;
		lock_num=log_flash[log_search].index;
	}
	
	if(get_cur_info->cur_card_index>0)
	{	
		// 统计所有的卡片到数组中
		for(i=0;i<CARD_NUM;i++)
		{
			if(nfc_flash[i].del_flag==0&&nfc_flash[i].user_permision==permision)
			{
				debug_printf(" !!!!!!!!!!! i:[%d:%d]\r\n",i,nfc_flash[i].index);
				temp[search_count++]=nfc_flash[i].index;
			}
		}
		debug_printf("search_count: %d \r\n",search_count);
		
		if(search_count==0)
		{
			set_win6170(34);
			display_card_null();
			sys_delay_us(1000000);
			return LOCK_OK;
		}
		if(search_count>=1)
			set_win6170(39);
	
		// 显示起始页
		All_Screen();
		debug_printf("temp: %d : %d\r\n",temp[0],temp[1]);
		for(i=0;i<search_count;i++)
		{
			if(i==0)
				display_number_card_const(temp[i],i*2,lock_flag,lock_num);
			else{
				if(i>3)
					break;
				debug_printf("!!!!!!!!!!! %d\r\n",i);
				display_number_card(temp[i],i*2,lock_flag,lock_num);
			}
		}
		debug_printf("i:%d \r\n",i);
LOOP:
		//向下翻页
		
		if(get_cur_number.number==8)
		{
			if(press_count%4==0)
			{	
				debug_printf(" ############### search_count:%d\r\n",search_count);
				All_Screen();
				for(i=press_count;i<search_count;i++)
				{
					debug_printf("############### i:%d\r\n",i);
					if(i==press_count)
					{
						display_number_card_const(temp[press_count],press_count%4*2,lock_flag,lock_num);
					}
					else{
						if(i-press_count==4)
							break;
						display_number_card(temp[i],i%4*2,lock_flag,lock_num);
					}
				}
			}else{
				debug_printf(" $$$$$$$$$$$$$$$$$ \r\n");
				display_number_card(temp[press_count-1],(press_count-1)%4*2,lock_flag,lock_num);
				display_number_card_const(temp[press_count],press_count%4*2,lock_flag,lock_num);
			}
		}
		else if(get_cur_number.number==2)
		{
			// 向上翻页
			if(search_count<3)
			{
				debug_printf(" ^^^^^^^^^^ press_count:%d search_count:%d\r\n",press_count,search_count);
				if(press_count+1==search_count)
				{
					display_number_card(temp[0],0,lock_flag,lock_num);
					display_number_card_const(temp[press_count],press_count%4*2,lock_flag,lock_num);
				}
				else{
					display_number_card(temp[press_count+1],(press_count+1)%4*2,lock_flag,lock_num);
					display_number_card_const(temp[press_count],press_count%4*2,lock_flag,lock_num);
				}
			}else{
				if((press_count-3)%4==0)
				{
					debug_printf(" *********************** \r\n");
					All_Screen();
					for(i=press_count;i>=0;i--)
					{
						if(i==press_count)
							display_number_card_const(temp[press_count],press_count%4*2,lock_flag,lock_num);
						else{
							if(press_count-i==4)
								break;
							display_number_card(temp[i],i%4*2,lock_flag,lock_num);
						}
					}
				}
				else{
					debug_printf(" &&&&&&&&&&&&&&&&&&&&&&& \r\n");
					display_number_card(temp[press_count+1],(press_count+1)%4*2,lock_flag,lock_num);
					display_number_card_const(temp[press_count],press_count%4*2,lock_flag,lock_num);
				}
			}
		}
		delay_count=800;
		do{
			if(get_cur_number.key_prees==1)
			{
					delay_count=800;
					get_cur_number.key_prees=0;
					if(get_cur_number.number==2)
					{
						if(press_count==0)
							press_count=search_count;
						press_count--;
						debug_printf(" press_count:%d \r\n",press_count);
						goto LOOP;
					}
					else if(get_cur_number.number==8)
					{
						press_count++;
						if(press_count==search_count)
							press_count=0;
						debug_printf("press_count:%d \r\n",press_count);
						goto LOOP;
					}
					else if(get_cur_number.number==11)
					{
						debug_printf("############ %d:%d \r\n",lock_num,temp[press_count]);
						if(lock_flag&&lock_num==temp[press_count])
						{
							set_win6170(51);
							display_del_failed();
							sys_delay_us(1800000);
							goto LOOP_MAIN;
						}
						// 删除操作
						nfc_delete(temp[press_count], permision);
						goto LOOP_MAIN;
					}
					else if(get_cur_number.number==10)
					{
						return LOCK_OK;
					}
			}
			sys_delay_us(10000);
		}while(--delay_count);
		
		return LOCK_OUTTIME;
	}else{
		set_win6170(34);
		display_card_null();
		sys_delay_us(800000);
		return LOCK_OK;
	}
}

uint8_t sys_set_menu(void)
{
	uint32_t delay_count;
	uint8_t reValue;
START:
	display_submenu_sys();
	send_bytes(KEY_MAIN_MENU);
	APT_KeyEnableSel(KDR0_CH_1_2_3_4_10_11_EN,KDR1_CH_1_2_3_4_10_11_EN);
RECALL:
	delay_count=1300;

	voice_repeat(7);
	
	do{
		if(get_cur_number.key_prees==1)
		{
			delay_count=1300;
			get_cur_number.key_prees=0;
			switch(get_cur_number.number){
				case 1:
					//音量设置
					if(vol_set()==LOCK_OK)  
						goto START;
					else
						return LOCK_OUTTIME;
				case 2:
				// 联网设置
					if(sys_set_config_wlan()==LOCK_OK)
							goto START;
					else
						return LOCK_OUTTIME;
				case 3:
				// 日期设置
				if(sys_set_date_menu()==LOCK_OK)
					goto START;
				else
						return LOCK_OUTTIME;
				case 4:
				// 恢复出厂设置
					reValue = sys_set_factory_menu();
					if(reValue == LOCK_OK)
					{
						goto START;
					}else{
						return LOCK_OUTTIME;
					}
					
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

uint8_t sys_set_factory_menu(void)
{
	uint16_t delay_count;
	
	display_whether_factory();
RECALL:

	voice_repeat(37);

	delay_count=1300;
	send_bytes(KEY_CONFIG_WLAN);
	APT_KeyEnableSel(KDR0_CH_1_10_11_EN,KDR1_CH_1_10_11_EN);
	
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			delay_count=1300;
			switch(get_cur_number.number)
			{
				case 1:
					factory_reset();
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

uint8_t sys_set_date_menu(void)
{
	int8_t fb_count=0;
	
	uint32_t delay_count;
	
	send_bytes(KEY_ALL_ON);
	
	APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
	HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
	display_date_set();
	oled_refresh_gram_with_multi_arg(3,fb_count*8+24+16,fb_count*8+24+16+8,1);
	delay_count=100000;
	set_win6170(36);
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			if(get_cur_number.number==10)
			{
				delay_count=100000;
				fb_count--;
				//debug_printf("*************fb_count:%d   \r\n ",fb_count);
				if(fb_count<0)
					return LOCK_OK;
				if(fb_count==2||fb_count==5)
				{
					fb_count--;
				}
				if(fb_count<8)
				{
					if(fb_count==7)
					{
						oled_refresh_gram_with_multi_arg(5,(fb_count+1)*8+32,(fb_count+1)*8+32+8,0);
						if(sdatestructure.Date/10==3)
						{
							send_bytes(KEY_0_1_10_11_ON);
							APT_KeyEnableSel(KDR0_CH_0_1_10_11_EN,KDR1_CH_0_1_10_11_EN);
						}
						else{
							send_bytes(KEY_ALL_ON);
							APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
						}
					}
					oled_refresh_gram_with_multi_arg(3,fb_count*8+24+16,fb_count*8+24+16+8,1);
					
					switch(fb_count){
						case 1:
							send_bytes(KEY_ALL_ON);
							APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
							break;
						case 3:
							send_bytes(KEY_0_1_10_11_ON);
							APT_KeyEnableSel(KDR0_CH_0_1_10_11_EN,KDR1_CH_0_1_10_11_EN);
							break;
						case 4:
							if(sdatestructure.Month/10==1)
							{
								send_bytes(KEY_0_1_2_10_11_ON);
								APT_KeyEnableSel(KDR0_CH_0_1_2_10_11_EN,KDR1_CH_0_1_2_10_11_EN);
							}
						else{
							send_bytes(KEY_ALL_ON);
							APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
						}
							break;
						case 6:
							send_bytes(KEY_0_1_2_3_10_11_ON);
							APT_KeyEnableSel(KDR0_CH_0_1_2_3_10_11_EN,KDR1_CH_0_1_2_3_10_11_EN);
							break;
						default:
							break;
						
					}
				}else{
					if(fb_count==10||fb_count==13)
						fb_count--;
					oled_refresh_gram_with_multi_arg(5,(fb_count-8)*8+32,(fb_count-8)*8+32+8,1);
					switch(fb_count){
						case 8:
							send_bytes(KEY_0_1_2_10_11_ON);
							APT_KeyEnableSel(KDR0_CH_0_1_2_10_11_EN,KDR1_CH_0_1_2_10_11_EN);
						break;
						case 9:
							if(stimestructure.Hours/10==2)
							{
								send_bytes(KEY_0_1_2_3_10_11_ON);
								APT_KeyEnableSel(KDR0_CH_0_1_2_3_10_11_EN,KDR1_CH_0_1_2_3_10_11_EN);
							}
							else{
								send_bytes(KEY_ALL_ON);
								APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
							}
							break;
						case 11:
							send_bytes(KEY_0_1_2_3_4_5_10_11_ON);
							APT_KeyEnableSel(KDR0_CH_0_1_2_3_4_5_10_11_EN,KDR1_CH_0_1_2_3_4_5_10_11_EN);
							break;
						case 12:
							send_bytes(KEY_ALL_ON);
							APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
							break;
						case 14:
							send_bytes(KEY_0_1_2_3_4_5_10_11_ON);
								APT_KeyEnableSel(KDR0_CH_0_1_2_3_4_5_10_11_EN,KDR1_CH_0_1_2_3_4_5_10_11_EN);
							break;
						case 15:
							send_bytes(KEY_ALL_ON);
							APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
							break;
						default:
							break;
					}
				}
			}
			else if(get_cur_number.number==11)
			{
				delay_count=100000;
				fb_count++;
				//debug_printf("#####################fb_count:%d   \r\n ",fb_count);
				if(fb_count>15)
				{
					set_win6170(42);
					display_set_sucess();
					sys_delay_us(1200000);
			
					debug_printf("%d-%d-%d \r\n ",sdatestructure.Year,sdatestructure.Month,sdatestructure.Date);	
					debug_printf("%d:%d:%d \r\n ",stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);
					
					HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
					HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
					return LOCK_OK;
				}
				if(fb_count==2||fb_count==5)
					fb_count++;
				if(fb_count<8){
					oled_refresh_gram_with_multi_arg(3,fb_count*8+24+16,fb_count*8+24+16+8,1);
					
					switch(fb_count){
						case 1:
							send_bytes(KEY_ALL_ON);
							APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
							break;
						case 3:
							send_bytes(KEY_0_1_10_11_ON);
							APT_KeyEnableSel(KDR0_CH_0_1_10_11_EN,KDR1_CH_0_1_10_11_EN);
							break;
						case 4:
							if(sdatestructure.Month/10==1)
							{
								send_bytes(KEY_0_1_2_10_11_ON);
								APT_KeyEnableSel(KDR0_CH_0_1_2_10_11_EN,KDR1_CH_0_1_2_10_11_EN);
							}
							else
							{
								send_bytes(KEY_ALL_ON);
								APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
							}
							break;
						case 6:
							send_bytes(KEY_0_1_2_3_10_11_ON);
							APT_KeyEnableSel(KDR0_CH_0_1_2_3_10_11_EN,KDR1_CH_0_1_2_3_10_11_EN);
							break;
						case 7:
							if(sdatestructure.Date/10==3)
							{
								send_bytes(KEY_0_1_10_11_ON);
								APT_KeyEnableSel(KDR0_CH_0_1_10_11_EN,KDR1_CH_0_1_10_11_EN);
							}
						else{
							send_bytes(KEY_ALL_ON);
							APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
						}
							
							break;
						case 8:
								send_bytes(KEY_0_1_2_10_11_ON);
								APT_KeyEnableSel(KDR0_CH_0_1_2_10_11_EN,KDR1_CH_0_1_2_10_11_EN);
						
							break;
						default:
							break;
					}
				}
				else{
					if(fb_count==10||fb_count==13)
						fb_count++;
					if(fb_count==8)
					{
						oled_refresh_gram_with_multi_arg(3,(fb_count-1)*8+24+16,(fb_count-1)*8+24+16+8,0);
						send_bytes(KEY_0_1_2_10_11_ON);
						APT_KeyEnableSel(KDR0_CH_0_1_2_10_11_EN,KDR1_CH_0_1_2_10_11_EN);
						
					}
					oled_refresh_gram_with_multi_arg(5,(fb_count-8)*8+32,(fb_count-8)*8+32+8,1);
					switch(fb_count){
						case 9:
							if(stimestructure.Hours/10==2)
							{
								send_bytes(KEY_0_1_2_3_10_11_ON);
								APT_KeyEnableSel(KDR0_CH_0_1_2_3_10_11_EN,KDR1_CH_0_1_2_3_10_11_EN);
							}
							else{
								send_bytes(KEY_ALL_ON);
								APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
							}
								
							break;
						case 11:
							send_bytes(KEY_0_1_2_3_4_5_10_11_ON);
							APT_KeyEnableSel(KDR0_CH_0_1_2_3_4_5_10_11_EN,KDR1_CH_0_1_2_3_4_5_10_11_EN);
							break;
						case 12:
							send_bytes(KEY_ALL_ON);
							APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
							break;
						case 14:
							send_bytes(KEY_0_1_2_3_4_5_10_11_ON);
							APT_KeyEnableSel(KDR0_CH_0_1_2_3_4_5_10_11_EN,KDR1_CH_0_1_2_3_4_5_10_11_EN);
							break;
						case 15:
							send_bytes(KEY_ALL_ON);
							APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
							break;
						default:
							break;
					}
				}
			}else{
				delay_count=100000;
				if(fb_count>15)
				{
					continue;
				}
				if(fb_count==2||fb_count==5)
					fb_count++;
				if(fb_count<8)
				{
					//debug_printf("$$$$$$$$$$$$$$$$$ fb_count:%d   number:%d \r\n ",fb_count,get_cur_number.number);	
					if(fb_count==0)
					{
						APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
						sdatestructure.Year=get_cur_number.number*10;
						send_bytes(KEY_ALL_ON);
					}
					else if(fb_count==1){
						APT_KeyEnableSel(KDR0_CH_0_1_10_11_EN,KDR1_CH_0_1_10_11_EN);
						
						sdatestructure.Year= (sdatestructure.Year/10)*10+get_cur_number.number;
						//sdatestructure.Year+=get_cur_number.number;
						send_bytes(KEY_0_1_10_11_ON);
					}
					else if(fb_count==3){
						if(get_cur_number.number>1)
							continue;
						sdatestructure.Month=get_cur_number.number*10;
						if(get_cur_number.number==1)
						{
								send_bytes(KEY_0_1_2_10_11_ON);
								APT_KeyEnableSel(KDR0_CH_0_1_2_10_11_EN,KDR1_CH_0_1_2_10_11_EN);
						}
						else{
							send_bytes(KEY_ALL_ON);
							APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
						}
					}
					else if(fb_count==4){
						if(sdatestructure.Month/10==0&&get_cur_number.number==0)
						{
							continue;
						}
						else if(sdatestructure.Month/10==1&&get_cur_number.number>2)
							continue;
						send_bytes(KEY_0_1_2_3_10_11_ON);
						APT_KeyEnableSel(KDR0_CH_0_1_2_3_10_11_EN,KDR1_CH_0_1_2_3_10_11_EN);
						sdatestructure.Month = (sdatestructure.Month)/10*10+get_cur_number.number;
						//sdatestructure.Month+=get_cur_number.number;
					}
					else if(fb_count==6){
						if(get_cur_number.number>3)
							continue;
						sdatestructure.Date=get_cur_number.number*10;
						if(get_cur_number.number==3)
						{
							APT_KeyEnableSel(KDR0_CH_0_1_10_11_EN,KDR1_CH_0_1_10_11_EN);
							send_bytes(KEY_0_1_10_11_ON);
						}
						else
						{
						  APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
							send_bytes(KEY_ALL_ON);
						}
					}
					else if(fb_count==7){
						if(sdatestructure.Date/10==0&&get_cur_number.number==0)
							continue;
					  APT_KeyEnableSel(KDR0_CH_0_1_2_10_11_EN,KDR1_CH_0_1_2_10_11_EN);
						
						sdatestructure.Date =(sdatestructure.Date)/10*10+get_cur_number.number;
						//sdatestructure.Date+=get_cur_number.number;
						send_bytes(KEY_0_1_2_10_11_ON);						
					}
					
					display_date_update(3,fb_count*8+24+16, fb_count*8+24+16+8,get_cur_number.number);
					if(fb_count+1==2||fb_count+1==5)
					{
						
						oled_refresh_gram_with_multi_arg(3,(fb_count+2)*8+24+16,(fb_count+2)*8+24+16+8,1);
					}
					else{
						if(fb_count==7)
						{
							oled_refresh_gram_with_multi_arg(3,(fb_count+1)*8+24+16,(fb_count+1)*8+24+16+8,0);
							oled_refresh_gram_with_multi_arg(5,32,32+8,1);
						}
						else
							oled_refresh_gram_with_multi_arg(3,(fb_count+1)*8+24+16,(fb_count+1)*8+24+16+8,1);
					}
						
				}else{
					if(fb_count==10||fb_count==13)
						fb_count++;
					if(fb_count==8)
					{
						if(get_cur_number.number>2)
							continue;
						stimestructure.Hours=get_cur_number.number*10;
						if(get_cur_number.number==2)
						{
							APT_KeyEnableSel(KDR0_CH_0_1_2_3_10_11_EN,KDR1_CH_0_1_2_3_10_11_EN);
							send_bytes(KEY_0_1_2_3_10_11_ON);
						}
						else{
							APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
							send_bytes(KEY_ALL_ON);
						}
					}
					else if(fb_count==9)
					{
						if(stimestructure.Hours==2&&get_cur_number.number>4)
							continue;
						APT_KeyEnableSel(KDR0_CH_0_1_2_3_4_5_10_11_EN,KDR1_CH_0_1_2_3_4_5_10_11_EN);
						
						stimestructure.Hours =(stimestructure.Hours)/10*10+get_cur_number.number;
						//stimestructure.Hours+=get_cur_number.number;
						send_bytes(KEY_0_1_2_3_4_5_10_11_ON);
					}
					else if(fb_count==11)
					{
						if(get_cur_number.number>5)
							continue;
					  APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
						stimestructure.Minutes=get_cur_number.number*10;
						
						send_bytes(KEY_ALL_ON);
					}
					else if(fb_count==12)
					{
						APT_KeyEnableSel(KDR0_CH_0_1_2_3_4_5_10_11_EN,KDR1_CH_0_1_2_3_4_5_10_11_EN);
						stimestructure.Minutes = (stimestructure.Minutes)/10*10+get_cur_number.number;
						//stimestructure.Minutes+=get_cur_number.number;
						send_bytes(KEY_0_1_2_3_4_5_10_11_ON);
					}
					else if(fb_count==14)
					{
						if(get_cur_number.number>5)
							continue;
						APT_KeyEnableSel(KDR0_ALL_EN,KDR1_ALL_EN);
						stimestructure.Seconds=get_cur_number.number*10;
						send_bytes(KEY_ALL_ON);
					}
					else if(fb_count==15)
					{
						stimestructure.Seconds =(stimestructure.Seconds)/10*10+get_cur_number.number;
						//stimestructure.Seconds+=get_cur_number.number;
						send_bytes(KEY10_11_ON);
					}
					display_date_update(5,(fb_count-8)*8+32, (fb_count-8)*8+32+8,get_cur_number.number);
					if(fb_count+1==10||fb_count+1==13)
						oled_refresh_gram_with_multi_arg(5,(fb_count-8+2)*8+32,(fb_count-8+2)*8+32+8,1);
					else{
						if(fb_count==15)
							oled_refresh_gram_with_multi_arg(5,(fb_count-8+1)*8+32,(fb_count-8+1)*8+32+8,0);
						else
							oled_refresh_gram_with_multi_arg(5,(fb_count-8+1)*8+32,(fb_count-8+1)*8+32+8,1);
					}
				}
				fb_count++;
			}
		}
		sys_delay_us(100);
	}while(--delay_count);
	return LOCK_OUTTIME;
}

static void cloud_status_upload(uint8_t cloudStatus )
{
	int8_t Revalue=-1;
	char lock_time[32];
	
	stm_flash_read(ADDR_FLASH_PAGE_70,(uint8_t*)wifi_flash,WIFI_SIZE);
	
	Revalue =wifi_flash->index;
	wifi_flash[Revalue].exist=1;

	HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
	sprintf(lock_time,"20%02d-%02d-%02d %02d:%02d:%02d",sdatestructure.Year,sdatestructure.Month,
																				sdatestructure.Date,stimestructure.Hours,stimestructure.Minutes,stimestructure.Seconds);
	sprintf(wifi_flash[Revalue].upload_info,"%d%%%d%%%s%%",CLOUD_UPLOAD,cloudStatus,lock_time);
	
	if(wifi_flash->index<99)
	{
		Revalue++;
		wifi_flash->index++;
	}
	stm_flash_erase_write(ADDR_FLASH_PAGE_70,(uint64_t*)wifi_flash,WIFI_SIZE);
}

uint8_t sys_set_config_wlan(void)
{
	uint16_t delay_count;

START:
	display_sys_config_wlan();
	send_bytes(KEY_ADD_MENU);
	APT_KeyEnableSel(KDR0_CH_1_2_3_10_11_EN,KDR1_CH_1_2_3_10_11_EN);
RECALL:
	delay_count=1300;

	voice_repeat(11);
	
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			delay_count=1300;
			switch(get_cur_number.number)
			{
				case 1:
				// 开始配网
				sys_set_config_wlan_countdown();
				goto START;
				case 2:
					// 启动云
					stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
					if(get_cur_info->wifi_button!=1)
						get_cur_info->wifi_button=1;
					
					stm_flash_erase_write(ADDR_FLASH_PAGE_100,(uint64_t*)get_cur_info,CUR_INDEX_SIZE);
					sys_delay_us(1200000);
					set_win6170(42);
					
					cloud_status_upload(get_cur_info->wifi_button);
					goto START;
					//return LOCK_OK;
					//break;
				case 3:
					// 关闭云
					stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
					if(get_cur_info->wifi_button!=0)
						get_cur_info->wifi_button=0;
					stm_flash_erase_write(ADDR_FLASH_PAGE_100,(uint64_t*)get_cur_info,CUR_INDEX_SIZE);
					sys_delay_us(1200000);
					set_win6170(42);
					
					cloud_status_upload(get_cur_info->wifi_button);
					goto START;
					//return LOCK_OK;
					//break;
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

uint8_t sys_set_config_wlan_countdown(void)
{
	int8_t Revalue=-1;
	uint8_t server_t[64]="Tue Apr  9 14:32:01 2019";
	uint8_t upload_info[256]={0};
	
	HAL_StatusTypeDef reValue;
	uint16_t locLen;
	char send_command[32];
	
	send_bytes(KEY10_ON);
	APT_KeyEnableSel(KDR0_CH_10_EN,KDR1_CH_10_EN);
	
	Revalue = wifi_smart_config((uint8_t*)AT_CWSTARTSMART,sizeof(AT_CWSTARTSMART));
	if(Revalue==HAL_OK)
	{
		stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
		get_cur_info->wifi_button=1;
		stm_flash_erase_write(ADDR_FLASH_PAGE_100,(uint64_t*)get_cur_info,CUR_INDEX_SIZE);
		
		if(strncmp((char*)wifi_sys_info.wifi_mac,"FFFFFFFFFFFF",12)==NULL)
				wifi_get_mac(wifi_sys_info.wifi_mac); 
		
		//上报配网 %mac%标识%操作码%时间%#
		sprintf((char*)upload_info,"%%%s%%%d%%xx%%xx%%#",wifi_sys_info.wifi_mac,SERVER_TIME_UPLOAD);
		debug_printf(" upload_info:%s \r\n",upload_info);
		
		/*
		// ssl 设置
		HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*)AT_CIPSSLSIZE,sizeof(AT_CIPSSLSIZE),OK_KEYWORDS,NULL,30));
		HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*)AT_CIPSSLCCONF,sizeof(AT_CIPSSLCCONF),OK_KEYWORDS,NULL,30));
		
		//HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*)AT_TCP_SERVER,sizeof(AT_TCP_SERVER),SERVER_KEYWORDS,NULL,30));
    Revalue = wifi_trans_recev_command((uint8_t*)AT_TCP_SERVER,sizeof(AT_TCP_SERVER),SERVER_KEYWORDS,NULL,50);
		if(Revalue!=HAL_OK)
		{
			// 网络连接失败
			set_win6170(45);
			display_sys_config_fail();
			sys_delay_us(1000000);
			return LOCK_ERROR;
		}
		
		*/
		// 启动透传模式
		locLen = strlen((char*)upload_info);
		
	  sprintf(send_command,"AT+CIPSEND=%d\r\n",locLen);
	 
		HAL_WIFI_STATUE_CHECK(HAL_OK,wifi_trans_recev_command((uint8_t*)send_command,sizeof(send_command),OK_KEYWORDS,NULL,30));
		
		// 获取服务器时间
		wifi_trans_recev_command(upload_info,strlen((char*)upload_info),TIME_KEYWORDS,server_t,30);
		
		reValue = usart3_search_item(DEVICE_APP_KEYWORDS,NULL,50);
		if(reValue==HAL_OK)
		{
			// 网络连接成功
			set_win6170(44);
			display_sys_config_sucessful();
			sys_delay_us(1500000);
		}else{
			// 网络连接失败
			set_win6170(45);
			display_sys_config_fail();
			sys_delay_us(1000000);
		}
		
		// 关闭透传模式
		HAL_WIFI_STATUE_CHECK(HAL_OK,	wifi_trans_recev_command((uint8_t *)AT_END,sizeof(AT_END),END_KEYWORDS,NULL,30));

		debug_printf(" server_t:%s \r\n",server_t);
		// 解析时间格式
		wifi_parase_time((char*)server_t);
		powerOffFlag=1;
		return LOCK_OK;

	}else{
		
		// 网络连接失败
		set_win6170(45);
		display_sys_config_fail();
		sys_delay_us(1000000);
	
	}
	return LOCK_ERROR;
}

uint8_t sys_info_menu(void)
{
	uint16_t delay_count;
	
START:
	display_sys_info_menu();
	send_bytes(KEY_MAIN_MENU);
	APT_KeyEnableSel(KDR0_CH_1_2_3_4_10_11_EN,KDR1_CH_1_2_3_4_10_11_EN);
RECALL:
	delay_count=1300;

	voice_repeat(8);
	
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			delay_count=1300;
			switch(get_cur_number.number){
				case 1:
					//操作日志
					if(sys_operator_log()==LOCK_OK)  
						goto START;
					else
						return LOCK_OUTTIME;
				case 2:
					// 用量信息
					if(sys_resource_used_log()==LOCK_OK)  
							goto START;
						else
							return LOCK_OUTTIME;
				case 3:
					// 电池电量
					if(sys_battery_used_log()==LOCK_OK)
						goto START;
					else
							return LOCK_OUTTIME;
				case 4:
					// 关于本机
					if(sys_host_about_log()==LOCK_OK)
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

uint8_t sys_operator_log(void)
{
	uint16_t delay_count;
	int16_t press_count=0;
	
	
	set_win6170(39);
	send_bytes(KEY_LOG_RECORD);
	
	APT_KeyEnableSel(KDR0_CH_2_8_10_EN,KDR1_CH_2_8_10_EN);

	stm_flash_read(ADDR_FLASH_PAGE_102,(uint8_t*)log_flash,LOG_SIZE);
	stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
	
	debug_printf("\r\nlog_num: %d\r\n",get_cur_info->cur_log_number);
	if(get_cur_info->cur_log_number>0)
	{
		debug_printf("\r\nlog_num: %d\r\n",get_cur_info->cur_log_number);
	}else{
		// 日志为空
		display_sys_log_null();
		sys_delay_us(500000);
		return LOCK_OK;
	}
	
	press_count=get_cur_info->cur_log_number;
	All_Screen();
	display_sys_log_menu(press_count);
	delay_count=8000;
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			delay_count=8000;
			if(get_cur_number.number==2)
			{
				// 向上翻
				press_count--;
				if(press_count<1)
					press_count=get_cur_info->cur_log_number;
				display_sys_log_menu(press_count);
			}
			else if(get_cur_number.number==8)
			{
				// 向下翻页
				press_count++;
				if(press_count>get_cur_info->cur_log_number)
					press_count=1;
				display_sys_log_menu(press_count);
			}
			else if(get_cur_number.number==10)
			{
				return LOCK_OK;
			}
		}
		sys_delay_us(1000);
	}while(--delay_count);
	return LOCK_OUTTIME;
}

uint8_t sys_resource_used_log(void)
{
	uint16_t delay_count;
	
	send_bytes(~KEY10_LED21_OFF);
	APT_KeyEnableSel(KDR0_CH_10_EN,KDR1_CH_10_EN);

	stm_flash_read(ADDR_FLASH_PAGE_100,(uint8_t*)get_cur_info,CUR_INDEX_SIZE);
	
	display_sys_source_info(get_cur_info->cur_fingerprint_index,get_cur_info->cur_keyboard_index,get_cur_info->cur_card_index);
	delay_count=50000;
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			switch(get_cur_number.number)
			{
				case 10:
					return LOCK_OK;
				default:
					break;
			}
		}
		sys_delay_us(100);
	}while(--delay_count);
	return LOCK_OUTTIME;
}
uint8_t sys_battery_used_log(void)
{
	uint16_t delay_count;
	send_bytes(~KEY10_LED21_OFF);
	APT_KeyEnableSel(KDR0_CH_10_EN,KDR1_CH_10_EN);
	
	display_sys_battery_info();
	delay_count=50000;
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			switch(get_cur_number.number)
			{
				case 10:
					return LOCK_OK;
				default:
					break;
			}
		}
		sys_delay_us(100);
	}while(--delay_count);
	return LOCK_OUTTIME;
}

uint8_t sys_host_about_log(void)
{
	uint16_t delay_count;
	
	send_bytes(~KEY10_LED21_OFF);
	APT_KeyEnableSel(KDR0_CH_10_EN,KDR1_CH_10_EN);
	
	display_sys_host_info();
	delay_count=50000;
	do{
		if(get_cur_number.key_prees==1)
		{
			get_cur_number.key_prees=0;
			switch(get_cur_number.number)
			{
				case 10:
					return LOCK_OK;
				default:
					break;
			}
		}
		sys_delay_us(100);
	}while(--delay_count);
	return LOCK_OUTTIME;
}
void sys_log_record(uint8_t index,uint8_t state,uint8_t permision,uint8_t type,uint8_t cur_index)
{
	log_flash[cur_index].index=index;
	log_flash[cur_index].state=(OPERATOR_STATE)state;
	log_flash[cur_index].permision=(PERMISION)permision;
	log_flash[cur_index].type=(USER_TYPE)type;
	log_flash[cur_index].date.Year=sdatestructure.Year;
	log_flash[cur_index].date.Month=sdatestructure.Month;
	log_flash[cur_index].date.Date=sdatestructure.Date;
	log_flash[cur_index].time.Hours=stimestructure.Hours;
	log_flash[cur_index].time.Minutes=stimestructure.Minutes;
	log_flash[cur_index].time.Seconds=stimestructure.Seconds;
	
	stm_flash_erase_write(ADDR_FLASH_PAGE_102,(uint64_t*)log_flash,LOG_SIZE);
}
