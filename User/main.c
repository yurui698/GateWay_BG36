#include "Stm32_Configuration.h"

int main(void)
{	
	SYS_Confgiuration();
	softspi = 0;	 		 //	  �л�ΪӲ��SPI
	SPI1_Configuration();	 //	  ��SPI���½�������	
	while(1)
	{
		SysClock();
	}				   	 
}
