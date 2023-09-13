#include "allFiles.h"

uint16_t CCR1 = 20000;

speedRampData srd1= {STOP,0,0,0,0,0,0};
speedRampData srd2= {STOP,0,0,0,0,0,0};
speedRampData srd3= {STOP,0,0,0,0,0,0};
speedRampData srd4= {STOP,0,0,0,0,0,0};

uint8_t  motor_sta        = 0;//���״̬
uint32_t sp = 0; 

/*********************************************
 *�����������������ʹ�����ų�ʼ������
 *��������step_motor_init(void)
 *��ע��
 *���ڣ�2023.08.03
 *�޸����ڣ�
 *********************************************/
void step_motor_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ST1_DIR_GPIO_PIN  | ST2_DIR_GPIO_PIN | ST3_DIR_GPIO_PIN | ST4_DIR_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����(�������ӷ���0Ϊ��Ч)
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = ST1_EN_GPIO_PIN  | ST3_EN_GPIO_PIN | ST4_EN_GPIO_PIN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = ST2_EN_GPIO_PIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
}
/*********************************************
 *��������������˶�����
 *��������MOTOR_Move(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
 *��ע��step   �ƶ�����������Ϊ��ת������Ϊ��ʱ�룩
		accel  ���ٶ�,ʵ��ֵΪaccel*0.1*rad/sec^2  10������2��������һ������������
		decel  ���ٶ�,ʵ��ֵΪdecel*0.1*rad/sec^2
		speed  ����ٶ�,ʵ��ֵΪspeed*0.1*rad/sec
���ڣ�2023.08.03
*�޸����ڣ�
* *********************************************/

void ST1_Move(int32_t step, uint32_t accel, uint32_t decel, int32_t speed)
{
		
    uint16_t tim_count; 																										 //����ж�ʱ�̵ļ���ֵ
    unsigned int max_s_lim;                                     					 	 //�ﵽ����ٶ�ʱ�Ĳ���    
    unsigned int accel_lim;																									 //���뿪ʼ���ٵĲ����������û�м��ٶȵ�����ٶ�ʱ��

	if(motor_sta != STOP)  																									 //ֻ�����������ֹͣ��ʱ��ż���
			return;			
    if(speed < 0)   								//��ʱ��
		{
//			GPIO_SetBits(ST1_DIR_GPIO_PORT,ST1_DIR_GPIO_PIN);			//����ߵ�ƽ
			speed = -speed;      
		}		
//		else   			//˳ʱ��
//		{
//			GPIO_ResetBits(ST1_DIR_GPIO_PORT,ST1_DIR_GPIO_PIN);		//����͵�ƽ		
//		}        
    if(step == 1)   																											   // ���ֻ�ƶ�һ��
    {       
      srd1.accel_count = -1; 																								 // ֻ�ƶ�һ��
        
      srd1.run_state = DECEL;																								 // ����״̬
        
      srd1.step_delay = 1000;																								 // ����ʱ

     }
    
    else if(step != 0)  																			 								// ������Ϊ����ƶ�
    {					
				srd1.min_delay = (int32_t)(A_T_x10/speed);							 // ��������ٶȼ���, ����min_delay���ڶ�ʱ���ļ�������ֵmin_delay = (alpha / tt)/ 
				srd1.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);		// ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶ�,����accel��λΪ0.01rad/sec^2
			 
			    max_s_lim = (uint32_t)(speed*speed/(A_x200*accel/10));   //������ٲ�֮��ﵽ����ٶȵ����� max_s_lim = speed^2 / (2*alpha*accel)
    
				if(max_s_lim == 0)									//����ﵽ����ٶ�С��0.5�������ǽ���������Ϊ0,��ʵ�����Ǳ����ƶ�����һ�����ܴﵽ��Ҫ���ٶ� 
				{
					max_s_lim = 1;
				}    
				accel_lim = (uint32_t)(step*decel/(accel+decel)); 										// ������ٲ�֮�����Ǳ��뿪ʼ����,n1 = (n1+n2)decel / (accel + decel)
   
				if(accel_lim == 0) 																					// ���Ǳ����������1�����ܿ�ʼ����
				{
					accel_lim = 1;
				}
   
				if(accel_lim <= max_s_lim)						//���ٽ׶ε���������ٶȾ͵ü��١�����ʹ�������������ǿ��Լ�������ٽ׶β��� 
				{
					srd1.decel_val = accel_lim - step;				//���ٶεĲ���
				}
				else
				{
					srd1.decel_val = -(max_s_lim*accel/decel);	//���ٶεĲ��� 
				}
   
				if(srd1.decel_val == 0) 				// ����һ�� ��һ������ 
				{
					srd1.decel_val = -1;
				}    
				srd1.decel_start = step + srd1.decel_val;																//���㿪ʼ����ʱ�Ĳ���
 
				if(srd1.step_delay <= srd1.min_delay)																		// ���һ��ʼc0���ٶȱ����ٶ��ٶȻ��󣬾Ͳ���Ҫ���м����˶���ֱ�ӽ�������
				{
					srd1.step_delay = srd1.min_delay;
					srd1.run_state = RUN;
				}
				else
				{
					srd1.run_state = ACCEL;
				}
    
				srd1.accel_count = 0;																									// ��λ���ٶȼ���ֵ
		
			}
			motor_sta = 1;  																												// ���Ϊ�˶�״̬
			tim_count = TIM_GetCounter(TIM3);																				//��ȡ����ֵ
			TIM_SetCompare1(TIM3,tim_count+srd1.step_delay/2);	 											//���ö�ʱ���Ƚ�ֵ 
			TIM_SetCompare2(TIM3,tim_count+srd1.step_delay/2);
			TIM_SetCompare3(TIM3,tim_count+srd1.step_delay/2);
			TIM_SetCompare4(TIM3,tim_count+srd1.step_delay/2);
			TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
			TIM_ITConfig(TIM3,TIM_IT_CC2,ENABLE);
			TIM_ITConfig(TIM3,TIM_IT_CC3,ENABLE);
			TIM_ITConfig(TIM3,TIM_IT_CC4,ENABLE);
			TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Enable);
			TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Enable);
			if (out_ku == 0)
			{
				TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Disable);
				TIM_CCxCmd(TIM3,TIM_Channel_4,TIM_CCx_Disable);
				out_ku = 1;
			}
			else
			{
				TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Enable);
				TIM_CCxCmd(TIM3,TIM_Channel_4,TIM_CCx_Enable);
			}
			TIM_Cmd(TIM3, ENABLE);
}
void ST2_Move(int32_t step, uint32_t accel, uint32_t decel, int32_t speed)
{
		
    uint16_t tim_count; 																										 //����ж�ʱ�̵ļ���ֵ
    unsigned int max_s_lim;                                     					 	 //�ﵽ����ٶ�ʱ�Ĳ���    
    unsigned int accel_lim;																									 //���뿪ʼ���ٵĲ����������û�м��ٶȵ�����ٶ�ʱ��

	if(motor_sta != STOP)  																									 //ֻ�����������ֹͣ��ʱ��ż���
			return;			
	//GPIO_ResetBits(ST1_EN_GPIO_PORT,ST1_EN_GPIO_PIN);
    if(speed < 0)   								//��ʱ��
	{
		GPIO_SetBits(ST2_DIR_GPIO_PORT,ST2_DIR_GPIO_PIN);			//����ߵ�ƽ
		speed = -speed;      
	}		
	else   			//˳ʱ��
	{
		GPIO_ResetBits(ST2_DIR_GPIO_PORT,ST2_DIR_GPIO_PIN);		//����͵�ƽ		
	}        
    if(step == 1)   																											   // ���ֻ�ƶ�һ��
    {       
      srd2.accel_count = -1; 																								 // ֻ�ƶ�һ��
        
      srd2.run_state = DECEL;																								 // ����״̬
        
      srd2.step_delay = 1000;																								 // ����ʱ

     }
    
    else if(step != 0)  																			 								// ������Ϊ����ƶ�
    {					
				srd2.min_delay = (int32_t)(A_T_x10/speed);							 // ��������ٶȼ���, ����min_delay���ڶ�ʱ���ļ�������ֵmin_delay = (alpha / tt)/ 
				srd2.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);		// ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶ�,����accel��λΪ0.01rad/sec^2
					// step_delay = 1/tt * sqrt(2*alpha/accel)
				// step_delay = ( tfreq*0.69/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 
			    max_s_lim = (uint32_t)(speed*speed/(A_x200*accel/10));   //������ٲ�֮��ﵽ����ٶȵ����� max_s_lim = speed^2 / (2*alpha*accel)
    
				if(max_s_lim == 0)									//����ﵽ����ٶ�С��0.5�������ǽ���������Ϊ0,��ʵ�����Ǳ����ƶ�����һ�����ܴﵽ��Ҫ���ٶ� 
				{
					max_s_lim = 1;
				}    
				accel_lim = (uint32_t)(step*decel/(accel+decel)); 										// ������ٲ�֮�����Ǳ��뿪ʼ����,n1 = (n1+n2)decel / (accel + decel)
   
				if(accel_lim == 0) 																					// ���Ǳ����������1�����ܿ�ʼ����
				{
					accel_lim = 1;
				}
   
				if(accel_lim <= max_s_lim)						//���ٽ׶ε���������ٶȾ͵ü��١�����ʹ�������������ǿ��Լ�������ٽ׶β��� 
				{
					srd2.decel_val = accel_lim - step;				//���ٶεĲ���
				}
				else
				{
					srd2.decel_val = -(max_s_lim*accel/decel);	//���ٶεĲ��� 
				}
   
				if(srd2.decel_val == 0) 				// ����һ�� ��һ������ 
				{
					srd2.decel_val = -1;
				}    
				srd2.decel_start = step + srd2.decel_val;																//���㿪ʼ����ʱ�Ĳ���
 
				if(srd2.step_delay <= srd2.min_delay)																		// ���һ��ʼc0���ٶȱ����ٶ��ٶȻ��󣬾Ͳ���Ҫ���м����˶���ֱ�ӽ�������
				{
					srd2.step_delay = srd2.min_delay;
					srd2.run_state = RUN;
				}
				else
				{
					srd2.run_state = ACCEL;
				}
    
				srd2.accel_count = 0;																									// ��λ���ٶȼ���ֵ
		
			}
			motor_sta = 1;  																												// ���Ϊ�˶�״̬
			tim_count = TIM_GetCounter(TIM3);																				//��ȡ����ֵ
			TIM_SetCompare2(TIM3,tim_count+srd1.step_delay/2);												//���ö�ʱ���Ƚ�ֵ 
			TIM_ITConfig(TIM3,TIM_IT_CC2,ENABLE);
			TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Enable);
			TIM_Cmd(TIM3, ENABLE);
			
}
void ST3_Move(int32_t step, uint32_t accel, uint32_t decel, int32_t speed)
{
		
    uint16_t tim_count; 																										 //����ж�ʱ�̵ļ���ֵ
    unsigned int max_s_lim;                                     					 	 //�ﵽ����ٶ�ʱ�Ĳ���    
    unsigned int accel_lim;																									 //���뿪ʼ���ٵĲ����������û�м��ٶȵ�����ٶ�ʱ��

	if(motor_sta!= STOP)  																									 //ֻ�����������ֹͣ��ʱ��ż���
			return;			
    if(speed < 0)   								//��ʱ��
	{
		GPIO_SetBits(ST3_DIR_GPIO_PORT,ST3_DIR_GPIO_PIN);			//����ߵ�ƽ
		speed = -speed;      
	}		
	else   			//˳ʱ��
	{
		GPIO_ResetBits(ST3_DIR_GPIO_PORT,ST3_DIR_GPIO_PIN);		//����͵�ƽ		
	}        
    if(step == 1)   																											   // ���ֻ�ƶ�һ��
    {       
      srd3.accel_count = -1; 																								 // ֻ�ƶ�һ��
        
      srd3.run_state = DECEL;																								 // ����״̬
        
      srd3.step_delay = 1000;																								 // ����ʱ

     }
    
    else if(step != 0)  																			 								// ������Ϊ����ƶ�
    {					
				srd3.min_delay = (int32_t)(A_T_x10/speed);							 // ��������ٶȼ���, ����min_delay���ڶ�ʱ���ļ�������ֵmin_delay = (alpha / tt)/ 
				srd3.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);		// ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶ�,����accel��λΪ0.01rad/sec^2
					// step_delay = 1/tt * sqrt(2*alpha/accel)
				// step_delay = ( tfreq*0.69/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 
			    max_s_lim = (uint32_t)(speed*speed/(A_x200*accel/10));   //������ٲ�֮��ﵽ����ٶȵ����� max_s_lim = speed^2 / (2*alpha*accel)
    
				if(max_s_lim == 0)									//����ﵽ����ٶ�С��0.5�������ǽ���������Ϊ0,��ʵ�����Ǳ����ƶ�����һ�����ܴﵽ��Ҫ���ٶ� 
				{
					max_s_lim = 1;
				}    
				accel_lim = (uint32_t)(step*decel/(accel+decel)); 										// ������ٲ�֮�����Ǳ��뿪ʼ����,n1 = (n1+n2)decel / (accel + decel)
   
				if(accel_lim == 0) 																					// ���Ǳ����������1�����ܿ�ʼ����
				{
					accel_lim = 1;
				}
   
				if(accel_lim <= max_s_lim)						//���ٽ׶ε���������ٶȾ͵ü��١�����ʹ�������������ǿ��Լ�������ٽ׶β��� 
				{
					srd3.decel_val = accel_lim - step;				//���ٶεĲ���
				}
				else
				{
					srd3.decel_val = -(max_s_lim*accel/decel);	//���ٶεĲ��� 
				}
   
				if(srd3.decel_val == 0) 				// ����һ�� ��һ������ 
				{
					srd3.decel_val = -1;
				}    
				srd3.decel_start = step + srd3.decel_val;																//���㿪ʼ����ʱ�Ĳ���
 
				if(srd3.step_delay <= srd3.min_delay)				// ���һ��ʼc0���ٶȱ����ٶ��ٶȻ��󣬾Ͳ���Ҫ���м����˶���ֱ�ӽ�������
				{
					srd3.step_delay = srd3.min_delay;
					srd3.run_state = RUN;
				}
				else
				{
					srd3.run_state = ACCEL;
				}
    
				srd3.accel_count = 0;																									// ��λ���ٶȼ���ֵ
		
			}
			motor_sta = 1;  																												// ���Ϊ�˶�״̬
			tim_count = TIM_GetCounter(TIM3);																				//��ȡ����ֵ
			TIM_SetCompare2(TIM3,tim_count+srd3.step_delay/2);												//���ö�ʱ���Ƚ�ֵ 
			TIM_ITConfig(TIM3,TIM_IT_CC2,ENABLE);																		//ʹ�ܶ�ʱ��ͨ�� 
			TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Enable);
			TIM_Cmd(TIM3, ENABLE);			//������ʱ��
			//LED0=!LED0;
}
//void ST4_Move(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
//{
//		
//    uint16_t tim_count; 																										 //����ж�ʱ�̵ļ���ֵ
//    unsigned int max_s_lim;                                     					 	 //�ﵽ����ٶ�ʱ�Ĳ���    
//    unsigned int accel_lim;																									 //���뿪ʼ���ٵĲ����������û�м��ٶȵ�����ٶ�ʱ��

//	if(motor_sta!= STOP)  																									 //ֻ�����������ֹͣ��ʱ��ż���
//			return;			
//    if(step < 0)   								//��ʱ��
//	{
//		GPIO_SetBits(ST4_DIR_GPIO_PORT,ST4_DIR_GPIO_PIN);			//����ߵ�ƽ
//		step = -step;      
//	}		
//	else   			//˳ʱ��
//	{
//		GPIO_ResetBits(ST4_DIR_GPIO_PORT,ST4_DIR_GPIO_PIN);		//����͵�ƽ		
//	}        
//    if(step == 1)   																											   // ���ֻ�ƶ�һ��
//    {       
//      srd4.accel_count = -1; 																								 // ֻ�ƶ�һ��
//        
//      srd4.run_state = DECEL;																								 // ����״̬
//        
//      srd4.step_delay = 1000;																								 // ����ʱ

//     }
//    
//    else if(step != 0)  																			 								// ������Ϊ����ƶ�
//    {					
//				srd4.min_delay = (int32_t)(A_T_x10/speed);							 // ��������ٶȼ���, ����min_delay���ڶ�ʱ���ļ�������ֵmin_delay = (alpha / tt)/ 
//				srd4.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);		// ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶ�,����accel��λΪ0.01rad/sec^2
//					// step_delay = 1/tt * sqrt(2*alpha/accel)
//				// step_delay = ( tfreq*0.69/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 
//			    max_s_lim = (uint32_t)(speed*speed/(A_x200*accel/10));   //������ٲ�֮��ﵽ����ٶȵ����� max_s_lim = speed^2 / (2*alpha*accel)
//    
//				if(max_s_lim == 0)									//����ﵽ����ٶ�С��0.5�������ǽ���������Ϊ0,��ʵ�����Ǳ����ƶ�����һ�����ܴﵽ��Ҫ���ٶ� 
//				{
//					max_s_lim = 1;
//				}    
//				accel_lim = (uint32_t)(step*decel/(accel+decel)); 										// ������ٲ�֮�����Ǳ��뿪ʼ����,n1 = (n1+n2)decel / (accel + decel)
//   
//				if(accel_lim == 0) 																					// ���Ǳ����������1�����ܿ�ʼ����
//				{
//					accel_lim = 1;
//				}
//   
//				if(accel_lim <= max_s_lim)						//���ٽ׶ε���������ٶȾ͵ü��١�����ʹ�������������ǿ��Լ�������ٽ׶β��� 
//				{
//					srd4.decel_val = accel_lim - step;				//���ٶεĲ���
//				}
//				else
//				{
//					srd4.decel_val = -(max_s_lim*accel/decel);	//���ٶεĲ��� 
//				}
//   
//				if(srd4.decel_val == 0) 				// ����һ�� ��һ������ 
//				{
//					srd4.decel_val = -1;
//				}    
//				srd4.decel_start = step + srd4.decel_val;																//���㿪ʼ����ʱ�Ĳ���
// 
//				if(srd4.step_delay <= srd4.min_delay)				// ���һ��ʼc0���ٶȱ����ٶ��ٶȻ��󣬾Ͳ���Ҫ���м����˶���ֱ�ӽ�������
//				{
//					srd4.step_delay = srd4.min_delay;
//					srd4.run_state = RUN;
//				}
//				else
//				{
//					srd4.run_state = ACCEL;
//				}
//    
//				srd4.accel_count = 0;																									// ��λ���ٶȼ���ֵ
//		
//			}
//			motor_sta = 1;  																												// ���Ϊ�˶�״̬
//			tim_count = TIM_GetCounter(TIM9);																				//��ȡ����ֵ
//			TIM_SetCompare1(TIM9,tim_count+srd4.step_delay/2);												//���ö�ʱ���Ƚ�ֵ 
//			TIM_ITConfig(TIM9,TIM_IT_CC1,ENABLE);																		//ʹ�ܶ�ʱ��ͨ�� 
//			TIM_CCxCmd(TIM9,TIM_Channel_1,TIM_CCx_Enable);
//			TIM_Cmd(TIM9, ENABLE);																									//������ʱ��
//}
void speed1_decision()                                                         //�ж�ִ�к���
{
	__IO uint32_t tim_count=0;
	__IO uint32_t tmp = 0;  
  uint16_t new_step_delay=0;                                                  // �����£��£�һ����ʱ����  
  __IO static uint16_t st1_last_accel_delay=0;                                    // ���ٹ��������һ����ʱ���������ڣ�. 
  __IO static uint32_t st1_step_count = 0; 																			  // ���ƶ�����������  
  __IO static int32_t st1_rest = 0;																								// ��¼new_step_delay�е������������һ������ľ���  
  __IO static uint8_t i=0;		
	
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1)== SET)
  {	
	  LED1=!LED1;
      TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);																	// �嶨ʱ���ж�	
	  tim_count = TIM_GetCounter(TIM3);																					//��ȡ����ֵ
	  tmp = tim_count+srd1.step_delay/2;
		sp = tmp;
	  TIM_SetCompare1(TIM3,tmp);		  // ���ñȽ�ֵ
		TIM_SetCompare2(TIM3,tmp);
		TIM_SetCompare3(TIM3,tmp);
		TIM_SetCompare4(TIM3,tmp);
	  // TIM_CCxCmd(TIM11,TIM_Channel_1,TIM_CCx_Enable);					//����������Թرն�ʱ����������ֻ�ر�ͨ������
	   //TIM_Cmd(TIM11, ENABLE);	
	  //TIM_SetCompare1(TIM11,tmp);
	  i++;  
	  if(i==2)																																	//�ж�����Ϊһ������
		{
		  i=0; 
		  switch(srd1.run_state)
			{
				case STOP:																														//ֹͣ״̬
					st1_step_count = 0;
					st1_rest = 0;
				  TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
					TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
					TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
					TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
				  TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Disable);					//����������Թرն�ʱ����������ֻ�ر�ͨ������
				  TIM_Cmd(TIM3, DISABLE);	
				  TIM_CCxCmd(TIM11,TIM_Channel_1,TIM_CCx_Disable);					//����������Թرն�ʱ����������ֻ�ر�ͨ������
				  TIM_Cmd(TIM11, DISABLE);	
					motor_sta = 0;  
					break;
				
				case ACCEL:																														//����״̬
				st1_step_count++;
				srd1.accel_count++;
				new_step_delay = srd1.step_delay - (((2 *srd1.step_delay) + st1_rest)/(4 * srd1.accel_count + 1));//������(��)һ����������(ʱ����)
				st1_rest = ((2 * srd1.step_delay)+st1_rest)%(4 * srd1.accel_count + 1);					// �����������´μ��㲹���������������
				
					if(st1_step_count >= srd1.decel_start) 																	//����ǹ�Ӧ�ÿ�ʼ����
					{
						srd1.accel_count = srd1.decel_val;																	//���ټ���ֵΪ���ٽ׶μ���ֵ�ĳ�ʼֵ
						srd1.run_state = DECEL;																						//�¸����������ٽ׶� 
					}
					
					else if(new_step_delay <= srd1.min_delay)														//����Ƿ񵽴�����������ٶ�
					{
						st1_last_accel_delay = new_step_delay;																//������ٹ��������һ����ʱ���������ڣ�
						new_step_delay = srd1.min_delay;   																// ʹ��min_delay����Ӧ����ٶ�speed�� 
						st1_rest = 0;            																							//������ֵ               
						srd1.run_state = RUN;																							//����Ϊ��������״̬ 
					}
					break;
					
				case RUN:
          st1_step_count++; 				// ������1	
//		  if (srd1.min_delay > new_step_delay)//�����ٶȱȵ�ǰ��
//		  {
//		      srd1.run_state = ACCEL; 
//			  new_step_delay = srd1.min_delay;   
//			  break;
//		  }
//		  else if (srd1.min_delay < new_step_delay)
//		  {
//			  
//			  break;
//		  }
          new_step_delay = srd1.min_delay;   																  // ʹ��min_delay����Ӧ����ٶ�speed��				 
          if(st1_step_count >= srd1.decel_start)   																// ��Ҫ��ʼ����
		  {
            srd1.accel_count = srd1.decel_val;  																// ���ٲ�����Ϊ���ټ���ֵ
            new_step_delay = st1_last_accel_delay;																// �ӽ׶�������ʱ��Ϊ���ٽ׶ε���ʼ��ʱ(��������)
            srd1.run_state = DECEL;           																  // ״̬�ı�Ϊ����
          }
          break;
					
				case DECEL:
          st1_step_count++;  																											// ������1

          srd1.accel_count++; 																									// �Ǹ�����
          new_step_delay = srd1.step_delay - (((2 * srd1.step_delay) + st1_rest)/(4 * srd1.accel_count + 1)); //������(��)һ����������(ʱ����)
          st1_rest = ((2 * srd1.step_delay)+st1_rest)%(4 * srd1.accel_count + 1);				// �����������´μ��㲹���������������
          if(srd1.accel_count >= 0) 																						//����Ƿ�Ϊ���һ��  �Ǹ���������Ҫ�ж� ���ڵ�����ʱ Ӧ�þ��Ǽ��ٽ���
          {
            srd1.run_state = STOP;
          }
          break;
			}
			 srd1.step_delay = new_step_delay; 																			// Ϊ�¸�(�µ�)��ʱ(��������)��ֵ
		}
	}
}

void speed2_decision()                                                         //�ж�ִ�к���
{
	__IO uint32_t tim_count=0;
	__IO uint32_t tmp = 0;  
  uint16_t new_step_delay=0;                                                  // �����£��£�һ����ʱ����  
  __IO static uint16_t last_accel_delay=0;                                    // ���ٹ��������һ����ʱ���������ڣ�. 
  __IO static uint32_t step_count = 0; 																			  // ���ƶ�����������  
  __IO static int32_t rest = 0;																								// ��¼new_step_delay�е������������һ������ľ���  
  __IO static uint8_t i=0;																						//��ʱ��ʹ�÷�תģʽ����Ҫ���������жϲ����һ����������
	
    if (TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET)// ST2���ж�
	{
		LED0=!LED0;//DS0��ת
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);																	// �嶨ʱ���ж�		
	    tim_count = TIM_GetCounter(TIM3);																					//��ȡ����ֵ
	    tmp = tim_count+srd2.step_delay/2;
		sp = tmp;
	    TIM_SetCompare2(TIM3,tmp);		  // ���ñȽ�ֵ
	    i++;  
	  if(i==2)																																	//�ж�����Ϊһ������
		{
		  i=0; 
		  switch(srd2.run_state)
			{
				case STOP:																														//ֹͣ״̬
					step_count = 0;
					rest = 0;
				    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
				    TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Disable);					//����������Թرն�ʱ����������ֻ�ر�ͨ������
					motor_sta = 0;  
					break;
				
				case ACCEL:																														//����״̬
					step_count++;
					srd2.accel_count++;
					new_step_delay = srd2.step_delay - (((2 *srd2.step_delay) + rest)/(4 * srd2.accel_count + 1));//������(��)һ����������(ʱ����)
					rest = ((2 * srd2.step_delay)+rest)%(4 * srd2.accel_count + 1);					// �����������´μ��㲹���������������
				
					if(step_count >= srd2.decel_start) 																	//����ǹ�Ӧ�ÿ�ʼ����
					{
						srd2.accel_count = srd2.decel_val;																	//���ټ���ֵΪ���ٽ׶μ���ֵ�ĳ�ʼֵ
						srd2.run_state = DECEL;																						//�¸����������ٽ׶� 
					}
					
					else if(new_step_delay <= srd2.min_delay)														//����Ƿ񵽴�����������ٶ�
					{
						last_accel_delay = new_step_delay;																//������ٹ��������һ����ʱ���������ڣ�
						new_step_delay = srd2.min_delay;   																// ʹ��min_delay����Ӧ����ٶ�speed�� 
						rest = 0;            																							//������ֵ               
						srd2.run_state = RUN;																							//����Ϊ��������״̬ 
					}
					break;
					
				case RUN:
					  step_count++;  																											// ������1				  
					  new_step_delay = srd2.min_delay;   																  // ʹ��min_delay����Ӧ����ٶ�speed��				 
					  if(step_count >= srd2.decel_start)   																// ��Ҫ��ʼ����
					  {
						srd2.accel_count = srd2.decel_val;  																// ���ٲ�����Ϊ���ټ���ֵ
						new_step_delay = last_accel_delay;																// �ӽ׶�������ʱ��Ϊ���ٽ׶ε���ʼ��ʱ(��������)
						srd2.run_state = DECEL;           																  // ״̬�ı�Ϊ����
					  }
					  break;
					
				case DECEL:
				  step_count++;  																											// ������1

				  srd2.accel_count++; 																									// �Ǹ�����
				  new_step_delay = srd2.step_delay - (((2 * srd2.step_delay) + rest)/(4 * srd2.accel_count + 1)); //������(��)һ����������(ʱ����)
				  rest = ((2 * srd2.step_delay)+rest)%(4 * srd2.accel_count + 1);				// �����������´μ��㲹���������������
				  if(srd2.accel_count >= 0) 																						//����Ƿ�Ϊ���һ��  �Ǹ���������Ҫ�ж� ���ڵ�����ʱ Ӧ�þ��Ǽ��ٽ���
				  {
					srd2.run_state = STOP;
				  }
				  break;
			}
			 srd2.step_delay = new_step_delay; 																			// Ϊ�¸�(�µ�)��ʱ(��������)��ֵ
		}
		
	}
}
void speed3_decision()                                                         //�ж�ִ�к���
{
	__IO uint32_t tim_count=0;
	__IO uint32_t tmp = 0;  
  uint16_t new_step_delay=0;                                                  // �����£��£�һ����ʱ����  
  __IO static uint16_t last_accel_delay=0;                                    // ���ٹ��������һ����ʱ���������ڣ�. 
  __IO static uint32_t step_count = 0; 																			  // ���ƶ�����������  
  __IO static int32_t rest = 0;																								// ��¼new_step_delay�е������������һ������ľ���  
  __IO static uint8_t i=0;																										//��ʱ��ʹ�÷�תģʽ����Ҫ���������жϲ����һ����������
 
	if (TIM_GetITStatus(TIM3, TIM_IT_CC3) == SET)// ST3���ж�
	{
		LED0=!LED0;
		LED1=!LED1;
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);																	// �嶨ʱ���ж�		
	    tim_count = TIM_GetCounter(TIM3);																					//��ȡ����ֵ
	    tmp = tim_count+srd3.step_delay/2;
	    TIM_SetCompare2(TIM3,tmp);		  // ���ñȽ�ֵ
	    i++;  
	  if(i==2)																																	//�ж�����Ϊһ������
		{
		  i=0; 
		  switch(srd3.run_state)
			{
				case STOP:																														//ֹͣ״̬
					step_count = 0;
					rest = 0;
				    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
				    TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Disable);					//����������Թرն�ʱ����������ֻ�ر�ͨ������
					motor_sta = 0;  
					break;
				
				case ACCEL:																														//����״̬
					step_count++;
					srd3.accel_count++;
					new_step_delay = srd3.step_delay - (((2 *srd3.step_delay) + rest)/(4 * srd3.accel_count + 1));//������(��)һ����������(ʱ����)
					rest = ((2 * srd3.step_delay)+rest)%(4 * srd3.accel_count + 1);					// �����������´μ��㲹���������������
				
					if(step_count >= srd3.decel_start) 																	//����ǹ�Ӧ�ÿ�ʼ����
					{
						srd3.accel_count = srd3.decel_val;																	//���ټ���ֵΪ���ٽ׶μ���ֵ�ĳ�ʼֵ
						srd3.run_state = DECEL;																						//�¸����������ٽ׶� 
					}
					
					else if(new_step_delay <= srd3.min_delay)														//����Ƿ񵽴�����������ٶ�
					{
						last_accel_delay = new_step_delay;																//������ٹ��������һ����ʱ���������ڣ�
						new_step_delay = srd3.min_delay;   																// ʹ��min_delay����Ӧ����ٶ�speed�� 
						rest = 0;            																							//������ֵ               
						srd3.run_state = RUN;																							//����Ϊ��������״̬ 
					}
					break;
					
				case RUN:
					  step_count++;  																											// ������1				  
					  new_step_delay = srd3.min_delay;   																  // ʹ��min_delay����Ӧ����ٶ�speed��				 
					  if(step_count >= srd3.decel_start)   																// ��Ҫ��ʼ����
					  {
						srd3.accel_count = srd3.decel_val;  																// ���ٲ�����Ϊ���ټ���ֵ
						new_step_delay = last_accel_delay;																// �ӽ׶�������ʱ��Ϊ���ٽ׶ε���ʼ��ʱ(��������)
						srd3.run_state = DECEL;           																  // ״̬�ı�Ϊ����
					  }
					  break;
					
				case DECEL:
				  step_count++;  																											// ������1

				  srd3.accel_count++; 																									// �Ǹ�����
				  new_step_delay = srd3.step_delay - (((2 * srd3.step_delay) + rest)/(4 * srd3.accel_count + 1)); //������(��)һ����������(ʱ����)
				  rest = ((2 * srd3.step_delay)+rest)%(4 * srd3.accel_count + 1);				// �����������´μ��㲹���������������
				  if(srd3.accel_count >= 0) 																						//����Ƿ�Ϊ���һ��  �Ǹ���������Ҫ�ж� ���ڵ�����ʱ Ӧ�þ��Ǽ��ٽ���
				  {
					srd3.run_state = STOP;
				  }
				  break;
			}
			 srd3.step_delay = new_step_delay; 																			// Ϊ�¸�(�µ�)��ʱ(��������)��ֵ
		}
	}
}
void speed4_decision()                                                         //�ж�ִ�к���
{
	__IO uint32_t tim_count=0;
	__IO uint32_t tmp = 0;  
  uint16_t new_step_delay=0;                                                  // �����£��£�һ����ʱ����  
  __IO uint16_t last_accel_delay=0;                                    // ���ٹ��������һ����ʱ���������ڣ�. 
  __IO uint32_t step_count = 0; 																			  // ���ƶ�����������  
  __IO int32_t rest = 0;																								// ��¼new_step_delay�е������������һ������ľ���  
  __IO uint8_t i=0;																										//��ʱ��ʹ�÷�תģʽ����Ҫ���������жϲ����һ����������
 
	if (TIM_GetITStatus(TIM3, TIM_IT_CC4)== SET)// ST4���ж�
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);																	// �嶨ʱ���ж�		
	    tim_count = TIM_GetCounter(TIM3);																					//��ȡ����ֵ
	    tmp = tim_count+srd4.step_delay/2;
	    TIM_SetCompare4(TIM3,tmp);		  // ���ñȽ�ֵ
	    i++;  
	  if(i==2)																																	//�ж�����Ϊһ������
		{
		  i=0; 
		  switch(srd4.run_state)
			{
				case STOP:																														//ֹͣ״̬
					step_count = 0;
					rest = 0;
				    TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
				    TIM_CCxCmd(TIM3,TIM_Channel_4,TIM_CCx_Disable);					//����������Թرն�ʱ����������ֻ�ر�ͨ������
					motor_sta = 0;  
					break;
				
				case ACCEL:																														//����״̬
					step_count++;
					srd4.accel_count++;
					new_step_delay = srd4.step_delay - (((2 *srd4.step_delay) + rest)/(4 * srd4.accel_count + 1));//������(��)һ����������(ʱ����)
					rest = ((2 * srd4.step_delay)+rest)%(4 * srd4.accel_count + 1);					// �����������´μ��㲹���������������
				
					if(step_count >= srd4.decel_start) 																	//����ǹ�Ӧ�ÿ�ʼ����
					{
						srd4.accel_count = srd4.decel_val;																	//���ټ���ֵΪ���ٽ׶μ���ֵ�ĳ�ʼֵ
						srd4.run_state = DECEL;																						//�¸����������ٽ׶� 
					}
					
					else if(new_step_delay <= srd4.min_delay)														//����Ƿ񵽴�����������ٶ�
					{
						last_accel_delay = new_step_delay;																//������ٹ��������һ����ʱ���������ڣ�
						new_step_delay = srd4.min_delay;   																// ʹ��min_delay����Ӧ����ٶ�speed�� 
						rest = 0;            																							//������ֵ               
						srd4.run_state = RUN;																							//����Ϊ��������״̬ 
					}
					break;
					
				case RUN:
					  step_count++;  																											// ������1				  
					  new_step_delay = srd4.min_delay;   																  // ʹ��min_delay����Ӧ����ٶ�speed��				 
					  if(step_count >= srd3.decel_start)   																// ��Ҫ��ʼ����
					  {
						srd4.accel_count = srd4.decel_val;  																// ���ٲ�����Ϊ���ټ���ֵ
						new_step_delay = last_accel_delay;																// �ӽ׶�������ʱ��Ϊ���ٽ׶ε���ʼ��ʱ(��������)
						srd4.run_state = DECEL;           																  // ״̬�ı�Ϊ����
					  }
					  break;
				case DECEL:
				  step_count++;  																											// ������1

				  srd4.accel_count++; 																									// �Ǹ�����
				  new_step_delay = srd4.step_delay - (((2 * srd4.step_delay) + rest)/(4 * srd4.accel_count + 1)); //������(��)һ����������(ʱ����)
				  rest = ((2 * srd4.step_delay)+rest)%(4 * srd4.accel_count + 1);				// �����������´μ��㲹���������������
				  if(srd4.accel_count >= 0) 																						//����Ƿ�Ϊ���һ��  �Ǹ���������Ҫ�ж� ���ڵ�����ʱ Ӧ�þ��Ǽ��ٽ���
				  {
					srd4.run_state = STOP;
				  }
				  break;
			}
			 srd4.step_delay = new_step_delay; 																			// Ϊ�¸�(�µ�)��ʱ(��������)��ֵ
		}
	}
}
/*********************************************
 *�����������ķ���˶�ѧ���㺯��
 *��������Move_Transfrom(double Vx,double Vy,double Vz)
*��ע:�����ƶ���ת��Ϊ�����ٶ�  x:ǰ+��-  y:��+��-  z:��+˳-
 *���ڣ�2023.08.10
 *�޸����ڣ�
 *********************************************/
void Move_Transfrom(int16_t Vx,int16_t Vy,int16_t Vz,uint32_t step)
{
	uint32_t step_accel =300;         // ���ٶ� ��λΪ0.1rad/sec^2
	uint32_t step_decel = 250;         // ���ٶ� ��λΪ0.1rad/sec^2
	int32_t TargetA, TargetB, TargetC, TargetD;
	int16_t Car_H = 21;//������������
	int16_t Car_W = 21;
	int16_t dir1 = 0, dir2 = 0, dir3 = 0, dir4 = 0;
	int i = 0;
	TargetA=Vx+Vy-Vz*(Car_H/2+Car_W/2);
	TargetB=Vx-Vy-Vz*(Car_H/2+Car_W/2);
	TargetC=Vx+Vy+Vz*(Car_H/2+Car_W/2);
	TargetD=Vx-Vy+Vz*(Car_H/2+Car_W/2);
	if (TargetA < 0)
	{
		GPIO_SetBits(ST1_DIR_GPIO_PORT,ST1_DIR_GPIO_PIN);
		TargetA  = -TargetA;
	}
	else
	{
		GPIO_ResetBits(ST1_DIR_GPIO_PORT,ST1_DIR_GPIO_PIN);
	}
	if(TargetB < 0)
	{
		GPIO_SetBits(ST2_DIR_GPIO_PORT,ST2_DIR_GPIO_PIN);
		TargetB  = -TargetB;
	}
	else
	{
		GPIO_ResetBits(ST2_DIR_GPIO_PORT,ST2_DIR_GPIO_PIN);
	}
	if (TargetC < 0)
	{
		GPIO_ResetBits(ST3_DIR_GPIO_PORT,ST3_DIR_GPIO_PIN);
		TargetC  = -TargetC;
	}
	else
	{
		GPIO_SetBits(ST3_DIR_GPIO_PORT,ST3_DIR_GPIO_PIN);
	}
	if (TargetD < 0)
	{
		GPIO_ResetBits(ST4_DIR_GPIO_PORT,ST4_DIR_GPIO_PIN);
		TargetD  = -TargetD;
	}
	else
	{
		GPIO_SetBits(ST4_DIR_GPIO_PORT,ST4_DIR_GPIO_PIN);
	}
	
//	Motor1_Run(dir1,step,TargetA);
//	Motor2_Run(dir2,step,TargetB);
//	Motor3_Run(dir3,step,TargetC);
//	Motor3_Run(dir4,step,TargetD);
	ST1_Move(step, step_accel, step_decel, TargetA);
//	ST2_Move(step, step_accel, step_decel, TargetB);//B��
//	ST3_Move(step, step_accel, step_decel, TargetC);//C��
//	ST4_Move(step, step_accel, step_decel, TargetD);//D��
}
void step_stop()
{
	TIM_Cmd(TIM11, DISABLE);
	TIM_Cmd(TIM3, DISABLE);
	TIM_Cmd(TIM9, DISABLE);
	TIM_Cmd(TIM10, DISABLE);
}
void step_start()
{
	TIM_Cmd(TIM11, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM9, ENABLE);
	TIM_Cmd(TIM10, ENABLE);
}
void TIM3_IRQHandler(void)
{
	//speed3_decision();
	speed1_decision();
	//speed2_decision();
}


void TIM2_IRQHandler(void)
{
	speed2_decision();
}

//void TIM4_IRQHandler(void)
//{
//	speed3_decision();
//}

//void TIM9_IRQHandler(void)
//{
//	speed4_decision();
//}
