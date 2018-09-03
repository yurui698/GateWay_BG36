#include "DataFlash.h"
#include "string.h"
static u8 tmp_flash[2048];

uint16_t Flash_Write_Without_check(uint32_t iAddress, uint8_t *buf, uint16_t iNumByteToWrite) 
{
    uint16_t i;
    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
    i = 0;
    
//    FLASH_UnlockBank1();
    while((i < iNumByteToWrite) && (FLASHStatus == FLASH_COMPLETE))
    {
		FLASHStatus = FLASH_ProgramHalfWord(iAddress, *(uint16_t*)buf);
		i = i+2;
		iAddress = iAddress + 2;
		buf = buf + 2;
    }
    
    return iNumByteToWrite;
}



/**
  * @brief  Programs a half word at a specified Option Byte Data address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  buf: specifies the data to be programmed.
  * @param  iNbrToWrite: the number to write into flash
  * @retval if success return the number to write, -1 if error
  *  
  */
int Flash_Write(uint32_t iAddress, uint8_t *buf, uint32_t iNbrToWrite) 
{
	/* Unlock the Flash Bank1 Program Erase controller */
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;  
	uint32_t i = 0;
	uint32_t temp=0;
//	uint8_t  tmp[2048];
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;	


	FLASH_UnlockBank1();
	secpos=iAddress & (~(FLASH_PAGE_SIZE -1 )) ;//������ַ 
	secoff=iAddress & (FLASH_PAGE_SIZE -1);     //�������ڵ�ƫ��
	secremain=FLASH_PAGE_SIZE-secoff;           //����ʣ��ռ��С 

	if(iNbrToWrite<=secremain) 
		temp=1;
	else                                                             //ʣ��ռ�С����������
	{
		i=iNbrToWrite-secremain;                                  //�жϻ�ռ�˼�������
		if(i%FLASH_PAGE_SIZE==0)
			temp=i/FLASH_PAGE_SIZE+1;
		else
			temp=i/FLASH_PAGE_SIZE+2;
	}


	for(i=0;i<temp;i++)
		FLASHStatus = FLASH_ErasePage(secpos+i*FLASH_PAGE_SIZE); //��������
	
	if(FLASHStatus != FLASH_COMPLETE)
	  	return -1;
	
	memcpy(tmp_flash,buf,iNbrToWrite);

	if(iNbrToWrite%2)
		temp=iNbrToWrite+1;
	else
		temp=iNbrToWrite;


	Flash_Write_Without_check(iAddress,tmp_flash,temp);

	FLASH_LockBank1();
	return iNbrToWrite; 
}






/**
  * @brief  Programs a half word at a specified Option Byte Data address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  buf: specifies the data to be programmed.
  * @param  iNbrToWrite: the number to read from flash
  * @retval if success return the number to write, without error
  *  
  */
int Flash_Read(uint32_t iAddress, uint8_t *buf, int32_t iNbrToRead) 
{
    int i = 0;
    while(i < iNbrToRead ) 
	{
       *(buf + i) = *(__IO uint8_t*) iAddress++;
       i++;
    }
    return i;
}
