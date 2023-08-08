#include "allFiles.h"

uint16_t CCR1 = 20000;

speedRampData srd= {STOP,0,0,0,0,0,0};
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
	GPIO_InitStructure.GPIO_Pin = ST1_DIR_GPIO_PIN  | ST1_EN_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉(共阳极接法，0为有效)
    GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化
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

void MOTOR_Move(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
		
    uint16_t tim_count; 																										 //存放中断时刻的计数值
    unsigned int max_s_lim;                                     					 	 //达到最大速度时的步数    
    unsigned int accel_lim;																									 //必须开始减速的步数（如果还没有加速度到最大速度时）

	if(motor_sta!= STOP)  																									 //只允许步进电机在停止的时候才继续
			return;			
    if(step < 0)   								//逆时针
	{
		GPIO_SetBits(ST1_DIR_GPIO_PORT,ST1_DIR_GPIO_PIN);			//输出高电平
		GPIO_SetBits(ST2_DIR_GPIO_PORT,ST2_DIR_GPIO_PIN);
		GPIO_SetBits(ST3_DIR_GPIO_PORT,ST3_DIR_GPIO_PIN);
		GPIO_SetBits(ST4_DIR_GPIO_PORT,ST4_DIR_GPIO_PIN);
		step = -step;      
	}		
	else   			//顺时针
	{
		GPIO_ResetBits(ST1_DIR_GPIO_PORT,ST1_DIR_GPIO_PIN);		//输出低电平		
		GPIO_ResetBits(ST2_DIR_GPIO_PORT,ST2_DIR_GPIO_PIN);	
		GPIO_ResetBits(ST3_DIR_GPIO_PORT,ST3_DIR_GPIO_PIN);	
		GPIO_ResetBits(ST4_DIR_GPIO_PORT,ST4_DIR_GPIO_PIN);	
	}        
    if(step == 1)   																											   // 如果只移动一步
    {       
      srd.accel_count = -1; 																								 // 只移动一步
        
      srd.run_state = DECEL;																								 // 减速状态
        
      srd.step_delay = 1000;																								 // 短延时

     }
    
    else if(step != 0)  																			 								// 步数不为零才移动
    {					
				srd.min_delay = (int32_t)(A_T_x10/speed);							 // 设置最大速度极限, 计算min_delay用于定时器的计数器的值min_delay = (alpha / tt)/ 
				srd.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);		// 通过计算第一个(c0) 的步进延时来设定加速度,其中accel单位为0.01rad/sec^2
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
					srd.decel_val = accel_lim - step;				//减速段的步数
				}
				else
				{
					srd.decel_val = -(max_s_lim*accel/decel);	//减速段的步数 
				}
   
				if(srd.decel_val == 0) 				// 不足一步 按一步处理 
				{
					srd.decel_val = -1;
				}    
				srd.decel_start = step + srd.decel_val;																//计算开始减速时的步数
 
				if(srd.step_delay <= srd.min_delay)																		// 如果一开始c0的速度比匀速段速度还大，就不需要进行加速运动，直接进入匀速
				{
					srd.step_delay = srd.min_delay;
					srd.run_state = RUN;
				}
				else
				{
					srd.run_state = ACCEL;
				}
    
				srd.accel_count = 0;																									// 复位加速度计数值
		
			}
			motor_sta = 1;  																												// 电机为运动状态
			tim_count = TIM_GetCounter(TIM3);																				//获取计数值
			TIM_SetCompare1(TIM3,tim_count+srd.step_delay/2);												//设置定时器比较值 
			TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);																		//使能定时器通道 
			TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Enable);
			TIM_Cmd(TIM3, ENABLE);																									//开启定时器
}
void TIM3_IRQHandler(void)
{
	
}

