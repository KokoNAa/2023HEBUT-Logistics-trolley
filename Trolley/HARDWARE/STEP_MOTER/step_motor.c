#include "allFiles.h"

uint16_t CCR1 = 20000;

speedRampData srd1= {STOP,0,0,0,0,0,0};
speedRampData srd2= {STOP,0,0,0,0,0,0};
speedRampData srd3= {STOP,0,0,0,0,0,0};
speedRampData srd4= {STOP,0,0,0,0,0,0};

uint8_t  motor_sta        = 0;//电机状态

/*********************************************
 *函数：步进电机方向使能引脚初始化函数
 *函数名：step_motor_init(void)
 *备注：
 *日期：2023.08.03
 *修改日期：
 *********************************************/
void step_motor_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ST1_DIR_GPIO_PIN  | ST1_EN_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉(共阳极接法，0为有效)
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = ST2_DIR_GPIO_PIN  | ST2_EN_GPIO_PIN;
	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = ST3_DIR_GPIO_PIN  | ST3_EN_GPIO_PIN;
	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = ST4_DIR_GPIO_PIN  | ST4_EN_GPIO_PIN;
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化
}
/*********************************************
 *函数：步进电机运动函数
 *函数名：MOTOR_Move(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
 *备注：step   移动步数（正数为正转，负数为逆时针）
		accel  加速度,实际值为accel*0.1*rad/sec^2  10倍并且2个脉冲算一个完整的周期
		decel  减速度,实际值为decel*0.1*rad/sec^2
		speed  最大速度,实际值为speed*0.1*rad/sec
日期：2023.08.03
*修改日期：
* *********************************************/

void ST1_Move(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
		
    uint16_t tim_count; 																										 //存放中断时刻的计数值
    unsigned int max_s_lim;                                     					 	 //达到最大速度时的步数    
    unsigned int accel_lim;																									 //必须开始减速的步数（如果还没有加速度到最大速度时）

	if(motor_sta!= STOP)  																									 //只允许步进电机在停止的时候才继续
			return;			
    if(step < 0)   								//逆时针
	{
		GPIO_SetBits(ST1_DIR_GPIO_PORT,ST1_DIR_GPIO_PIN);			//输出高电平
		step = -step;      
	}		
	else   			//顺时针
	{
		GPIO_ResetBits(ST1_DIR_GPIO_PORT,ST1_DIR_GPIO_PIN);		//输出低电平		
	}        
    if(step == 1)   																											   // 如果只移动一步
    {       
      srd1.accel_count = -1; 																								 // 只移动一步
        
      srd1.run_state = DECEL;																								 // 减速状态
        
      srd1.step_delay = 1000;																								 // 短延时

     }
    
    else if(step != 0)  																			 								// 步数不为零才移动
    {					
				srd1.min_delay = (int32_t)(A_T_x10/speed);							 // 设置最大速度极限, 计算min_delay用于定时器的计数器的值min_delay = (alpha / tt)/ 
				srd1.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);		// 通过计算第一个(c0) 的步进延时来设定加速度,其中accel单位为0.01rad/sec^2
					// step_delay = 1/tt * sqrt(2*alpha/accel)
				// step_delay = ( tfreq*0.69/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 
			    max_s_lim = (uint32_t)(speed*speed/(A_x200*accel/10));   //计算多少步之后达到最大速度的限制 max_s_lim = speed^2 / (2*alpha*accel)
    
				if(max_s_lim == 0)									//如果达到最大速度小于0.5步，我们将四舍五入为0,但实际我们必须移动至少一步才能达到想要的速度 
				{
					max_s_lim = 1;
				}    
				accel_lim = (uint32_t)(step*decel/(accel+decel)); 										// 计算多少步之后我们必须开始减速,n1 = (n1+n2)decel / (accel + decel)
   
				if(accel_lim == 0) 																					// 我们必须加速至少1步才能开始减速
				{
					accel_lim = 1;
				}
   
				if(accel_lim <= max_s_lim)						//加速阶段到不了最大速度就得减速。。。使用限制条件我们可以计算出减速阶段步数 
				{
					srd1.decel_val = accel_lim - step;				//减速段的步数
				}
				else
				{
					srd1.decel_val = -(max_s_lim*accel/decel);	//减速段的步数 
				}
   
				if(srd1.decel_val == 0) 				// 不足一步 按一步处理 
				{
					srd1.decel_val = -1;
				}    
				srd1.decel_start = step + srd1.decel_val;																//计算开始减速时的步数
 
				if(srd1.step_delay <= srd1.min_delay)																		// 如果一开始c0的速度比匀速段速度还大，就不需要进行加速运动，直接进入匀速
				{
					srd1.step_delay = srd1.min_delay;
					srd1.run_state = RUN;
				}
				else
				{
					srd1.run_state = ACCEL;
				}
    
				srd1.accel_count = 0;																									// 复位加速度计数值
		
			}
			motor_sta = 1;  																												// 电机为运动状态
			tim_count = TIM_GetCounter(TIM3);																				//获取计数值
			TIM_SetCompare1(TIM3,tim_count+srd1.step_delay/2);												//设置定时器比较值 
			TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);																		//使能定时器通道 
			TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Enable);
			TIM_Cmd(TIM3, ENABLE);																									//开启定时器
}
void ST2_Move(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
		
    uint16_t tim_count; 																										 //存放中断时刻的计数值
    unsigned int max_s_lim;                                     					 	 //达到最大速度时的步数    
    unsigned int accel_lim;																									 //必须开始减速的步数（如果还没有加速度到最大速度时）

	if(motor_sta!= STOP)  																									 //只允许步进电机在停止的时候才继续
			return;			
    if(step < 0)   								//逆时针
	{
		GPIO_SetBits(ST2_DIR_GPIO_PORT,ST2_DIR_GPIO_PIN);			//输出高电平
		step = -step;      
	}		
	else   			//顺时针
	{
		GPIO_ResetBits(ST2_DIR_GPIO_PORT,ST2_DIR_GPIO_PIN);		//输出低电平		
	}        
    if(step == 1)   																											   // 如果只移动一步
    {       
      srd2.accel_count = -1; 																								 // 只移动一步
        
      srd2.run_state = DECEL;																								 // 减速状态
        
      srd2.step_delay = 1000;																								 // 短延时

     }
    
    else if(step != 0)  																			 								// 步数不为零才移动
    {					
				srd2.min_delay = (int32_t)(A_T_x10/speed);							 // 设置最大速度极限, 计算min_delay用于定时器的计数器的值min_delay = (alpha / tt)/ 
				srd2.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);		// 通过计算第一个(c0) 的步进延时来设定加速度,其中accel单位为0.01rad/sec^2
					// step_delay = 1/tt * sqrt(2*alpha/accel)
				// step_delay = ( tfreq*0.69/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 
			    max_s_lim = (uint32_t)(speed*speed/(A_x200*accel/10));   //计算多少步之后达到最大速度的限制 max_s_lim = speed^2 / (2*alpha*accel)
    
				if(max_s_lim == 0)									//如果达到最大速度小于0.5步，我们将四舍五入为0,但实际我们必须移动至少一步才能达到想要的速度 
				{
					max_s_lim = 1;
				}    
				accel_lim = (uint32_t)(step*decel/(accel+decel)); 										// 计算多少步之后我们必须开始减速,n1 = (n1+n2)decel / (accel + decel)
   
				if(accel_lim == 0) 																					// 我们必须加速至少1步才能开始减速
				{
					accel_lim = 1;
				}
   
				if(accel_lim <= max_s_lim)						//加速阶段到不了最大速度就得减速。。。使用限制条件我们可以计算出减速阶段步数 
				{
					srd2.decel_val = accel_lim - step;				//减速段的步数
				}
				else
				{
					srd2.decel_val = -(max_s_lim*accel/decel);	//减速段的步数 
				}
   
				if(srd2.decel_val == 0) 				// 不足一步 按一步处理 
				{
					srd2.decel_val = -1;
				}    
				srd2.decel_start = step + srd2.decel_val;																//计算开始减速时的步数
 
				if(srd2.step_delay <= srd2.min_delay)																		// 如果一开始c0的速度比匀速段速度还大，就不需要进行加速运动，直接进入匀速
				{
					srd2.step_delay = srd2.min_delay;
					srd2.run_state = RUN;
				}
				else
				{
					srd2.run_state = ACCEL;
				}
    
				srd2.accel_count = 0;																									// 复位加速度计数值
		
			}
			motor_sta = 1;  																												// 电机为运动状态
			tim_count = TIM_GetCounter(TIM11);																				//获取计数值
			TIM_SetCompare1(TIM11,tim_count+srd2.step_delay/2);												//设置定时器比较值 
			TIM_ITConfig(TIM11,TIM_IT_CC1,ENABLE);																		//使能定时器通道 
			TIM_CCxCmd(TIM11,TIM_Channel_1,TIM_CCx_Enable);
			TIM_Cmd(TIM11, ENABLE);																									//开启定时器
}
void ST3_Move(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
		
    uint16_t tim_count; 																										 //存放中断时刻的计数值
    unsigned int max_s_lim;                                     					 	 //达到最大速度时的步数    
    unsigned int accel_lim;																									 //必须开始减速的步数（如果还没有加速度到最大速度时）

	if(motor_sta!= STOP)  																									 //只允许步进电机在停止的时候才继续
			return;			
    if(step < 0)   								//逆时针
	{
		GPIO_SetBits(ST3_DIR_GPIO_PORT,ST3_DIR_GPIO_PIN);			//输出高电平
		step = -step;      
	}		
	else   			//顺时针
	{
		GPIO_ResetBits(ST3_DIR_GPIO_PORT,ST3_DIR_GPIO_PIN);		//输出低电平		
	}        
    if(step == 1)   																											   // 如果只移动一步
    {       
      srd3.accel_count = -1; 																								 // 只移动一步
        
      srd3.run_state = DECEL;																								 // 减速状态
        
      srd3.step_delay = 1000;																								 // 短延时

     }
    
    else if(step != 0)  																			 								// 步数不为零才移动
    {					
				srd3.min_delay = (int32_t)(A_T_x10/speed);							 // 设置最大速度极限, 计算min_delay用于定时器的计数器的值min_delay = (alpha / tt)/ 
				srd3.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);		// 通过计算第一个(c0) 的步进延时来设定加速度,其中accel单位为0.01rad/sec^2
					// step_delay = 1/tt * sqrt(2*alpha/accel)
				// step_delay = ( tfreq*0.69/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 
			    max_s_lim = (uint32_t)(speed*speed/(A_x200*accel/10));   //计算多少步之后达到最大速度的限制 max_s_lim = speed^2 / (2*alpha*accel)
    
				if(max_s_lim == 0)									//如果达到最大速度小于0.5步，我们将四舍五入为0,但实际我们必须移动至少一步才能达到想要的速度 
				{
					max_s_lim = 1;
				}    
				accel_lim = (uint32_t)(step*decel/(accel+decel)); 										// 计算多少步之后我们必须开始减速,n1 = (n1+n2)decel / (accel + decel)
   
				if(accel_lim == 0) 																					// 我们必须加速至少1步才能开始减速
				{
					accel_lim = 1;
				}
   
				if(accel_lim <= max_s_lim)						//加速阶段到不了最大速度就得减速。。。使用限制条件我们可以计算出减速阶段步数 
				{
					srd3.decel_val = accel_lim - step;				//减速段的步数
				}
				else
				{
					srd3.decel_val = -(max_s_lim*accel/decel);	//减速段的步数 
				}
   
				if(srd3.decel_val == 0) 				// 不足一步 按一步处理 
				{
					srd3.decel_val = -1;
				}    
				srd3.decel_start = step + srd3.decel_val;																//计算开始减速时的步数
 
				if(srd3.step_delay <= srd3.min_delay)				// 如果一开始c0的速度比匀速段速度还大，就不需要进行加速运动，直接进入匀速
				{
					srd3.step_delay = srd3.min_delay;
					srd3.run_state = RUN;
				}
				else
				{
					srd3.run_state = ACCEL;
				}
    
				srd3.accel_count = 0;																									// 复位加速度计数值
		
			}
			motor_sta = 1;  																												// 电机为运动状态
			tim_count = TIM_GetCounter(TIM10);																				//获取计数值
			TIM_SetCompare1(TIM10,tim_count+srd3.step_delay/2);												//设置定时器比较值 
			TIM_ITConfig(TIM10,TIM_IT_CC1,ENABLE);																		//使能定时器通道 
			TIM_CCxCmd(TIM10,TIM_Channel_1,TIM_CCx_Enable);
			TIM_Cmd(TIM10, ENABLE);																									//开启定时器
}
void ST4_Move(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
		
    uint16_t tim_count; 																										 //存放中断时刻的计数值
    unsigned int max_s_lim;                                     					 	 //达到最大速度时的步数    
    unsigned int accel_lim;																									 //必须开始减速的步数（如果还没有加速度到最大速度时）

	if(motor_sta!= STOP)  																									 //只允许步进电机在停止的时候才继续
			return;			
    if(step < 0)   								//逆时针
	{
		GPIO_SetBits(ST4_DIR_GPIO_PORT,ST4_DIR_GPIO_PIN);			//输出高电平
		step = -step;      
	}		
	else   			//顺时针
	{
		GPIO_ResetBits(ST4_DIR_GPIO_PORT,ST4_DIR_GPIO_PIN);		//输出低电平		
	}        
    if(step == 1)   																											   // 如果只移动一步
    {       
      srd4.accel_count = -1; 																								 // 只移动一步
        
      srd4.run_state = DECEL;																								 // 减速状态
        
      srd4.step_delay = 1000;																								 // 短延时

     }
    
    else if(step != 0)  																			 								// 步数不为零才移动
    {					
				srd4.min_delay = (int32_t)(A_T_x10/speed);							 // 设置最大速度极限, 计算min_delay用于定时器的计数器的值min_delay = (alpha / tt)/ 
				srd4.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);		// 通过计算第一个(c0) 的步进延时来设定加速度,其中accel单位为0.01rad/sec^2
					// step_delay = 1/tt * sqrt(2*alpha/accel)
				// step_delay = ( tfreq*0.69/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 
			    max_s_lim = (uint32_t)(speed*speed/(A_x200*accel/10));   //计算多少步之后达到最大速度的限制 max_s_lim = speed^2 / (2*alpha*accel)
    
				if(max_s_lim == 0)									//如果达到最大速度小于0.5步，我们将四舍五入为0,但实际我们必须移动至少一步才能达到想要的速度 
				{
					max_s_lim = 1;
				}    
				accel_lim = (uint32_t)(step*decel/(accel+decel)); 										// 计算多少步之后我们必须开始减速,n1 = (n1+n2)decel / (accel + decel)
   
				if(accel_lim == 0) 																					// 我们必须加速至少1步才能开始减速
				{
					accel_lim = 1;
				}
   
				if(accel_lim <= max_s_lim)						//加速阶段到不了最大速度就得减速。。。使用限制条件我们可以计算出减速阶段步数 
				{
					srd4.decel_val = accel_lim - step;				//减速段的步数
				}
				else
				{
					srd4.decel_val = -(max_s_lim*accel/decel);	//减速段的步数 
				}
   
				if(srd4.decel_val == 0) 				// 不足一步 按一步处理 
				{
					srd4.decel_val = -1;
				}    
				srd4.decel_start = step + srd4.decel_val;																//计算开始减速时的步数
 
				if(srd4.step_delay <= srd4.min_delay)				// 如果一开始c0的速度比匀速段速度还大，就不需要进行加速运动，直接进入匀速
				{
					srd4.step_delay = srd4.min_delay;
					srd4.run_state = RUN;
				}
				else
				{
					srd4.run_state = ACCEL;
				}
    
				srd4.accel_count = 0;																									// 复位加速度计数值
		
			}
			motor_sta = 1;  																												// 电机为运动状态
			tim_count = TIM_GetCounter(TIM9);																				//获取计数值
			TIM_SetCompare1(TIM9,tim_count+srd4.step_delay/2);												//设置定时器比较值 
			TIM_ITConfig(TIM9,TIM_IT_CC1,ENABLE);																		//使能定时器通道 
			TIM_CCxCmd(TIM9,TIM_Channel_1,TIM_CCx_Enable);
			TIM_Cmd(TIM9, ENABLE);																									//开启定时器
}
void speed_decision()                                                         //中断执行函数
{
	__IO uint32_t tim_count=0;
	__IO uint32_t tmp = 0;  
  uint16_t new_step_delay=0;                                                  // 保存新（下）一个延时周期  
  __IO static uint16_t last_accel_delay=0;                                    // 加速过程中最后一次延时（脉冲周期）. 
  __IO static uint32_t step_count = 0; 																			  // 总移动步数计数器  
  __IO static int32_t rest = 0;																								// 记录new_step_delay中的余数，提高下一步计算的精度  
  __IO static uint8_t i=0;																										//定时器使用翻转模式，需要进入两次中断才输出一个完整脉冲
 
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1)== SET)
  {	
      TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);																	// 清定时器中断		
	  tim_count = TIM_GetCounter(TIM3);																					//获取计数值
	  tmp = tim_count+srd1.step_delay/2;
	  TIM_SetCompare1(TIM3,tmp);		  // 设置比较值
	  i++;  
	  
	  if(i==2)																																	//中断两次为一个脉冲
		{
		  i=0; 
		  switch(srd1.run_state)
			{
				case STOP:																														//停止状态
					step_count = 0;
					rest = 0;
				  TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
				  TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Disable);					//单个电机可以关闭定时器，多个电机只关闭通道即可
				  TIM_Cmd(TIM3, DISABLE);	
					motor_sta = 0;  
					break;
				
				case ACCEL:																														//加速状态
				step_count++;
				srd1.accel_count++;
				new_step_delay = srd1.step_delay - (((2 *srd1.step_delay) + rest)/(4 * srd1.accel_count + 1));//计算新(下)一步脉冲周期(时间间隔)
				rest = ((2 * srd1.step_delay)+rest)%(4 * srd1.accel_count + 1);					// 计算余数，下次计算补上余数，减少误差
				
					if(step_count >= srd1.decel_start) 																	//检查是够应该开始减速
					{
						srd1.accel_count = srd1.decel_val;																	//加速计数值为减速阶段计数值的初始值
						srd1.run_state = DECEL;																						//下个脉冲进入减速阶段 
					}
					
					else if(new_step_delay <= srd1.min_delay)														//检查是否到达期望的最大速度
					{
						last_accel_delay = new_step_delay;																//保存加速过程中最后一次延时（脉冲周期）
						new_step_delay = srd1.min_delay;   																// 使用min_delay（对应最大速度speed） 
						rest = 0;            																							//清零余值               
						srd1.run_state = RUN;																							//设置为匀速运行状态 
					}
					break;
					
				case RUN:
          step_count++;  																											// 步数加1				  
          new_step_delay = srd1.min_delay;   																  // 使用min_delay（对应最大速度speed）				 
          if(step_count >= srd1.decel_start)   																// 需要开始减速
					{
            srd1.accel_count = srd1.decel_val;  																// 减速步数做为加速计数值
            new_step_delay = last_accel_delay;																// 加阶段最后的延时做为减速阶段的起始延时(脉冲周期)
            srd1.run_state = DECEL;           																  // 状态改变为减速
          }
          break;
					
				case DECEL:
          step_count++;  																											// 步数加1

          srd1.accel_count++; 																									// 是个负数
          new_step_delay = srd1.step_delay - (((2 * srd1.step_delay) + rest)/(4 * srd1.accel_count + 1)); //计算新(下)一步脉冲周期(时间间隔)
          rest = ((2 * srd1.step_delay)+rest)%(4 * srd1.accel_count + 1);				// 计算余数，下次计算补上余数，减少误差
          if(srd1.accel_count >= 0) 																						//检查是否为最后一步  是个负数所以要判断 大于等于零时 应该就是减速结束
          {
            srd1.run_state = STOP;
          }
          break;
			}
			 srd1.step_delay = new_step_delay; 																			// 为下个(新的)延时(脉冲周期)赋值
		}
	}
    if (TIM_GetITStatus(TIM11, TIM_IT_CC1)== SET)// ST2进中断
	{
		TIM_ClearITPendingBit(TIM11, TIM_IT_CC1);																	// 清定时器中断		
	    tim_count = TIM_GetCounter(TIM11);																					//获取计数值
	    tmp = tim_count+srd2.step_delay/2;
	    TIM_SetCompare1(TIM11,tmp);		  // 设置比较值
	    i++;  
	  if(i==2)																																	//中断两次为一个脉冲
		{
		  i=0; 
		  switch(srd2.run_state)
			{
				case STOP:																														//停止状态
					step_count = 0;
					rest = 0;
				    TIM_ClearITPendingBit(TIM11, TIM_IT_CC1);
				    TIM_CCxCmd(TIM11,TIM_Channel_1,TIM_CCx_Disable);					//单个电机可以关闭定时器，多个电机只关闭通道即可
				    TIM_Cmd(TIM11, DISABLE);	
					motor_sta = 0;  
					break;
				
				case ACCEL:																														//加速状态
					step_count++;
					srd2.accel_count++;
					new_step_delay = srd2.step_delay - (((2 *srd2.step_delay) + rest)/(4 * srd2.accel_count + 1));//计算新(下)一步脉冲周期(时间间隔)
					rest = ((2 * srd2.step_delay)+rest)%(4 * srd2.accel_count + 1);					// 计算余数，下次计算补上余数，减少误差
				
					if(step_count >= srd2.decel_start) 																	//检查是够应该开始减速
					{
						srd2.accel_count = srd2.decel_val;																	//加速计数值为减速阶段计数值的初始值
						srd2.run_state = DECEL;																						//下个脉冲进入减速阶段 
					}
					
					else if(new_step_delay <= srd2.min_delay)														//检查是否到达期望的最大速度
					{
						last_accel_delay = new_step_delay;																//保存加速过程中最后一次延时（脉冲周期）
						new_step_delay = srd2.min_delay;   																// 使用min_delay（对应最大速度speed） 
						rest = 0;            																							//清零余值               
						srd2.run_state = RUN;																							//设置为匀速运行状态 
					}
					break;
					
				case RUN:
					  step_count++;  																											// 步数加1				  
					  new_step_delay = srd2.min_delay;   																  // 使用min_delay（对应最大速度speed）				 
					  if(step_count >= srd2.decel_start)   																// 需要开始减速
					  {
						srd2.accel_count = srd2.decel_val;  																// 减速步数做为加速计数值
						new_step_delay = last_accel_delay;																// 加阶段最后的延时做为减速阶段的起始延时(脉冲周期)
						srd2.run_state = DECEL;           																  // 状态改变为减速
					  }
					  break;
					
				case DECEL:
				  step_count++;  																											// 步数加1

				  srd2.accel_count++; 																									// 是个负数
				  new_step_delay = srd2.step_delay - (((2 * srd2.step_delay) + rest)/(4 * srd2.accel_count + 1)); //计算新(下)一步脉冲周期(时间间隔)
				  rest = ((2 * srd2.step_delay)+rest)%(4 * srd2.accel_count + 1);				// 计算余数，下次计算补上余数，减少误差
				  if(srd2.accel_count >= 0) 																						//检查是否为最后一步  是个负数所以要判断 大于等于零时 应该就是减速结束
				  {
					srd2.run_state = STOP;
				  }
				  break;
			}
			 srd2.step_delay = new_step_delay; 																			// 为下个(新的)延时(脉冲周期)赋值
		}
	}
	if (TIM_GetITStatus(TIM10, TIM_IT_CC1)== SET)// ST3进中断
	{
		TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);																	// 清定时器中断		
	    tim_count = TIM_GetCounter(TIM10);																					//获取计数值
	    tmp = tim_count+srd3.step_delay/2;
	    TIM_SetCompare1(TIM10,tmp);		  // 设置比较值
	    i++;  
	  if(i==2)																																	//中断两次为一个脉冲
		{
		  i=0; 
		  switch(srd3.run_state)
			{
				case STOP:																														//停止状态
					step_count = 0;
					rest = 0;
				    TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);
				    TIM_CCxCmd(TIM10,TIM_Channel_1,TIM_CCx_Disable);					//单个电机可以关闭定时器，多个电机只关闭通道即可
				    TIM_Cmd(TIM10, DISABLE);	
					motor_sta = 0;  
					break;
				
				case ACCEL:																														//加速状态
					step_count++;
					srd3.accel_count++;
					new_step_delay = srd3.step_delay - (((2 *srd3.step_delay) + rest)/(4 * srd3.accel_count + 1));//计算新(下)一步脉冲周期(时间间隔)
					rest = ((2 * srd3.step_delay)+rest)%(4 * srd3.accel_count + 1);					// 计算余数，下次计算补上余数，减少误差
				
					if(step_count >= srd3.decel_start) 																	//检查是够应该开始减速
					{
						srd3.accel_count = srd3.decel_val;																	//加速计数值为减速阶段计数值的初始值
						srd3.run_state = DECEL;																						//下个脉冲进入减速阶段 
					}
					
					else if(new_step_delay <= srd3.min_delay)														//检查是否到达期望的最大速度
					{
						last_accel_delay = new_step_delay;																//保存加速过程中最后一次延时（脉冲周期）
						new_step_delay = srd3.min_delay;   																// 使用min_delay（对应最大速度speed） 
						rest = 0;            																							//清零余值               
						srd3.run_state = RUN;																							//设置为匀速运行状态 
					}
					break;
					
				case RUN:
					  step_count++;  																											// 步数加1				  
					  new_step_delay = srd3.min_delay;   																  // 使用min_delay（对应最大速度speed）				 
					  if(step_count >= srd3.decel_start)   																// 需要开始减速
					  {
						srd3.accel_count = srd3.decel_val;  																// 减速步数做为加速计数值
						new_step_delay = last_accel_delay;																// 加阶段最后的延时做为减速阶段的起始延时(脉冲周期)
						srd3.run_state = DECEL;           																  // 状态改变为减速
					  }
					  break;
					
				case DECEL:
				  step_count++;  																											// 步数加1

				  srd3.accel_count++; 																									// 是个负数
				  new_step_delay = srd3.step_delay - (((2 * srd3.step_delay) + rest)/(4 * srd3.accel_count + 1)); //计算新(下)一步脉冲周期(时间间隔)
				  rest = ((2 * srd3.step_delay)+rest)%(4 * srd3.accel_count + 1);				// 计算余数，下次计算补上余数，减少误差
				  if(srd3.accel_count >= 0) 																						//检查是否为最后一步  是个负数所以要判断 大于等于零时 应该就是减速结束
				  {
					srd3.run_state = STOP;
				  }
				  break;
			}
			 srd3.step_delay = new_step_delay; 																			// 为下个(新的)延时(脉冲周期)赋值
		}
	}
	if (TIM_GetITStatus(TIM9, TIM_IT_CC1)== SET)// ST3进中断
	{
		TIM_ClearITPendingBit(TIM9, TIM_IT_CC1);																	// 清定时器中断		
	    tim_count = TIM_GetCounter(TIM9);																					//获取计数值
	    tmp = tim_count+srd4.step_delay/2;
	    TIM_SetCompare1(TIM9,tmp);		  // 设置比较值
	    i++;  
	  if(i==2)																																	//中断两次为一个脉冲
		{
		  i=0; 
		  switch(srd4.run_state)
			{
				case STOP:																														//停止状态
					step_count = 0;
					rest = 0;
				    TIM_ClearITPendingBit(TIM9, TIM_IT_CC1);
				    TIM_CCxCmd(TIM9,TIM_Channel_1,TIM_CCx_Disable);					//单个电机可以关闭定时器，多个电机只关闭通道即可
				    TIM_Cmd(TIM9, DISABLE);	
					motor_sta = 0;  
					break;
				
				case ACCEL:																														//加速状态
					step_count++;
					srd4.accel_count++;
					new_step_delay = srd4.step_delay - (((2 *srd4.step_delay) + rest)/(4 * srd4.accel_count + 1));//计算新(下)一步脉冲周期(时间间隔)
					rest = ((2 * srd4.step_delay)+rest)%(4 * srd4.accel_count + 1);					// 计算余数，下次计算补上余数，减少误差
				
					if(step_count >= srd4.decel_start) 																	//检查是够应该开始减速
					{
						srd4.accel_count = srd4.decel_val;																	//加速计数值为减速阶段计数值的初始值
						srd4.run_state = DECEL;																						//下个脉冲进入减速阶段 
					}
					
					else if(new_step_delay <= srd4.min_delay)														//检查是否到达期望的最大速度
					{
						last_accel_delay = new_step_delay;																//保存加速过程中最后一次延时（脉冲周期）
						new_step_delay = srd4.min_delay;   																// 使用min_delay（对应最大速度speed） 
						rest = 0;            																							//清零余值               
						srd4.run_state = RUN;																							//设置为匀速运行状态 
					}
					break;
					
				case RUN:
					  step_count++;  																											// 步数加1				  
					  new_step_delay = srd4.min_delay;   																  // 使用min_delay（对应最大速度speed）				 
					  if(step_count >= srd3.decel_start)   																// 需要开始减速
					  {
						srd4.accel_count = srd4.decel_val;  																// 减速步数做为加速计数值
						new_step_delay = last_accel_delay;																// 加阶段最后的延时做为减速阶段的起始延时(脉冲周期)
						srd4.run_state = DECEL;           																  // 状态改变为减速
					  }
					  break;
					
				case DECEL:
				  step_count++;  																											// 步数加1

				  srd4.accel_count++; 																									// 是个负数
				  new_step_delay = srd4.step_delay - (((2 * srd4.step_delay) + rest)/(4 * srd4.accel_count + 1)); //计算新(下)一步脉冲周期(时间间隔)
				  rest = ((2 * srd4.step_delay)+rest)%(4 * srd4.accel_count + 1);				// 计算余数，下次计算补上余数，减少误差
				  if(srd4.accel_count >= 0) 																						//检查是否为最后一步  是个负数所以要判断 大于等于零时 应该就是减速结束
				  {
					srd4.run_state = STOP;
				  }
				  break;
			}
			 srd4.step_delay = new_step_delay; 																			// 为下个(新的)延时(脉冲周期)赋值
		}
	}
}
// 整车移动量转换为单轮速度  x:前+后-  y:左+右-  z:逆+顺-
void Move_Transfrom(double Vx,double Vy,double Vz)
{
	uint32_t step_accel =25;         // 加速度 单位为0.1rad/sec^2
	uint32_t step_decel = 25;         // 减速度 单位为0.1rad/sec^2
	uint32_t TargetA, TargetB, TargetC, TargetD;
	int16_t Car_H = 0;//常数，随意
	int16_t Car_W = 0;
	TargetA=Vx+Vy-Vz*(Car_H/2+Car_W/2);
	TargetB=Vx-Vy-Vz*(Car_H/2+Car_W/2);
	TargetC=Vx+Vy+Vz*(Car_H/2+Car_W/2);
	TargetD=Vx-Vy+Vz*(Car_H/2+Car_W/2);
	ST1_Move(50 * 1600, step_accel, step_decel, TargetA);//A轮
	ST2_Move(50 * 1600, step_accel, step_decel, TargetB);//B轮
	ST3_Move(50 * 1600, step_accel, step_decel, TargetC);//C轮
	ST4_Move(50 * 1600, step_accel, step_decel, TargetD);//D轮
}

void TIM3_IRQHandler(void)
{
	speed_decision();
}

