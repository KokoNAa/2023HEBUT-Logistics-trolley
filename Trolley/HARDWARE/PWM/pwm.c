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
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	GPIO_PinAFConfig(ST1_PWM_GPIO,GPIO_PinSource8,GPIO_AF_TIM3); 
	GPIO_PinAFConfig(ST2_PWM_GPIO,GPIO_PinSource7,GPIO_AF_TIM11); 
	GPIO_PinAFConfig(ST3_PWM_GPIO,GPIO_PinSource6,GPIO_AF_TIM10); 
	GPIO_PinAFConfig(ST4_PWM_GPIO,GPIO_PinSource5,GPIO_AF_TIM9);
	GPIO_InitStructure.GPIO_Pin = ST1_PWM_PIN | ST2_PWM_PIN | ST3_PWM_PIN | ST4_PWM_PIN;      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOF,&GPIO_InitStructure);         
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM3);
	TIM_InternalClockConfig(TIM11);
	TIM_InternalClockConfig(TIM10);
	TIM_InternalClockConfig(TIM9);
	//TIM3
	TIM_TimeBaseStructure.TIM_Prescaler=TIM_PRESCALER;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period = 65535 - 1;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM13,&TIM_TimeBaseStructure);//初始化定时器13
	//TIM9,10,11
	TIM_TimeBaseStructure.TIM_Prescaler = 1 - 1;
	TIM_TimeBaseStructure.TIM_Period = 65535 - 1;  
	TIM_TimeBaseInit(TIM11,&TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle; //翻转模式
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC1Init(TIM11, &TIM_OCInitStructure);
	TIM_OC1Init(TIM10, &TIM_OCInitStructure);
	TIM_OC1Init(TIM9, &TIM_OCInitStructure); 
	//关闭预装载
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);  
	TIM_OC1PreloadConfig(TIM11, TIM_OCPreload_Disable);
	TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Disable); 
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Disable);
//    TIM_ARRPreloadConfig(TIM13,ENABLE);//ARPE使能 
//	TIM_ARRPreloadConfig(TIM11,ENABLE);
//	TIM_ARRPreloadConfig(TIM10,ENABLE);
//	TIM_ARRPreloadConfig(TIM9,ENABLE);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3, ENABLE);  //使能TIM14		
	TIM_Cmd(TIM11, ENABLE);
	TIM_Cmd(TIM10, ENABLE);
	TIM_Cmd(TIM9, ENABLE);
}  
