#include "myPID.h"
#include "math.h"


/*PID初始化函数*/

void PID_init(vPID *mypid,float vMin,float vMax,float sp)
{
  mypid->maximum=vMax;                /*输出值上限*/
  mypid->minimum=vMin;                /*输出值下限*/

  mypid->setpoint=sp;                 /*设定值*/
  mypid->kp=0.6;                      /*比例系数*/
  mypid->ki=0.03;                     /*积分系数*/                

  mypid->lasterror=0.0;              /*前一拍偏差*/
  mypid->preerror=0.0;               /*前两拍偏差*/
  mypid->result=vMin;                /*PID控制器结果*/
  mypid->output=0.0;                 /*输出值*/

  mypid->errorabsmax=(vMax-vMin)*0.8;
  mypid->errorabsmin=(vMax-vMin)*0.2;

  mypid->deadband=(vMax-vMin)*0.005;               /*死区*/
 
  mypid->integralValue=0.0;
}

void PIDRegulator(vPID *mypid,float pv) //pv为processvalue 实际测量值
{
  float thisError;
  float result;
  float increment;
  float pError,iError;

  thisError=mypid->setpoint-pv; //得到偏差值
  result=mypid->result;
  if (fabs(thisError)>mypid->deadband)
  {
    pError=thisError-mypid->lasterror;
    iError=thisError;
    
    increment=mypid->kp*pError+mypid->ki*iError;   //增量计算,去除kd
  }
  else
  {
    if((fabs(mypid->setpoint-mypid->minimum)<mypid->deadband)&&(fabs(pv-mypid->minimum)<mypid->deadband))
    {
      result=mypid->minimum;
    }
    increment=0.0;
  }

  result=result+increment;  //计算输出

  /*对输出限值，避免超调和积分饱和问题*/
  if(result>=mypid->maximum)
  {
    result=mypid->maximum;
  }
  if(result<=mypid->minimum)
  {
    result=mypid->minimum;
  } 

  mypid->preerror=mypid->lasterror;  //存放偏差用于下次运算
  mypid->lasterror=thisError;
  mypid->result=result;
  //myPID.output=((result-myPID.minimum)/(myPID.maximum-myPID.minimum))*100.0;
	//t_motor = increment*0.1/(myPID.maximum-myPID.minimum)/0.02    电机开转时间,单位s,thisError为正则正转,阀门开大,否则反转
}

