#include "allFiles.h"

//TIM13 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
/*********************************************
 *函数：步进电机pwm初始化函数
 *函数名：ST1_PWM_Init(void)
 *备注：
 *日期：2023.08.01
 *修改日期：
 *********************************************/
void ST_PWM_Init(void)
{		 			
	GPIO_InitTypeDef GPIO_InitStructure;
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	GPIO_PinAFConfig(ST1_PWM_GPIO,GPIO_PinSource6,GPIO_AF_TIM3); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4); 
	GPIO_PinAFConfig(ST4_PWM_GPIO,GPIO_PinSource5,GPIO_AF_TIM9);
	GPIO_InitStructure.GPIO_Pin = ST1_PWM_PIN | ST2_PWM_PIN | ST3_PWM_PIN | ST4_PWM_PIN;      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //下拉
	GPIO_Init(GPIOB,&GPIO_InitStructure);         
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM3);
	TIM_InternalClockConfig(TIM2);
	TIM_InternalClockConfig(TIM4);
	TIM_InternalClockConfig(TIM9);
	//TIM3
	TIM_TimeBaseStructure.TIM_Prescaler=84 - 1;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period = 65536-1;	   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//初始化定时器3
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	//TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	//TIM9,10,11
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Prescaler =168 - 1;
	TIM_TimeBaseStructure.TIM_Period = 65536-1; 
	//TIM_TimeBaseInit(TIM11,&TIM_TimeBaseStructure);
	//TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);
	
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle; 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM9, &TIM_OCInitStructure); 
	
	//关闭预装载
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);  
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
	//TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Disable);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Disable);
	
	TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM5,TIM_Channel_3,TIM_CCx_Disable);
//    TIM_ARRPreloadConfig(TIM3,ENABLE);//ARPE使能 
//	TIM_ARRPreloadConfig(TIM11,ENABLE);
//	TIM_ARRPreloadConfig(TIM10,ENABLE);
//	TIM_ARRPreloadConfig(TIM9,ENABLE);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
	TIM_ClearFlag(TIM3, TIM_FLAG_CC2);
	TIM_ClearFlag(TIM4, TIM_FLAG_CC2);
	TIM_ClearFlag(TIM9, TIM_FLAG_CC1);
	TIM_ClearFlag(TIM2, TIM_FLAG_CC2);
	
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM9, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}  
void Motor_Init(void)
{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
//	
//    GPIO_InitStructure.GPIO_Pin = ST1_PWM_PIN | ST1_DIR_GPIO_PIN| ST1_EN_GPIO_PIN;  
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    GPIO_SetBits(GPIOA, ST1_PWM_PIN | ST1_DIR_GPIO_PIN| ST1_EN_GPIO_PIN);	
//	
//	GPIO_InitStructure.GPIO_Pin = ST2_PWM_PIN | ST2_DIR_GPIO_PIN| ST2_EN_GPIO_PIN;  
//	GPIO_Init(GPIOF, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = ST3_PWM_PIN | ST3_DIR_GPIO_PIN| ST3_EN_GPIO_PIN;
//	GPIO_Init(GPIOF, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = ST4_PWM_PIN | ST4_DIR_GPIO_PIN| ST4_EN_GPIO_PIN;
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_InitTypeDef GPIO_InitStructure;
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3); //GF9 复用为 TIM14
	GPIO_PinAFConfig(ST2_PWM_GPIO,GPIO_PinSource7,GPIO_AF_TIM11); 
	GPIO_PinAFConfig(ST3_PWM_GPIO,GPIO_PinSource6,GPIO_AF_TIM10); 
	GPIO_PinAFConfig(ST4_PWM_GPIO,GPIO_PinSource5,GPIO_AF_TIM9);
	
	GPIO_InitStructure.GPIO_Pin = ST1_PWM_PIN| ST2_PWM_PIN | ST3_PWM_PIN | ST4_PWM_PIN; //GPIOF12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //速度 50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化 PF9
	GPIO_Init(GPIOE,&GPIO_InitStructure); //初始化 PF9
	GPIO_Init(GPIOF,&GPIO_InitStructure); //初始化 PF9
	TIM_TimeBaseStructure.TIM_Prescaler=84 - 1; //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=500 - 1; //自动重装载值
	
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//初始化定时器 14
	
	TIM_TimeBaseStructure.TIM_Prescaler=168 - 1; //定时器分频
	TIM_TimeBaseInit(TIM11,&TIM_TimeBaseStructure);//初始化定时器 14
	TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);//初始化定时器 14
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM 调制模式 1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性低
	TIM_OCInitStructure.TIM_Pulse = 50;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure); //初始化外设 TIM1 4OC1
	TIM_OC1Init(TIM11, &TIM_OCInitStructure);
	TIM_OC1Init(TIM10, &TIM_OCInitStructure);
	TIM_OC1Init(TIM9, &TIM_OCInitStructure); 
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); //使能预装载寄存器
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable); //使能预装载寄存器
	TIM_OC1PreloadConfig(TIM11, TIM_OCPreload_Enable); //使能预装载寄存器
	TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable); //使能预装载寄存器
	TIM_ARRPreloadConfig(TIM3,ENABLE);//ARPE 使能
	TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPE 使能
	TIM_ARRPreloadConfig(TIM11,ENABLE);//ARPE 使能
	TIM_ARRPreloadConfig(TIM10,ENABLE);//ARPE 使能
	TIM_Cmd(TIM3, ENABLE); //使能 TIM14
	TIM_Cmd(TIM9, ENABLE); //使能 TIM14
	TIM_Cmd(TIM10, ENABLE); //使能 TIM14
	TIM_Cmd(TIM11, ENABLE); //使能 TIM14
	
}
void SE_Init(u32 arr,u32 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); //TIM14 时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //使能 PORTF 时钟
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4); //GF9 复用为 TIM14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIOF12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //速度 50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化 PF9
	TIM_TimeBaseStructure.TIM_Prescaler=psc; //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr; //自动重装载值
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//初始化定时器 14
	//初始化 TIM14 Channel1 PWM 模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM 调制模式 1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性低
	TIM_OC1Init(TIM4, &TIM_OCInitStructure); //初始化外设 TIM1 4OC1
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); //使能预装载寄存器
	TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPE 使能
	TIM_Cmd(TIM4, ENABLE); //使能 TIM14
}

void PWM_TIM4_Init(void)
{		 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM4 时钟频率84MHz
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 	
			
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //复用推挽
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	 
	GPIO_Init(GPIOD,&GPIO_InitStructure);               
	
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4); //复用GPIOA_Pin0为TIM1_Ch1,
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4); 
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4); 
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
	
	
	TIM_TimeBaseStructure.TIM_Prescaler=42-1;  //PSC
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=10000-1;   //ARR
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //不分频
	//TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;//默认就为0，只有高级定时器有效。
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
 	  
	
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//在空闲时输出     低,这里的设置可以改变TIM_OCPolarity 如果没这句，第1通道有问题
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //PWM1为正常占空比模式，PWM2为反极性模式
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低,有效电平为低电平
 
	TIM_OC1Init(TIM4, &TIM_OCInitStructure); //CH1初始化
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);   
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  
	
	TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPE使能 
	TIM_Cmd(TIM4, ENABLE);  //使能TIM1
	//TIM_CtrlPWMOutputs(TIM2, ENABLE);//使能TIM1的PWM输出，TIM1与TIM8有效,如果没有这行会问题  
}  

void TIM5_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM5); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 	
	GPIO_Init(GPIOA,&GPIO_InitStructure);              
	
	/* 时基 */
	TIM_TimeBaseStructure.TIM_Prescaler=42-1;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=10000 - 1;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;  //设置时钟分割
	//TIM_TimeBaseStructure.TIM_RepetitionCounter=0x00;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);//初始化定时器5(时基)
 
	/* PWM模式 */
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM5输出比较极性低
	//TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM5OC3
	
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);  //使能TIM5在CCR1上的预装载寄存器
	TIM_ARRPreloadConfig(TIM5,ENABLE);//ARPE使能 
	TIM_Cmd(TIM5, ENABLE);  //使能TIM5
}