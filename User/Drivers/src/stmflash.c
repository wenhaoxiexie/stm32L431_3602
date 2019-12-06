#include "stmflash.h"
#include "stdio.h"
#include "stdlib.h"
#include "usart.h"

static uint32_t GetPage(uint32_t Address);
static uint32_t GetBank(uint32_t Address);

// len 对应的是多少个字节
void stm_flash_erase_write(uint32_t start_addr,uint64_t* data,uint32_t len)
{
		uint32_t FirstPage = 0, NbOfPages = 0;
	  uint32_t BankNumber = 0,PAGEError = 0;
		uint32_t Address,end_addr;
		uint64_t* ptr=NULL;
		uint32_t i;
	
		FLASH_EraseInitTypeDef EraseInitStruct;

		ptr=data;
		end_addr=start_addr+len;
		
		HAL_FLASH_Unlock();
		 /* Clear OPTVERR bit set on virgin samples */
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 
		/* Get the 1st page to erase */
		FirstPage = GetPage(start_addr);
		/* Get the number of pages to erase from 1st page */
		NbOfPages = GetPage(end_addr) - FirstPage + 1;
		
		//debug_printf("\r\n FirstPage:%d NbOfPages:%d \r\n",FirstPage,NbOfPages);
		/* Get the bank */
		BankNumber = GetBank(start_addr);
		/* Fill EraseInit structure*/
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.Banks       = BankNumber;
		EraseInitStruct.Page        = FirstPage;
		EraseInitStruct.NbPages     = NbOfPages;
		
		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
		{
				debug_printf("flash erase failed %d\n\r",PAGEError);
		}
		
		Address = start_addr;
		//debug_printf(" len:%d \r\n",len);
	
		// 一次写入8个字节
		for(i=0;i<len;i+=8)
		{
			//debug_printf(" %p:0x%llx\n\r",(void*)Address,*ptr);
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, *ptr++) == HAL_OK)
			{
				Address = Address + 8;
				
			}
		 else
			{
				debug_printf("\n\rflash program failed\n\r");
			}
		}
		
		HAL_FLASH_Lock();
}

void stm_flash_erase(uint32_t start_addr,uint32_t len)
{
		uint32_t FirstPage = 0, NbOfPages = 0;
	  uint32_t BankNumber = 0,PAGEError = 0;
		uint32_t end_addr;
	
		FLASH_EraseInitTypeDef EraseInitStruct;

		end_addr=start_addr+len;
		
		HAL_FLASH_Unlock();
		 /* Clear OPTVERR bit set on virgin samples */
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 
		/* Get the 1st page to erase */
		FirstPage = GetPage(start_addr);
		/* Get the number of pages to erase from 1st page */
		NbOfPages = GetPage(end_addr) - FirstPage + 1;
		
		//debug_printf(" FirstPage:%d NbOfPages:%d \r\n",FirstPage,NbOfPages);
		/* Get the bank */
		BankNumber = GetBank(start_addr);
		/* Fill EraseInit structure*/
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.Banks       = BankNumber;
		EraseInitStruct.Page        = FirstPage;
		EraseInitStruct.NbPages     = NbOfPages;
		
		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
		{
				debug_printf("\n\rflash erase failed %d\n\r",PAGEError);
		}
		HAL_FLASH_Lock();
}

void stm_flash_read(uint32_t start_addr,uint8_t* buf,uint32_t len)
{	
	uint32_t i;
	
	//debug_printf(" len:%d \r\n",len);
	for(i=0;i<len;i++)
	{
		//debug_printf(" %p : 0x%x\r\n",(void*)start_addr,*(__IO uint32_t *)start_addr);
		
		*buf=*(__IO uint8_t *)start_addr;
		
		//debug_printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ %p : 0x%x\r\n",(void*)buf,*buf);
		
		start_addr++;
		buf++;
	}	
}

static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;
  
  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }
  
  return page;
}

static uint32_t GetBank(uint32_t Addr)
{
  return FLASH_BANK_1;
}











