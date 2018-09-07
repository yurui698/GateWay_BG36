#include "Stm32_Configuration.h"
#include "string.h"
#include "DataFlash.h"

ErrorStatus HSEStartUpStatus;

extern u8 SOFTSPI;//�ɼ�
extern u8 RecDataBuffer1[];
extern u8 USART1SendTCB[];
extern u8 RecDataBuffer2[];
extern u8 USART2SendTCB[];
extern u8 RecDataBuffer3[];
extern u8 USART3SendTCB[];
extern u8 RecDataBuffer4[];
extern u8 UART4SendTCB[];
__IO uint16_t ADC_ConvertedValue[2]={0,0};//�ɼ�
extern u8 factory_gateway_set[];
#define FALSE   0
#define TRUE    1

void RCC_Configuration(void)
{   
  	/* RCC system reset(for debug purpose) */
  	RCC_DeInit();
  	/* Enable HSE */
  	RCC_HSEConfig(RCC_HSE_ON);
  	/* Wait till HSE is ready */
  	HSEStartUpStatus = RCC_WaitForHSEStartUp();

  	if(HSEStartUpStatus == SUCCESS)
  	{
    	/* HCLK = SYSCLK */
    	RCC_HCLKConfig(RCC_SYSCLK_Div1);   
    	/* PCLK2 = HCLK */
    	RCC_PCLK2Config(RCC_HCLK_Div1); 
    	/* PCLK1 = HCLK/2 */
    	RCC_PCLK1Config(RCC_HCLK_Div2);
    	/* Flash 2 wait state */
    	FLASH_SetLatency(FLASH_Latency_2);
    	/* Enable Prefetch Buffer */
    	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    	/* PLLCLK = 8MHz * 9 = 72 MHz */
    	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    	/* Enable PLL */ 
    	RCC_PLLCmd(ENABLE);

    	/* Wait till PLL is ready */
    	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    	{
    	}

    	/* Select PLL as system clock source */
    	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    	/* Wait till PLL is used as system clock source */
    	while(RCC_GetSYSCLKSource() != 0x08)
    	{
    	}
  	} 
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//�ɼ�	
}

/*   STM32���������ģʽ:
1.��ͨ���������GPIO_Mode_Out_PP��
2.��ͨ��©�����GPIO_Mode_Out_OD��
3.�������������GPIO_Mode_AF_PP��:�������ڵ����
4.���ÿ�©�����GPIO_Mode_AF_OD��������IIC*/

void GPIO_Configuration(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;

  	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
							RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
							RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE  );

	/*USART2��USART3 Alternate Function mapping*/
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE); //USART2��pin����ӳ�䣨Remap��������Ψһ�ģ������������ GPIO_Remap_USART2
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);//USART3��pinȫ������ӳ�䣨FullRemap��������Ψһ�ģ������������

	/* ����PE0Ϊ�������,��������ͨ������ָʾ����С��ͨѶָʾ����������1ͨѶָʾ�� */
  	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_0;                           
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;				     
  	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		             
  	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_Pin_0);  //˵��Px�˿ڣ��磺GPIOEΪPE�˿�
	
	/* ����PB6Ϊ�������,��������ͨ������ָʾ����M433HZС��ͨѶָʾ������SPI1ͨѶָʾ�� */
  	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_6;                           
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;				     
  	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		             
  	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_0);  //˵��Px�˿ڣ��磺GPIOBΪPB�˿�	

	/*����PD14Ϊ������������ڿ���CDMA�Ŀ��ػ�*/
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_14;                           
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;				     
  	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		             
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD, GPIO_Pin_14);	 

	/*����PD13Ϊ������������ڿ���CDMA������*/
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_13;                           
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;				     
  	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		             
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD, GPIO_Pin_13);	 

	/*����PD15Ϊ������������ڿ���CDMAע�����ߣ��������ݲ�ʹ��*/
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_15;                           
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;				     
  	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		             
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD, GPIO_Pin_15);	 

	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	  GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_9;			         
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_AF_PP;		        
  	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				                 
	/* Configure USART1 Rx (PA.10) as input floating */
  	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_10;			         
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_IN_FLOATING;	         
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	/* Configure USART2 Tx (PD.05) as alternate function push-pull */
  	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_5;			         
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_AF_PP;		        
  	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
  	GPIO_Init(GPIOD, &GPIO_InitStructure);				                 
  	/* Configure USART2 Rx (PD.06) as input floating */
  	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_6;			         
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_IN_FLOATING;	         
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* ����PD4Ϊ�����������,Ϊ����2��RTS�� */
  	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_4;                           
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_AF_PP;				   
  	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		             
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
	  GPIO_ResetBits(GPIOD, GPIO_Pin_4);

	/*PD3Ϊ����2��CTS��*/
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_3;			         
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_IN_FLOATING;	         
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
 
  	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_7;                           
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_AF_PP;				   
  	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		             
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD, GPIO_Pin_7);

	/* Configure USART3 Tx (PD.08) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_8;			         
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_AF_PP;		        
  	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
  	GPIO_Init(GPIOD, &GPIO_InitStructure);				                 
	/* Configure USART3 Rx (PD.09) as input floating */
  	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_9;			         
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_IN_FLOATING;	         
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
	/*485�շ��л�*/
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_10;				        
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;		          
	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
		/* Configure UART4 Tx (PC.10) as alternate function push-pull */
	  GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_10;			         
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_AF_PP;	//����GPS��Ҫ�޸�	        
  	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
  	GPIO_Init(GPIOC, &GPIO_InitStructure);				                 
	/* Configure UART4 Rx (PC.11) as input floating */
  	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_11;			         
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_IN_FLOATING;	         
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
		/*485�շ��л�*/
		GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_9;				        
		GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;		          
		GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		
		/* Configure UART5 Tx (PC.12) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_12;			         
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_AF_PP;		        
  	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
  	GPIO_Init(GPIOC, &GPIO_InitStructure);				                 
	/* Configure UART5 Rx (PD.02) as input floating */
  	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_2;			         
  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_IN_FLOATING;	         
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
	/*485�շ��л�*/
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_8;				        
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;		          
	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
	GPIO_Init(GPIOB, &GPIO_InitStructure);	 
 
/*PB.6��������ָʾ��*/
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_6;				        
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;		          
	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	WX_LED_ON;

	/*SI4463�ŵ�ʶ���뿪�عܽ����� PD.02 PB.03~PB.09*/
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | 
									   GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_IPU;				        		          
	GPIO_Init(GPIOE, &GPIO_InitStructure);


	/*SPI1�ܽ����ã�PA.04--NSS;PA.05--SCK;PA.06--MISO;PA.07--MOSI */
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_4;				        
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;		          
	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	SPI1_NSS_HIGH;

	/*SI4463�ж�����ܽ�nIRQ--PC.04����Դ�ܽ�SDN����PB.07*/
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_4;				        
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_IPU;		          		         
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_7;				        
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;		          
	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	
/*��·����ܽ����� PC1�����¶�DATA2��PC2����ˮ��DATA1;PC0���ն�DATA3;PA0������̼DATA5;PA1������ʪ��DATA4;PC3 ΪSCK;
  ���������PE724V��Դ������� */
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_1 | GPIO_Pin_2;	
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_AIN;	//ģ�������루PC1�������¶ȣ�PC2����ˮ�֣�		        		          
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_3;	//���������PC3ΪSCK
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;			        		          
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_0;	//���������PC0Ϊ����
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_IN_FLOATING;//δ���壬��������
	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;			        		          
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_0 |GPIO_Pin_1;	//�����������PA0������̼��PA1������ʪ�ȣ�
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_IN_FLOATING;//δ���壬��������
	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;			        		          
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_7;	//�����������PE7��������
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;			        		          
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	}

void BH1750_DATA_R(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE  );

	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_0;				        
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_IN_FLOATING;		          		         
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}

void BH1750_DATA_W(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE  );

	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_0;				        
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;		          
	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void BH1750_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE  );

	GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_13;				        
	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;		          
	GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}


void EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure; 

EXTI_ClearITPendingBit(EXTI_Line4);
    /*��EXTI��4���ӵ�PC.4*/
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
    /*����EXTI��4�ϳ����½��أ�������ж�*/
    EXTI_InitStructure.EXTI_Line    =  EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode    =  EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger =  EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd =  ENABLE;
    EXTI_Init(&EXTI_InitStructure);  	
//�ɼ������жϽ���
	
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	 
	NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannel                   = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
	NVIC_InitStructure.NVIC_IRQChannel                   = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
//�ɼ��ж����ȼ�
  NVIC_InitStructure.NVIC_IRQChannel                   = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;   // ����DMAͨ�����ж�����
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // ���ȼ�����
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
		
		 /* Enable the TIM2 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                        = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority      = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority             = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                     = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

		 /* Enable the TIM5 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                        = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority      = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority             = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                     = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
	#ifdef   VECT_TAB_RAM   //���������ram�е�����ô�����ж���������Ram�з�����Flash��
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);

    #else   /* VECT_TAB_FLASH   */
    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   

    #endif
}

void ADC1_Configuration(void)
{	
	DMA_InitTypeDef DMA_InitStructure;
  	ADC_InitTypeDef ADC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE );
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );

	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC��ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue;//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//����
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);	 	

   	/* Time Base configuration */
  	ADC_InitStructure.ADC_Mode               =  ADC_Mode_Independent;
  	ADC_InitStructure.ADC_ScanConvMode       =  ENABLE;
  	ADC_InitStructure.ADC_ContinuousConvMode =  ENABLE;
  	ADC_InitStructure.ADC_ExternalTrigConv   =  ADC_ExternalTrigConv_None;       
  	ADC_InitStructure.ADC_DataAlign          =  ADC_DataAlign_Right;             
  	ADC_InitStructure.ADC_NbrOfChannel       =  2;                            
  	ADC_Init(ADC1, &ADC_InitStructure);
  
 	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 2, ADC_SampleTime_239Cycles5); 

	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1, ENABLE);  	

	ADC_ResetCalibration(ADC1);
  	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
  	while(ADC_GetCalibrationStatus(ADC1)); 
}


//void USART1_Configuration(void)
//{
//	USART_InitTypeDef USART_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE  );
//																	 
//	USART_InitStructure.USART_BaudRate     =  9600;						        // ���س���Ϊ������Ϊ��115200��boot0�ܽ����ߣ��û�����ִ��
//	USART_InitStructure.USART_WordLength   =  USART_WordLength_8b;			    // 8λ����
//	USART_InitStructure.USART_StopBits     =  USART_StopBits_1;				    // ��֡��β����1��ֹͣλ
//	USART_InitStructure.USART_Parity       =  USART_Parity_No ;				    // ��żʧ��
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// Ӳ��������ʧ��

//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		        // ����ʹ��+����ʹ��
//	/* Configure USART1 basic and asynchronous paramters */
//	USART_Init(USART1, &USART_InitStructure);
//    
//	/* Enable USART1 */
//	  USART_ClearFlag(USART1, USART_IT_RXNE); 			                        //���жϣ�����һ�����жϺ����������ж�	
//    USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);		                        //ʹ��USART1�����ж�

//	USART_Cmd(USART1, ENABLE); 
//}

void UART5_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5 , ENABLE  );
	
	USART_InitStructure.USART_BaudRate     =  9600;						        // Ĭ�����ò���
	USART_InitStructure.USART_WordLength   =  USART_WordLength_8b;			    // 8λ���ݣ���У��λΪ9λ
	USART_InitStructure.USART_StopBits     =  USART_StopBits_1;				    // ��֡��β����1��ֹͣλ
	USART_InitStructure.USART_Parity       =  USART_Parity_No ;				    // ��żʧ�ܣ�USART_Parity_No��������ΪżУ��
	if(factory_gateway_set[27]==13)         //�����������ƣ��������
  {		
		USART_InitStructure.USART_BaudRate     =  2400;						        // =13 �����������ƣ�=16 ������������+������Һλ
		USART_InitStructure.USART_WordLength   =  USART_WordLength_9b;			    // 8λ���ݣ���У��λΪ9λ
		USART_InitStructure.USART_StopBits     =  USART_StopBits_1;				    // ��֡��β����1��ֹͣλ
		USART_InitStructure.USART_Parity       =  USART_Parity_Even ;				    // ��żʧ�ܣ�USART_Parity_No��������ΪżУ��
	}
	if(factory_gateway_set[27]==14||factory_gateway_set[27]==16||factory_gateway_set[27]==17)         //����������Ϻ�����ư�������
  {		
		USART_InitStructure.USART_BaudRate     =  2400;						        // =14 ��������ƣ�=17 ���������+������Һλ
		USART_InitStructure.USART_WordLength   =  USART_WordLength_8b;			    // 8λ���ݣ���У��λΪ9λ
		USART_InitStructure.USART_StopBits     =  USART_StopBits_1;				    // ��֡��β����1��ֹͣλ
		USART_InitStructure.USART_Parity       =  USART_Parity_No ;				    // ��żʧ�ܣ�USART_Parity_No��������ΪżУ��
	}
	
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// Ӳ��������ʧ��

	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		        // ����ʹ��+����ʹ��
	/* Configure UART5 basic and asynchronous paramters */
	USART_Init(UART5, &USART_InitStructure);
    
	/* Enable UART5 */
	USART_ClearFlag(UART5, USART_IT_RXNE); 			                        //���жϣ�����һ�����жϺ����������ж�
//  USART_ClearFlag(UART5, USART_IT_TC);                                   //����жϱ�־
	
//   USART_ITConfig(UART5,USART_IT_IDLE, ENABLE); 
//    USART_ITConfig(UART5,USART_FLAG_ORE, ENABLE);                               //ʹ��UART5�����ж� 
	USART_ITConfig(UART5,USART_IT_RXNE, ENABLE);		                        //ʹ��USART5�ж�Դ
	USART_Cmd(UART5, ENABLE); 
}


void IWDG_Configuration(void)
{
    RCC_LSICmd(ENABLE);                              //��LSI
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET); 
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_32);	  //32
    IWDG_SetReload(3000);      				
    IWDG_ReloadCounter();
    IWDG_Enable();//����ʱ�رտ��Ź�
}

void SPI1_Configuration(void)
 {
 	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO, ENABLE);
	if(softspi){
		GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_5 | GPIO_Pin_7;				        
		GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_Out_PP;		          
		GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_6;			         
	  	GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_IN_FLOATING;	         
	  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else
	{
		SPI_Cmd(SPI1, DISABLE);
		GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;				        
		GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_AF_PP;		          
		GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_50MHz;		         
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	  	SPI_InitStructure.SPI_CRCPolynomial = 7;
	  	SPI_Init(SPI1, &SPI_InitStructure);
	  	/* Enable SPI1  */
	
	  	SPI_Cmd(SPI1, ENABLE);
		SPI_SSOutputCmd(SPI1, ENABLE);

	}
	Delayms(100);
  
 }
/*DMA1(7��)��DMA2��5�������壺USART3���գ�δ�ã������жϷ�ʽ���գ���DMAͨ�������ʹ�õ������Ƕ���õģ��������ʹ�ã�
 DMA1_Channel 1��A/Dת��
 DMA1_Channel 2��USART3 ����
 DMA1_Channel 3��δ��
 DMA1_Channel 4��USART1����
 DMA1_Channel 5��USART1����
 DMA1_Channel 6��USART2����
 DMA1_Channel 7��USART2����
 DMA2_Channel 1��δ��
 DMA2_Channel 2��δ��
 DMA2_Channel 3��UART4����
 DMA2_Channel 4��δ��
 DMA2_Channel 5��UART4����
 */
/*--- Usart1 DMA����+DMA���� Config ---------------------------------------*/ 
void Uart1_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
 
    /* System Clocks Configuration */
//= System Clocks Configuration ====================================================================//
   
    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,  ENABLE ); // ������������IO�˿ڵ�ʱ��
    /* Enable USART Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // ��ʼ����ʱ��
   
   
//=NVIC_Configuration==============================================================================//
 
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
 
    /* Enable the DMA Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;   // ����DMAͨ�����ж�����
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // ���ȼ�����
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
 
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;     // �����ж����ã�����
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
   
//=GPIO_Configuration==============================================================================//
 
//    GPIO_PinRemapConfig(GPIO_FullRemap_USART1, ENABLE);  // ������û����Ĭ��IO�ڣ����Խ���������ӳ�䣬������Ը����Լ���Ӳ���������ѡ��
   
    /* Configure USART1 Rx as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // ���ڽ���IO�ڵ�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//USART1--RX�ܽ�
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // ���ڷ���IO�ڵ�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // �������óɸ�����ʽ���������   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//USART1--TX�ܽ�
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
   
 
    /* USART Format configuration ------------------------------------------------------*/
    /* Configure USART1 */
		//����1Ĭ���趨
		USART_InitStructure.USART_BaudRate = 9600;		
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // ���ڸ�ʽ����
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
		if(factory_gateway_set[0]==3)  //������̫���趨
      { 
			  USART_InitStructure.USART_BaudRate = 115200;
			  USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // ���ڸ�ʽ����
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
			} 
		if(factory_gateway_set[27]==15) //����1������Һλ�趨
      { 
			  USART_InitStructure.USART_BaudRate = 9600;
			  USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // ���ڸ�ʽ����
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
			} 
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
    
    USART_Init(USART1, &USART_InitStructure);
 
    /* Enable USART1 Receive and Transmit interrupts */
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  // ���� ���ڿ���IDEL �ж�
//		USART_ClearFlag(USART1, USART_IT_RXNE);             //���жϣ�����һ�����жϺ����������ж�  
//	  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		   
   
    /* Enable the USART1 */
    USART_Cmd(USART1, ENABLE);  // ��������
    /* Enable USARTy DMA TX request */
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);  // ��������DMA����
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); // ��������DMA����

}
 
 
void DMA_Uart1_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
   
    /* DMA clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ����DMA1ʱ��
   
   
//=DMA_Configuration==============================================================================//
 
/*--- ����1 UART_Tx_DMA_Channel DMA Config ʹ��DMA1_Channel4��DMA1_Channel5---*/    
    DMA_Cmd(DMA1_Channel4, DISABLE);                           // ��DMAͨ��
    DMA_DeInit(DMA1_Channel4);                                 // �ָ�ȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);// ���ô��ڷ������ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART1SendTCB;         // ���÷��ͻ������׵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // ��������λĿ�꣬�ڴ滺���� -> ����Ĵ���
    DMA_InitStructure.DMA_BufferSize = 256;                     // ��Ҫ���͵��ֽ�����������ʵ��������Ϊ0����Ϊ��ʵ��Ҫ���͵�ʱ�򣬻��������ô�ֵ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ��DMAģʽ
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);               // д������
    DMA_ClearFlag(DMA1_FLAG_GL4);                                 // ���DMA���б�־��DMA1_FLAG_TC4 | DMA1_FLAG_TE4 
    DMA_Cmd(DMA1_Channel4, DISABLE); // �ر�DMA
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);            // ��������DMAͨ���ж�
   
/*--- DMA1_Channel5 DMA Config ---*/
 
    DMA_Cmd(DMA1_Channel5, DISABLE);                           // ��DMAͨ��������
    DMA_DeInit(DMA1_Channel5);                                 // �ָ�ȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);// ���ô��ڽ������ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RecDataBuffer1;         // ���ý��ջ������׵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      // ��������Ϊ����Դ������Ĵ��� -> �ڴ滺����
    DMA_InitStructure.DMA_BufferSize = 256;                     // ��Ҫ�����ܽ��յ����ֽ���
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ;ѭ��ģʽDMA_Mode_Circular
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ��DMAģʽ
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);               // д������
    DMA_ClearFlag(DMA1_FLAG_GL5);                                 // ���DMA���б�־
    DMA_Cmd(DMA1_Channel5, ENABLE);                            // ��������DMAͨ�����ȴ���������
   
}

//����1--DMA������� 
 
/*--- Usart2 DMA����+DMA���� Config ---------------------------------------*/ 
void Uart2_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
 
    /* System Clocks Configuration */
//= System Clocks Configuration ====================================================================//
   
    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO,  ENABLE ); // ������������IO�˿ڵ�ʱ��
    /* Enable USART Clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // ��ʼ����ʱ��
   
   
//=NVIC_Configuration==============================================================================//
 
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
 
    /* Enable the DMA Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;   // ����DMAͨ�����ж�����
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // ���ȼ�����
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
 
    /* Enable the USART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;     // �����ж����ã�����
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
   
//=GPIO_Configuration==============================================================================//
 
//    GPIO_PinRemapConfig(GPIO_FullRemap_USART2, ENABLE);  // ������û����Ĭ��IO�ڣ����Խ���������ӳ�䣬������Ը����Լ���Ӳ���������ѡ��
   
    /* Configure USART2 Rx as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // ���ڽ���IO�ڵ�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//USART3--RX�ܽ�
    GPIO_Init(GPIOD, &GPIO_InitStructure);
 
    /* Configure USART3 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // ���ڷ���IO�ڵ�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // �������óɸ�����ʽ���������   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//USART3--TX�ܽ�
    GPIO_Init(GPIOD, &GPIO_InitStructure);
 
   
 
    /* USART Format configuration ------------------------------------------------------*/
 
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // ���ڸ�ʽ����
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 
    /* Configure USART2 */
    USART_InitStructure.USART_BaudRate = 115200;  //  ����������
    USART_Init(USART2, &USART_InitStructure);
 
    /* Enable USART3 Receive and Transmit interrupts */
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  // ���� ���ڿ���IDEL �ж�
//		USART_ClearFlag(USART2, USART_IT_RXNE);             //���жϣ�����һ�����жϺ����������ж�  
//	  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);		   
   
    /* Enable the USART2 */
    USART_Cmd(USART2, ENABLE);  // ��������
    /* Enable USARTy DMA TX request */
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);  // ��������DMA����
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); // ��������DMA����

}
 
 
void DMA_Uart2_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
   
    /* DMA clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ����DMA1ʱ��
   
   
//=DMA_Configuration==============================================================================//
 
/*--- ����2 UART_Tx_DMA_Channel DMA Config ʹ��DMA1_Channel7��DMA1_Channel6---*/    
    DMA_Cmd(DMA1_Channel7, DISABLE);                           // ��DMAͨ��
    DMA_DeInit(DMA1_Channel7);                                 // �ָ�ȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);// ���ô��ڷ������ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART2SendTCB;         // ���÷��ͻ������׵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // ��������λĿ�꣬�ڴ滺���� -> ����Ĵ���
    DMA_InitStructure.DMA_BufferSize = 256;                     // ��Ҫ���͵��ֽ�����������ʵ��������Ϊ0����Ϊ��ʵ��Ҫ���͵�ʱ�򣬻��������ô�ֵ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ��DMAģʽ
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);               // д������
    DMA_ClearFlag(DMA1_FLAG_GL7);                                 // ���DMA���б�־��DMA1_FLAG_TC4 | DMA1_FLAG_TE4 
    DMA_Cmd(DMA1_Channel7, DISABLE); // �ر�DMA
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);            // ��������DMAͨ���ж�
   
/*--- DMA1_Channel6 DMA Config ---*/
 
    DMA_Cmd(DMA1_Channel6, DISABLE);                           // ��DMAͨ��������
    DMA_DeInit(DMA1_Channel6);                                 // �ָ�ȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);// ���ô��ڽ������ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RecDataBuffer2;         // ���ý��ջ������׵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      // ��������Ϊ����Դ������Ĵ��� -> �ڴ滺����
    DMA_InitStructure.DMA_BufferSize = 256;                     // ��Ҫ�����ܽ��յ����ֽ���
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ;ѭ��ģʽDMA_Mode_Circular
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ��DMAģʽ
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);               // д������
    DMA_ClearFlag(DMA1_FLAG_GL6);                                 // ���DMA���б�־
    DMA_Cmd(DMA1_Channel6, ENABLE);                            // ��������DMAͨ�����ȴ���������
   
}

//����2--DMA������� 

 
/*--- Usart3 DMA����+�жϽ��� Config ---------------------------------------*/ 
 
void Uart3_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
 
    /* System Clocks Configuration */
//= System Clocks Configuration ====================================================================//
   
    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD ,  ENABLE ); // ������������IO�˿ڵ�ʱ��
    /* Enable USART Clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // ��ʼ����ʱ��
   
   
//=NVIC_Configuration==============================================================================//
 
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
 
    /* Enable the DMA Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;   // ����DMAͨ�����ж�����
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // ���ȼ�����
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
 
    /* Enable the USART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;     // �����ж�����
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
   
//=GPIO_Configuration==============================================================================//
 
//    GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);  // ������û����Ĭ��IO�ڣ����Խ���������ӳ�䣬������Ը����Լ���Ӳ���������ѡ��
   
    /* Configure USART3 Rx as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // ���ڽ���IO�ڵ�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//USART3--RX�ܽ�
    GPIO_Init(GPIOD, &GPIO_InitStructure);
 
    /* Configure USART3 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // ���ڷ���IO�ڵ�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // �������óɸ�����ʽ���������   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//USART3--TX�ܽ�
    GPIO_Init(GPIOD, &GPIO_InitStructure);
 
   
 
    /* USART Format configuration ------------------------------------------------------*/
 
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;    // ���ڸ�ʽ����
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;//USART_Parity_Even  USART_Parity_No
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 
    /* Configure USART3 */
    USART_InitStructure.USART_BaudRate = 9600;  //  ����������
    USART_Init(USART3, &USART_InitStructure);
 
    /* Enable USART3 Receive and Transmit interrupts */
//    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);  // ���� ���ڿ���IDEL �ж�
		USART_ClearFlag(USART3, USART_IT_RXNE);             //���жϣ�����һ�����жϺ����������ж�  
	  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);		   
   
    /* Enable the USART3 */
    USART_Cmd(USART3, ENABLE);  // ��������
    /* Enable USARTy DMA TX request */
    USART_DMACmd(USART3, USART_DMAReq_Tx, DISABLE);  // ��������DMA����
//    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE); // ��������DMA����

}
 
 
void DMA_Uart3_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
   
    /* DMA clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ����DMA1ʱ��
   
   
//=DMA_Configuration==============================================================================//
 
/*--- ����3 UART_Tx_DMA_Channel DMA Config ʹ��DMA1_Channel2---*/    
    DMA_Cmd(DMA1_Channel2, DISABLE);                           // ��DMAͨ��
    DMA_DeInit(DMA1_Channel2);                                 // �ָ�ȱʡֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);// ���ô��ڷ������ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART3SendTCB;         // ���÷��ͻ������׵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // ��������λĿ�꣬�ڴ滺���� -> ����Ĵ���
    DMA_InitStructure.DMA_BufferSize = 128;                     // ��Ҫ���͵��ֽ�����������ʵ��������Ϊ0����Ϊ��ʵ��Ҫ���͵�ʱ�򣬻��������ô�ֵ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ��DMAģʽ
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);               // д������
    DMA_ClearFlag(DMA1_FLAG_GL2);                                 // ���DMA���б�־��DMA1_FLAG_TC2 | DMA1_FLAG_TE2 
    DMA_Cmd(DMA1_Channel2, DISABLE); // �ر�DMA
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);            // ��������DMAͨ���ж�
}

//����3--DMA�������
//����4--DMA���忪ʼ
/*1.ʱ��RCC���ã�����ʱ�� + DMAʱ�� + IOʱ��*/
void Uart4_Init(void)	
{      
	 NVIC_InitTypeDef NVIC_InitStructure;
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
	
       RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);  //����ʱ��       
       RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);  //DMA2ʱ�� 
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC| RCC_APB2Periph_AFIO, ENABLE);//IOʱ��
       
 
/*2.GPIO���ã�
UART4��TXΪPC10�ţ����Ͷ�����Ϊ�����������ģʽ��GPIO_Mode_AF_PP��
UART4��RXΪPC11�ţ����ն�����Ϊ��������ģʽ��GPIO_Mode_IN_FLOATING��*/
                            
       GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10;              
       GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;  //TX�����������ģʽ
       GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
       GPIO_Init(GPIOC,&GPIO_InitStructure);               
 
       GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;             
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //RX��������ģʽ
       GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
       GPIO_Init(GPIOC,&GPIO_InitStructure);

 
/*3.�ж�NVIC���ã���������DMAͨ���жϣ�
UART4��RX��DMAͨ��ΪDMA2��ͨ��3��
UART4��TX��DMAͨ��ΪDMA2��ͨ��5��*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
   
    NVIC_InitStructure.NVIC_IRQChannel =UART4_IRQn;//�����жϣ�����
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority= 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
    NVIC_Init(&NVIC_InitStructure);      

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE; 
    NVIC_Init(&NVIC_InitStructure);

 
/*4.�������ã�����䴮�����ýṹ��*/
     
       USART_InitStructure.USART_BaudRate =115200;
       USART_InitStructure.USART_WordLength =USART_WordLength_8b;//����λ8λ
       USART_InitStructure.USART_StopBits =USART_StopBits_1;//ֹͣλ1λ
       USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
       USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;//������Ӳ������
       USART_InitStructure.USART_Mode =USART_Mode_Rx | USART_Mode_Tx;//TX��RX������ 
       USART_Init(UART4,&USART_InitStructure);
			 USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);  // ���� ���ڿ���IDEL �ж�
       USART_Cmd(UART4, ENABLE); //ʹ��UART4����
			 USART_DMACmd(UART4, USART_DMAReq_Tx,DISABLE);//���ô�����DMA����Tx��������������
			 USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);//���ô�����DMA����Rx��������������
}
 
/*5.DMA���ã�
DMA���԰����ݴ�����ת�Ƶ��ڴ棨�紮�ڽ��յ�ʱ�򣩣�Ҳ���Դ��ڴ�ת�Ƶ����裨�紮�ڷ��͵�ʱ�򣩣�
��ͬ���������ת��Ҫ������Ӧ������*/
 
//���ڷ��ͣ�
void DMA_Uart4_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;   
   
    /* DMA2 Channel5  Config */
    DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)&(UART4->DR);//�������ַ������4���ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr =(u32)UART4SendTCB;//�ڴ����ַ������UART4_DMA_HeadBuf
    DMA_InitStructure.DMA_DIR =DMA_DIR_PeripheralDST; //�ڴ浽DST����
    DMA_InitStructure.DMA_BufferSize =128;//DMA���ݴ��䳤��
    DMA_InitStructure.DMA_PeripheralInc =DMA_PeripheralInc_Disable;//�����ַ������
    DMA_InitStructure.DMA_MemoryInc =DMA_MemoryInc_Enable;//�ڴ��ַ����
    DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_Byte;//�������ݵ�λΪ1�ֽ�
    DMA_InitStructure.DMA_MemoryDataSize =DMA_MemoryDataSize_Byte;//�ڴ����ݵ�λΪ1�ֽ�
    DMA_InitStructure.DMA_Mode =DMA_Mode_Normal;//DMA��������ģʽ������ģʽ����һ��
    DMA_InitStructure.DMA_Priority =DMA_Priority_High;//DMAͨ�����ȼ�
    DMA_InitStructure.DMA_M2M =DMA_M2M_Disable;//��ֹDMA�ڴ浽�ڴ洫��      

    DMA_Init(DMA2_Channel5,&DMA_InitStructure);
	  DMA_ClearFlag(DMA2_FLAG_GL5);     // ���DMA���б�־��DMA2_FLAG_TC5 | DMA1_FLAG_TE5
	  DMA_Cmd(DMA2_Channel5, DISABLE);//��ʽ����DMA
    DMA_ITConfig(DMA2_Channel5, DMA_IT_TC,ENABLE);//����DMA2������ɺ�����ж�
     
//���ڽ��գ�     
    DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)(&UART4->DR);//�������ַ������4���ݼĴ���
    DMA_InitStructure.DMA_MemoryBaseAddr =(uint32_t)RecDataBuffer4; 
    DMA_InitStructure.DMA_DIR =DMA_DIR_PeripheralSRC;//���赽�ڴ�SRC 
    DMA_InitStructure.DMA_BufferSize =128; 
    DMA_InitStructure.DMA_PeripheralInc =DMA_PeripheralInc_Disable; 
    DMA_InitStructure.DMA_MemoryInc =DMA_MemoryInc_Enable; 
    DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_Byte; 
    DMA_InitStructure.DMA_MemoryDataSize =DMA_PeripheralDataSize_Byte; 
    DMA_InitStructure.DMA_Mode =DMA_Mode_Normal; 
    DMA_InitStructure.DMA_Priority =DMA_Priority_High; 
    DMA_InitStructure.DMA_M2M =DMA_M2M_Disable;             

    DMA_Init(DMA2_Channel3,&DMA_InitStructure);          
    DMA_ClearFlag(DMA2_FLAG_GL3);              // ���DMA���б�־
    DMA_Cmd(DMA2_Channel3, ENABLE);//��ʽ����DMA 
		DMA_ITConfig(DMA2_Channel3, DMA_IT_TC,ENABLE);
}
//����4--DMA�������

void TIM2_Configuration(void)//����1s
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	TIM_DeInit(TIM2);

	TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;  //Ƶ��ΪCK_CNT=TIMxCLK/(PSC+1),72M/7200=10000hz ��0.1ms
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
	TIM_TimeBaseStructure.TIM_Period    = 10000-1;  //1/CK_CLK *(ARR+1) ��ʱʱ��Ϊ1s
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	             //�½��ز���
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;					 //ÿ���½��ض�����
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;			 
	TIM_ICInitStructure.TIM_ICFilter = 0x01;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;//PA0��Ӳ���涨�ģ�����
	TIM_ICInit(TIM2,&TIM_ICInitStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_CC1,DISABLE);
	TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE );	
}

void TIM5_Configuration(void)//����1us
{
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5 , ENABLE);
	TIM_DeInit(TIM5);
	
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period    = 0xFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);

	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;	 //�ϡ��½��ز���TIM_ICPolarity_BothEdge
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;					 //ÿ���ϡ��½��ض�����
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;			 
	TIM_ICInitStructure.TIM_ICFilter = 0x00;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;//PA1��Ӳ���涨�ģ�������ʪ��
	TIM_ICInit(TIM5,&TIM_ICInitStructure);	
	
	TIM_ITConfig(TIM5,TIM_IT_CC2,DISABLE);
	TIM_ITConfig(TIM5,TIM_IT_Update,DISABLE ); 
}
 void EXTI1_Enable(Boolean bEnable)//PA1,������ʪ��
{
	NVIC_InitTypeDef NVIC_InitStructure;  
  EXTI_InitTypeDef EXTI_InitStructure;  
    EXTI_ClearITPendingBit(EXTI_Line1);  
  
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = bEnable ? ENABLE : DISABLE;  
    NVIC_Init(&NVIC_InitStructure);
	 
	  EXTI_ClearITPendingBit(EXTI_Line1);
    /*��EXTI��1���ӵ�PA1��������ʪ��������*/
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
    /*����EXTI��1�ϳ����½��أ�������ж�*/
    EXTI_InitStructure.EXTI_Line    =  EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode    =  EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger =  EXTI_Trigger_Falling;//EXTI_Trigger_Rising;EXTI_Trigger_Rising_Falling�������½��ش���
    EXTI_InitStructure.EXTI_LineCmd =  bEnable ? ENABLE : DISABLE;  
    EXTI_Init(&EXTI_InitStructure);  
}

void EXIT0_Enable(Boolean bEnable)
{
	NVIC_InitTypeDef NVIC_InitStructure;  
  EXTI_InitTypeDef EXTI_InitStructure; 
    EXTI_ClearITPendingBit(EXTI_Line0);  
  
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = bEnable ? ENABLE : DISABLE;  
    NVIC_Init(&NVIC_InitStructure);	
	
	  EXTI_ClearITPendingBit(EXTI_Line0);
    /*��EXTI��0���ӵ�PC.0,���ն�������*/
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource0);
    /*����EXTI��0�ϳ����½��أ�������ж�*/
    EXTI_InitStructure.EXTI_Line    =  EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode    =  EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger =  EXTI_Trigger_Falling;//EXTI_Trigger_Falling;EXTI_Trigger_Rising_Falling�������½��ش���
    EXTI_InitStructure.EXTI_LineCmd =  bEnable ? ENABLE : DISABLE;  
    EXTI_Init(&EXTI_InitStructure);	  
}
void wgfrist_init_readflash(void)     //���س�ʼ����flash;flash��0x0800 0000~0x0807 FFFF����512k
{
  u8 init_flash_flg[2]={0,0};
	   Flash_Read(0x0807B000,  init_flash_flg, 2);//�������·����������趨������Ҫд��flash0x0807 C000����256k��ʼд�룬0x0804��1��������64k,ÿ��д�����2Kbyte
	   if(init_flash_flg[0]!=0xFF&&init_flash_flg[1]!=0xFF) //����Ѿ������������ã������������趨������ʹ�ó����ֵ
		 {
			 Flash_Read(0x0807B000,  factory_gateway_set, 221);			
		 }		 
}
void SYS_Confgiuration(void)
{	
	RCC_Configuration();
	GPIO_Configuration();
	EXTI_Configuration(); 		   	 		
	NVIC_Configuration();
  wgfrist_init_readflash();
	UART5_Configuration();
	SPI1_Configuration(); 	
	IWDG_Configuration();
	ADC1_Configuration();//�ɼ����� 
	YX_LED_ON;	 //��������ָʾ��
	WX_LED_ON;	 //��������ָʾ��
	SysTick->LOAD  = 0x00FFFFFF- 1;    
  SysTick->VAL   = 0;
  SysTick->CTRL  = 0x05;
	TimerInit();
  TIM5_Configuration();
	TIM2_Configuration();
	Uart1_Init();
  DMA_Uart1_Init();   // ����1 DMA ����
	Uart2_Init();
  DMA_Uart2_Init();   // ����2 DMA ����
	Uart3_Init();
  DMA_Uart3_Init();   // ����3 DMA ����
	Uart4_Init();
  DMA_Uart4_Init();   // ����4 DMA ����
	GPIO_ResetBits(GPIOD, GPIO_Pin_10);	 //USART3��ʹ��
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);//UART4��ʹ��
	Set_Event(SYS_INIT_EVT);
}
