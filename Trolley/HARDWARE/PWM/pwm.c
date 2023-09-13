#include "allFiles.h"

//TIM13 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
/*********************************************
 *�������������pwm��ʼ������
 *��������ST1_PWM_Init(void)
 *��ע��
 *���ڣ�2023.08.01
 *�޸����ڣ�
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOB,&GPIO_InitStructure);         
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM3);
	TIM_InternalClockConfig(TIM2);
	TIM_InternalClockConfig(TIM4);
	TIM_InternalClockConfig(TIM9);
	//TIM3
	TIM_TimeBaseStructure.TIM_Prescaler=84 - 1;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = 65536-1;	   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//��ʼ����ʱ��3
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	//TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	//TIM9,10,11
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Prescaler =168 - 1;
	TIM_TimeBaseStructure.TIM_Period = 65536-1; 
	//TIM_TimeBaseInit(TIM11,&TIM_TimeBaseStructure);
	//TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);
	
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle; 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM9, &TIM_OCInitStructure); 
	
	//�ر�Ԥװ��
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);  
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
	//TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Disable);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Disable);
	
	TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM5,TIM_Channel_3,TIM_CCx_Disable);
//    TIM_ARRPreloadConfig(TIM3,ENABLE);//ARPEʹ�� 
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
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3); //GF9 ����Ϊ TIM14
	GPIO_PinAFConfig(ST2_PWM_GPIO,GPIO_PinSource7,GPIO_AF_TIM11); 
	GPIO_PinAFConfig(ST3_PWM_GPIO,GPIO_PinSource6,GPIO_AF_TIM10); 
	GPIO_PinAFConfig(ST4_PWM_GPIO,GPIO_PinSource5,GPIO_AF_TIM9);
	
	GPIO_InitStructure.GPIO_Pin = ST1_PWM_PIN| ST2_PWM_PIN | ST3_PWM_PIN | ST4_PWM_PIN; //GPIOF12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //�ٶ� 50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ�� PF9
	GPIO_Init(GPIOE,&GPIO_InitStructure); //��ʼ�� PF9
	GPIO_Init(GPIOF,&GPIO_InitStructure); //��ʼ�� PF9
	TIM_TimeBaseStructure.TIM_Prescaler=84 - 1; //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=500 - 1; //�Զ���װ��ֵ
	
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//��ʼ����ʱ�� 14
	
	TIM_TimeBaseStructure.TIM_Prescaler=168 - 1; //��ʱ����Ƶ
	TIM_TimeBaseInit(TIM11,&TIM_TimeBaseStructure);//��ʼ����ʱ�� 14
	TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);//��ʼ����ʱ�� 14
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM ����ģʽ 1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //������Ե�
	TIM_OCInitStructure.TIM_Pulse = 50;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure); //��ʼ������ TIM1 4OC1
	TIM_OC1Init(TIM11, &TIM_OCInitStructure);
	TIM_OC1Init(TIM10, &TIM_OCInitStructure);
	TIM_OC1Init(TIM9, &TIM_OCInitStructure); 
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); //ʹ��Ԥװ�ؼĴ���
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable); //ʹ��Ԥװ�ؼĴ���
	TIM_OC1PreloadConfig(TIM11, TIM_OCPreload_Enable); //ʹ��Ԥװ�ؼĴ���
	TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable); //ʹ��Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM3,ENABLE);//ARPE ʹ��
	TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPE ʹ��
	TIM_ARRPreloadConfig(TIM11,ENABLE);//ARPE ʹ��
	TIM_ARRPreloadConfig(TIM10,ENABLE);//ARPE ʹ��
	TIM_Cmd(TIM3, ENABLE); //ʹ�� TIM14
	TIM_Cmd(TIM9, ENABLE); //ʹ�� TIM14
	TIM_Cmd(TIM10, ENABLE); //ʹ�� TIM14
	TIM_Cmd(TIM11, ENABLE); //ʹ�� TIM14
	
}
void SE_Init(u32 arr,u32 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); //TIM14 ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //ʹ�� PORTF ʱ��
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4); //GF9 ����Ϊ TIM14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIOF12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //�ٶ� 50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD,&GPIO_InitStructure); //��ʼ�� PF9
	TIM_TimeBaseStructure.TIM_Prescaler=psc; //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr; //�Զ���װ��ֵ
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//��ʼ����ʱ�� 14
	//��ʼ�� TIM14 Channel1 PWM ģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM ����ģʽ 1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //������Ե�
	TIM_OC1Init(TIM4, &TIM_OCInitStructure); //��ʼ������ TIM1 4OC1
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); //ʹ��Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPE ʹ��
	TIM_Cmd(TIM4, ENABLE); //ʹ�� TIM14
}

void PWM_TIM4_Init(void)
{		 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM4 ʱ��Ƶ��84MHz
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 	
			
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //��������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	 
	GPIO_Init(GPIOD,&GPIO_InitStructure);               
	
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4); //����GPIOA_Pin0ΪTIM1_Ch1,
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4); 
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4); 
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
	
	
	TIM_TimeBaseStructure.TIM_Prescaler=42-1;  //PSC
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=10000-1;   //ARR
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //����Ƶ
	//TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;//Ĭ�Ͼ�Ϊ0��ֻ�и߼���ʱ����Ч��
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
 	  
	
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//�ڿ���ʱ���     ��,��������ÿ��Ըı�TIM_OCPolarity ���û��䣬��1ͨ��������
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //PWM1Ϊ����ռ�ձ�ģʽ��PWM2Ϊ������ģʽ
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ե�,��Ч��ƽΪ�͵�ƽ
 
	TIM_OC1Init(TIM4, &TIM_OCInitStructure); //CH1��ʼ��
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);   
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  
	
	TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPEʹ�� 
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM1
	//TIM_CtrlPWMOutputs(TIM2, ENABLE);//ʹ��TIM1��PWM�����TIM1��TIM8��Ч,���û�����л�����  
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
	
	/* ʱ�� */
	TIM_TimeBaseStructure.TIM_Prescaler=42-1;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=10000 - 1;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;  //����ʱ�ӷָ�
	//TIM_TimeBaseStructure.TIM_RepetitionCounter=0x00;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);//��ʼ����ʱ��5(ʱ��)
 
	/* PWMģʽ */
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM5����Ƚϼ��Ե�
	//TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM5OC3
	
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);  //ʹ��TIM5��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM5,ENABLE);//ARPEʹ�� 
	TIM_Cmd(TIM5, ENABLE);  //ʹ��TIM5
}