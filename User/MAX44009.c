#include "max44009.h"
#include "math.h"

/**************************** 
����I2C 
������ʱ�Ӹߵ�ƽ��ʱ��Ӹ�����Ծ�� 
*****************************/ 

void MAX44009_Transtart(void) 
{ 
  LIGHT_DTA_W; 
  LIGHT_DTA_H; 
  LIGHT_SCK_H; 
  Delayus(TIMEOUT); 
  LIGHT_DTA_L; 
  Delayus(TIMEOUT);  
  LIGHT_SCK_L; 
  Delayus(TIMEOUT);
  LIGHT_DTA_R;//��ȫʹ��		
} 

/******************************** 

����I2C 

������ʱ�Ӹߵ�ƽ��ʱ��ӵ�����Ծ�� 
********************************/ 

void MAX44009_Transtop(void) 
{ 
  LIGHT_DTA_W; 
  LIGHT_DTA_L;
  LIGHT_SCK_H; 
  Delayus(TIMEOUT);
  LIGHT_DTA_H; 
  Delayus(TIMEOUT);  
  LIGHT_SCK_L; 
  Delayus(TIMEOUT);
  LIGHT_DTA_R;//��ȫʹ��		
} 

/****************************** 
�����ֽڲ����ж��Ƿ��յ�ACK 
���յ�ACK����Ϊ0�����򷵻�Ϊ1 
******************************/ 
u8 MAX44009_Send(u8 val)                  
{ 
  u8 i;  
  u8 err=0;
  
  LIGHT_DTA_W;

  for(i=0;i<8;i++)
  {
    if(val&0x80)
      LIGHT_DTA_H;
    else
      LIGHT_DTA_L;
      
    Delayus(TIMEOUT); 
    LIGHT_SCK_H;  
    Delayus(TIMEOUT); 
    LIGHT_SCK_L;
    val<<=1;
    Delayus(TIMEOUT); 
  } 
  
  LIGHT_DTA_H; 
  LIGHT_DTA_R;
  Delayus(TIMEOUT); 
  LIGHT_SCK_H;  
  Delayus(TIMEOUT); 

  if(LIGHT_DTA)
  { 
    err=0; 
  }
  else
  {
    err=1;
  } 
  
  Delayus(TIMEOUT); 
  LIGHT_SCK_L;
  Delayus(TIMEOUT);
   
  return err; 
} 


/*************************** 
��ȡI2C���ֽڣ����ҷ���ACK 
������Ϊ1��ʱ����һ��ACK(�͵�ƽ) 
***************************/ 

u8 MAX44009_Read(u8 ack) 
{ 
  u8 i; 
  u8 val=0;
  
  LIGHT_DTA_R;
  
  for(i=0;i<8;i++) 
  { 
    val<<=1;
    LIGHT_SCK_H;    
    Delayus(TIMEOUT); 
    if(LIGHT_DTA) 
      val|=0x01; 
    LIGHT_SCK_L; 
    Delayus(TIMEOUT); 
  }   
  LIGHT_DTA_W;

  if(ack!=0) 
    LIGHT_DTA_L; 
  else
    LIGHT_DTA_H;
  
  Delayus(TIMEOUT);
  LIGHT_SCK_H; 
  Delayus(TIMEOUT); 
  LIGHT_SCK_L; 
  LIGHT_DTA_H;
  LIGHT_DTA_R;//��ȫʹ��	
  return val; 
} 

void MAX44009_INIT(void)
{ 
	MAX44009_Transtart();
	MAX44009_Send(MAX44009_WRITE);   //���͵�ַ	
	MAX44009_Send(CONFIGURATION);
	MAX44009_Send(0x80);
	MAX44009_Transtop();	
}
  
u16 get_light(void) 
{   
  u16 value=0x0000; 

  MAX44009_Transtart();
  MAX44009_Send(MAX44009_WRITE);   //����slaveд��ַ 
  MAX44009_Send(LUX_HIGH_BYTE);	   //���͹��ո��ֽڼĴ�����ַ

  MAX44009_Transtart();				//restart
  MAX44009_Send(MAX44009_READ);	 	//����slave����ַ
  value = MAX44009_Read(0); 		//�����ո��ֽ�

  MAX44009_Transtart();			   //restart
  MAX44009_Send(MAX44009_WRITE);   //����slaveд��ַ 
  MAX44009_Send(LUX_LOW_BYTE);	   //���͹��յ��ֽڼĴ�����ַ

  MAX44009_Transtart();			   //restart
  MAX44009_Send(MAX44009_READ);	   //����slave����ַ
  
  value=(value<<8) | MAX44009_Read(0);	 //�����յ��ֽ�
 
  MAX44009_Transtop();			   //ֹͣIIC����

  return value; 
}

u16 Get_Illuminance(void)
{
  u16 result = 0x0000;
  u16 temp = 0x0000;
 
  u8  exp = 0x00;
  u8  coef = 0x00;

  double res = 0.0;
  
  temp = get_light();
  exp =  (temp & 0xF000) >> 12;
  coef = ((temp & 0x0F00) >> 4) | (temp & 0x000F);

  if(exp == 0x0F)	 //ָ�����������������0xFFFF��Ϊ����
  {
  	return 0xFFFF;
  }
  else
  {
  	res = pow(2,exp) * 0.045 * coef / 10;  //��ʵ��ֵ��1/10
	result = (u16)res;
  	return result; 	
  }
	
}

u16 GET_PRESSUE0(void) //����ѹ��;ͨ��0-���ն�PC0
{   
  u16 value;
  u16 data[5]={0x00};
  
  MAX44009_Transtart();
  MAX44009_Send(0x50);
  MAX44009_Send(0xAC);
  MAX44009_Transtop();
  Delayms(100);//�����ʱ����Ҫ��ʱ��Ҫ����37ms
  
  MAX44009_Transtart(); 
  MAX44009_Send(0x51);   //���͵�ַ   
  data[0]=MAX44009_Read(1); 
  data[1]=MAX44009_Read(1); 
  data[2]=MAX44009_Read(1);
  data[3]=MAX44009_Read(1);
  data[4]=MAX44009_Read(0);
  MAX44009_Transtop(); 
  
  if((data[0]&0x60)==0x40)
  {
  	value=(data[1]<<8)|data[2];
  }
  else
  {
   	value=0xffff;
  }
  return value; 
}

u16 GET_level0(void) //Һλѹ��cps120;ͨ��0-���ն�PC0
{   
  u16 value;
  u16 data[5]={0x00};
  
  MAX44009_Transtart();
  MAX44009_Send(0x50);
  MAX44009_Send(0xAC);
  MAX44009_Transtop();
  Delayms(100);//�����ʱ����Ҫ��ʱ��Ҫ����37ms
  
  MAX44009_Transtart(); 
  MAX44009_Send(0x51);   //���͵�ַ   
  data[0]=MAX44009_Read(1); 
  data[1]=MAX44009_Read(1); 
  data[2]=MAX44009_Read(1);
  data[3]=MAX44009_Read(0);  
  MAX44009_Transtop(); 
  
  if((data[0]&0xC0)==0x00)
  {
  	value=(data[0]<<8)|data[1];
  }
  else
  {
   	value=0xffff;
  }
  return value; 
}


