/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x.h"
#include "systemclock.h"
#include "HandleTask.h"
#include "Stm32_Configuration.h"
#include "string.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define fALSE   0
#define tRUE    1

#define RXENABLE3		         GPIO_ResetBits(GPIOD, GPIO_Pin_10)
#define RXENABLE4		         GPIO_ResetBits(GPIOB, GPIO_Pin_9)
#define TXENABLE4		         GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define RXENABLE5		         GPIO_ResetBits(GPIOB, GPIO_Pin_8)

u8 RxFlag2    = 1;
u8 TxFlag2    = 1;
u8 RecLen2    = 1;
u8 TxCount2   = 0;
u8 USART2BufferCNT  =0;
u8 RecDataBuffer2[256];	 //����2�������ϴ�
u8 USART2SendTCB[256];	 //����2�������ϴ�
extern u8  ReceiveData2[];

u8 RxFlag1   = 1;
u8 TxFlag1   = 1;
u8 RecLen1   = 1;
u8 TxCount1  = 0;
u8 USART1BufferCNT=0;
u8 RecDataBuffer1[256];
u8 USART1SendTCB[256];
extern u8  ReceiveData1[];

u8 RxFlag3   = 1;
u8 TxFlag3   = 1;
u8 RecLen3   = 1;
u8 TxCount3  = 0;
u8 USART3BufferCNT=0;
u8 RecDataBuffer3[128];
u8 USART3SendTCB[128];
extern u8  ReceiveData3[];

u8 RxFlag4   = 1;
u8 TxFlag4   = 1;
u8 RecLen4   = 1;
u8 TxCount4  = 0;
u8 UART4BufferCNT=0;
u8 RecDataBuffer4[256];
u8 UART4SendTCB[256];
extern u8  ReceiveData4[];

u8 RxFlag5   = 1;
u8 TxFlag5   = 1;
u8 RecLen5   = 1;
u8 TxCount5  = 0;
u8 UART5BufferCNT=0;
u8 RecDataBuffer5[128];
u8 UART5SendTCB[128];

u8 SI4463TxFlag = 1;
u8 SI4463RxFlag = 1;


u8 SI4463_RxBUFF[128] = {0};
u8 SI4463_RxLenth = 0;
u8 Int_BUFF[9] = {0};
u8 cmd[2] = {0}; 

u32   Capture_time;
static u16   IC5ReadValue1, IC5ReadValue2;
static u8    CaptureNumber = 0,test1_up=0,test2_down=0;
static u8    TIM5_pulse_num;
static u8    pulse_I;

static u16 rain_time_second=0;
static u16 rain_last_times=0;
extern u16 report_last_rain; 

extern u8    level_num_err;//���20�η��ͼ���������û���յ�TIME5D1�ж�,��ϵͳ��������TIME5�жϳ�����level_num_err����;��IOxh_send_cmd�����м�1
extern u8 TIM5_pulsePA1_NUM;
//������ⶨ��
extern u8 freq_I;
extern u16 TIM2_FrequencyPA0[];//������̼
extern u16 TIM2_FrequencyPC0[];//����
extern u16 TIM2_FrequencyPA1[];//������ʪ��
extern u8 factory_gateway_set[];//���ز����趨

extern u32 TIM5_pulsePA1[];//������ʪ��
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

void SysTick_Handler(void)
{
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/
void EXTI0_IRQHandler(void)
{
  TIM2_FrequencyPC0[freq_I]++;
	EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI1_IRQHandler(void)
{
  TIM2_FrequencyPA1[freq_I]++;
	rain_last_times++;
	EXTI_ClearITPendingBit(EXTI_Line1);
}
void EXTI9_5_IRQHandler(void)
{
}
void EXTI15_10_IRQHandler(void)
{
}
/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
 /*nIRQ�жϴ�����*/
 	Stop_timerEx(WX_CMD_EVT);
	SI4463_INT_STATUS(Int_BUFF);				//��ȡ�ж�״̬��ͬʱ���ж�
	if(Int_BUFF[3] & ( 1<<3 ))				    //�����CRC����
	{	
		Start_timerEx(WX_CMD_EVT,20);
	}
	if(Int_BUFF[3] & ( 1<<4 ))				    //����ǽ����ж�
	{	
		Start_timerEx(WX_RECEIVE_EVT,50);
	}
	else if(Int_BUFF[3] & ( 1<<5 ))
	{	
	
		Start_timerEx(WX_CMD_EVT,20);
	}
	SI4463_INT_STATUS(Int_BUFF);
	EXTI_ClearITPendingBit(EXTI_Line4);
}


//void USART1_IRQHandler(void)
//{	
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//	{
//		RxFlag1=0;
//	
//		/* Read one byte from the receive data register */
//	    RecDataBuffer1[RecLen1] = USART_ReceiveData(USART1);
//		RecLen1+=1;
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
//    Start_timerEx(RX1_TIMEOUT_EVT,3);  //�����ݽ��к�������
//		
//	}
//        
//	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
//	{
//		                                              
//     if(TxCount1<USART1BufferCNT)
//     { USART_SendData(USART1,USART1SendTCB[TxCount1++]);}
//     else   //������ɺ�ر�TXE�ж�
//     { 
//			 USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
//			 USART_ITConfig(USART1, USART_IT_TC, ENABLE);    //ʹ�ܷ�������ж�								
//			}	
//	}	
//	if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
//    { 
//      USART_ITConfig(USART1, USART_IT_TC, DISABLE);
//      TxCount1=0;
//		  USART1BufferCNT=0;
//		  TxFlag1=1;
//      RxFlag1=1;			
//    }
// }

void USART3_IRQHandler(void)
{		
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{ 	   
		/* Read one byte from the receive data register */
		RxFlag3=0;		
	  RecDataBuffer3[RecLen3++] = USART_ReceiveData(USART3);		
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);		
		Start_timerEx(RX3_TIMEOUT_EVT,80);  //�����ݽ��к�������
	 }
}

//DMA ����Ӧ��Դ��
/*����1--DMA����*/ 
void DMA1_Channel4_IRQHandler(void)//����1���ͳ���
{
    if(DMA_GetITStatus(DMA1_FLAG_TC4))
    {
			while ( USART_GetFlagStatus(USART1,USART_FLAG_TC ) == RESET ){}
      DMA_ClearFlag(DMA1_FLAG_GL4);         // ���DMA1_Channel4������ɱ�־��DMA1_FLAG_TC4 | DMA1_FLAG_TE4 
			
			USART1->SR;
			USART1->DR;
			USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);  // �رմ���DMA����
      DMA_Cmd(DMA1_Channel4, DISABLE);   // �ر�DMA����ͨ��
      TxFlag1=0;          // ���ñ�־λ������1�������
			RxFlag1=1; 
			    		
    }
}

void USART1_IRQHandler(void)//����1���ճ���
{
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  // �����ж�
    {			  
        DMA_Cmd(DMA1_Channel5, DISABLE);       // �ر�DMA����ͨ�� ����ֹ����
				DMA_ClearFlag( DMA1_FLAG_GL5 );           // ��DMA1_Channel4���ձ�־λ��DMA1_FLAG_TC4 | DMA1_FLAG_TE4
			  					
				RecLen1 = 256 - DMA_GetCurrDataCounter(DMA1_Channel5); //��ý��յ����ֽ���
				DMA1_Channel5->CNDTR = 256;    //  ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ
			  memcpy(ReceiveData1,RecDataBuffer1,RecLen1);	
				USART1->SR;   //����жϱ�־λUSART_FLAG_ORE
        USART1->DR;   //����жϱ�־λ	
			  DMA_Cmd(DMA1_Channel5, ENABLE);        /* DMA �������ȴ����ݡ�ע�⣬����жϷ�������֡�����ʺܿ죬MCU����������˴ν��յ������ݣ��ж��ַ������ݵĻ������ﲻ�ܿ������������ݻᱻ���ǡ���2�ַ�ʽ�����
                                                   1. �����¿�������DMAͨ��֮ǰ����HZJMKJ_Rx_Buf��������������ݸ��Ƶ�����һ�������У�Ȼ���ٿ���DMA��Ȼ�����ϴ����Ƴ��������ݡ�
                                                   2. ����˫���壬��HZJMKJ_Uart_DMA_Rx_Data�����У���������DMA_MemoryBaseAddr �Ļ�������ַ����ô�´ν��յ������ݾͻᱣ�浽�µĻ������У������ڱ����ǡ�*/
        RxFlag1=0;// ���ͽ��յ������ݱ�־����ǰ̨�����ѯ
        			
			  Start_timerEx(RX1_DELAY_EVT,3);  //�����ݽ��к�������
    }
}


/*����2--DMA����*/ 
void DMA1_Channel7_IRQHandler(void)//����2���ͳ���
{
    if(DMA_GetITStatus(DMA1_FLAG_TC7))
    {
			while ( USART_GetFlagStatus(USART2,USART_FLAG_TC ) == RESET ){}
      DMA_ClearFlag(DMA1_FLAG_GL7);         // ���DMA1_Channel7������ɱ�־��DMA1_FLAG_TC7 | DMA1_FLAG_TE7 
			
			USART2->SR;
			USART2->DR;
			USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);  // �رմ���DMA����
      DMA_Cmd(DMA1_Channel7, DISABLE);   // �ر�DMA����ͨ��
      TxFlag2=0;          // ���ñ�־λ������2�������
			RxFlag2=1; 
			    		
    }
}

void USART2_IRQHandler(void)//����2���ճ���
{
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  // �����ж�
    {			  
        DMA_Cmd(DMA1_Channel6, DISABLE);       // �ر�DMA����ͨ�� ����ֹ����
				DMA_ClearFlag( DMA1_FLAG_GL6 );           // ��DMA1_Channel6���ձ�־λ��DMA1_FLAG_TC6 | DMA1_FLAG_TE6
			  					
				RecLen2 = 256 - DMA_GetCurrDataCounter(DMA1_Channel6); //��ý��յ����ֽ���
				DMA1_Channel6->CNDTR = 256;    //  ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ
			  memcpy(ReceiveData2,RecDataBuffer2,RecLen2);	
				USART2->SR;   //����жϱ�־λUSART_FLAG_ORE
        USART2->DR;   //����жϱ�־λ	
			  DMA_Cmd(DMA1_Channel6, ENABLE);        /* DMA �������ȴ����ݡ�ע�⣬����жϷ�������֡�����ʺܿ죬MCU����������˴ν��յ������ݣ��ж��ַ������ݵĻ������ﲻ�ܿ������������ݻᱻ���ǡ���2�ַ�ʽ�����
                                                   1. �����¿�������DMAͨ��֮ǰ����HZJMKJ_Rx_Buf��������������ݸ��Ƶ�����һ�������У�Ȼ���ٿ���DMA��Ȼ�����ϴ����Ƴ��������ݡ�
                                                   2. ����˫���壬��HZJMKJ_Uart_DMA_Rx_Data�����У���������DMA_MemoryBaseAddr �Ļ�������ַ����ô�´ν��յ������ݾͻᱣ�浽�µĻ������У������ڱ����ǡ�*/
        RxFlag2=0;// ���ͽ��յ������ݱ�־����ǰ̨�����ѯ
        			
			  Start_timerEx(RX2_DELAY_EVT,3);  //�����ݽ��к�������
    }
}


/*����3--DMA����*/ 
void DMA1_Channel2_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_FLAG_TC2))
    {
			while ( USART_GetFlagStatus(USART3,USART_FLAG_TC ) == RESET ){}
      DMA_ClearFlag(DMA1_FLAG_GL2);         // ���������ɱ�־��DMA1_FLAG_TC2 | DMA1_FLAG_TE2 
			
			USART3->SR;
			USART3->DR;
			USART_DMACmd(USART3, USART_DMAReq_Tx, DISABLE);  // �رմ���DMA����
      DMA_Cmd(DMA1_Channel2, DISABLE);   // �ر�DMA����ͨ��
      TxFlag3=0;          // ���ñ�־λ������3�������
			RxFlag3=1; 
			RXENABLE3;      		
    }
}

/*����4--DMA����*/ 
void DMA2_Channel4_5_IRQHandler(void)//����4������ɳ���
{
    if(DMA_GetITStatus(DMA2_FLAG_TC5))
    {
			while ( USART_GetFlagStatus(UART4,USART_FLAG_TC ) == RESET ){}
      DMA_ClearFlag(DMA2_FLAG_GL5);         // ���DMA2_Channel5������ɱ�־��DMA2_FLAG_TC5 | DMA2_FLAG_TE5 
			UART4->SR;
			UART4->DR;
			USART_DMACmd(UART4, USART_DMAReq_Tx, DISABLE);  // �رմ���DMA����
      DMA_Cmd(DMA2_Channel5, DISABLE);   // �ر�DMA����ͨ��
      TxFlag4=0;          // ���ñ�־λ������2�������
			RxFlag4=1;			
			RXENABLE4;      				
    }
}

void UART4_IRQHandler(void)//��������ж�  ����4���ճ���
{
    if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)  // �����ж�
    {		
        			
        DMA_Cmd(DMA2_Channel3, DISABLE);       // �ر�DMA����ͨ�� ����ֹ����
				DMA_ClearFlag( DMA2_FLAG_GL3 );           // ��DMA1_Channel3���ձ�־λ��DMA1_FLAG_TC3 | DMA1_FLAG_TE3
			  				
				RecLen4 = 128 - DMA_GetCurrDataCounter(DMA2_Channel3); //��ý��յ����ֽ���
				DMA2_Channel3->CNDTR = 128;    //  ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ
			  memcpy(ReceiveData4,RecDataBuffer4,RecLen4);
        UART4->SR;   //����жϱ�־λUSART_FLAG_ORE
        UART4->DR;   //����жϱ�־λ	
				DMA_Cmd(DMA2_Channel3, ENABLE);       
			/* DMA �������ȴ����ݡ�ע�⣬����жϷ�������֡�����ʺܿ죬MCU����������˴ν��յ������ݣ��ж��ַ������ݵĻ������ﲻ�ܿ�����
			   �������ݻᱻ���ǡ���2�ַ�ʽ�����
 1. �����¿�������DMAͨ��֮ǰ����HZJMKJ_Rx_Buf��������������ݸ��Ƶ�����һ�������У�Ȼ���ٿ���DMA��Ȼ�����ϴ����Ƴ��������ݡ�
 2. ����˫���壬��HZJMKJ_Uart_DMA_Rx_Data�����У���������DMA_MemoryBaseAddr �Ļ�������ַ����ô�´ν��յ������ݾͻᱣ�浽�µĻ������У�
			�����ڱ����ǡ�*/        
			  RxFlag4=0;// ���ͽ��յ������ݱ�־����ǰ̨�����ѯ        
			  Start_timerEx(RX4_DELAY_EVT,3);  //�����ݽ��к�������
    }
}

void UART5_IRQHandler(void)
{	
//  u8 clear;	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{ 
	   RxFlag5=0;  
		/* Read one byte from the receive data register */
	    RecDataBuffer5[RecLen5] = USART_ReceiveData(UART5);
		RecLen5+=1;
		USART_ClearITPendingBit(UART5,USART_IT_RXNE);
    Start_timerEx(RX5_TIMEOUT_EVT,30);  //�����ݽ��к�������		
	}
	else if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
	{  
    USART_SendData(UART5,UART5SendTCB[TxCount5++]);
    if(TxCount5>=UART5BufferCNT)
     { 
			 USART_ITConfig(UART5,USART_IT_TXE,DISABLE);
			 USART_ClearITPendingBit(UART5, USART_IT_TXE);
			 USART_ITConfig(UART5, USART_IT_TC, ENABLE);    //ʹ�ܷ�������ж�
			}
		} 
	else if (USART_GetITStatus(UART5, USART_IT_TC) != RESET)//ʹ�ܷ�����ɱ�־��תΪ����
	{ 
		USART_ITConfig(UART5, USART_IT_TC, DISABLE);
		USART_ClearITPendingBit(UART5, USART_IT_TC);
		RXENABLE5;
		TxCount5=0;
		UART5BufferCNT=0;
		TxFlag5=1;			
		RxFlag5=1;        
	}   		
}

void TIM2_IRQHandler(void) 
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
		{	
			freq_I++;   //ÿ��1s�����ж�,��freq_I++,�൱�������Լ�¼ʱ��
			if(freq_I>60){freq_I=0;}
			TIM2_FrequencyPC0[freq_I]=0;
			TIM2_FrequencyPA0[freq_I]=0;
			TIM2_FrequencyPA1[freq_I]=0;
			
			rain_time_second++;
			if(rain_time_second>=180)
       {
				 if(rain_last_times<=2){rain_last_times=0;}
				 else{rain_last_times=rain_last_times-2;}
				 report_last_rain=rain_last_times;
				 rain_last_times=0;
			   rain_time_second=0;
			 }
			TIM_ClearITPendingBit(TIM2,TIM_IT_Update);		
		}
		else if(TIM_GetITStatus(TIM2,TIM_IT_CC1) == SET)
		{
			TIM_ClearITPendingBit(TIM2,TIM_IT_CC1);
			TIM2_FrequencyPA0[freq_I]++;	
		}	
}

void TIM5_IRQHandler(void)
{
  if(factory_gateway_set[15]==18)
	{    
    if(TIM_GetITStatus(TIM5, TIM_IT_CC2) == SET) 
    {			  
        /* Clear TIM5 Capture compare interrupt pending bit */
        TIM_ClearITPendingBit(TIM5, TIM_IT_CC2);
        level_num_err=0;			
        if(CaptureNumber == 0)
        {
					  /* Get the Input Capture value */
					  TIM5->CCER|=(1<<5);//�½��ز���
            IC5ReadValue1 = TIM_GetCapture2(TIM5);
					  TIM5_pulse_num=1;
            CaptureNumber = 1;
            test1_up++;
            test2_down=0;					
        }
        else if(CaptureNumber == 1)
        {
            /* Get the Input Capture value */
//					  TIM5->CCER&=~(1<<5);//�����ز���
					  test2_down++;
            test1_up=0;					
            IC5ReadValue2 = TIM_GetCapture2(TIM5);             
            /* Capture computation */
            if (IC5ReadValue2 > IC5ReadValue1)
            {
                Capture_time = (IC5ReadValue2 - IC5ReadValue1); //|(TIM5_pulse_num-1)*65536Ϊ(TIM5_pulse_num-1)<<16 |((TIM5_pulse_num-1)<<16)
							  TIM5_pulse_num=1;
            }
            else
            {
                Capture_time = ((0xFFFF - IC5ReadValue1) + IC5ReadValue2); //|(TIM5_pulse_num-1)*65536Ϊ(TIM5_pulse_num-1)<<16
						    TIM5_pulse_num=1;//|((TIM5_pulse_num-1)<<16)
            }
						
//            TIM_ITConfig(TIM5,TIM_IT_CC2,DISABLE);
//	          TIM_ITConfig(TIM5,TIM_IT_Update,DISABLE );
//						TIM_Cmd(TIM5,DISABLE);						
						if(pulse_I>=TIM5_pulsePA1_NUM){pulse_I=0;}
            if((Capture_time<0x9696)&&(Capture_time>=128))//������·200us��ʱ��ȥ��0x9696us=6553.5mm
            {							
							TIM5_pulsePA1[pulse_I]=Capture_time;//-512
							pulse_I++;
						}
//						else
//						{
//						  TIM5_pulsePA1[pulse_I]=0x9696;
//							pulse_I++;
//						}
            CaptureNumber = 0;
        }
    }
  if(TIM_GetITStatus(TIM5,TIM_IT_Update) != RESET)
    {
		  TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
			TIM5_pulse_num++;
//			if((TIM5_pulse_num)>5){while(1);}
			if((test1_up)>5){while(1);}//ֻ�յ��������жϣ�û���յ��½����ж�
			if((test2_down)>5){while(1);}//ֻ�յ��������жϣ�û���յ��Ͻ����ж�
		}
	}

}




/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
