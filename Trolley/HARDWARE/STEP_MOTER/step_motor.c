#include "allFiles.h"

uint16_t CCR1 = 20000;

speedRampData srd= {STOP,0,0,0,0,0,0};
uint8_t  motor_sta        = 0;//���״̬

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
	GPIO_InitStructure.GPIO_Pin = ST1_DIR_GPIO_PIN  | ST1_EN_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����(�������ӷ���0Ϊ��Ч)
    GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
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

void MOTOR_Move(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
		
    uint16_t tim_count; 																										 //����ж�ʱ�̵ļ���ֵ
    unsigned int max_s_lim;                                     					 	 //�ﵽ����ٶ�ʱ�Ĳ���    
    unsigned int accel_lim;																									 //���뿪ʼ���ٵĲ����������û�м��ٶȵ�����ٶ�ʱ��

	if(motor_sta!= STOP)  																									 //ֻ�����������ֹͣ��ʱ��ż���
			return;			
    if(step < 0)   								//��ʱ��
	{
		GPIO_SetBits(ST1_DIR_GPIO_PORT,ST1_DIR_GPIO_PIN);			//����ߵ�ƽ
		GPIO_SetBits(ST2_DIR_GPIO_PORT,ST2_DIR_GPIO_PIN);
		GPIO_SetBits(ST3_DIR_GPIO_PORT,ST3_DIR_GPIO_PIN);
		GPIO_SetBits(ST4_DIR_GPIO_PORT,ST4_DIR_GPIO_PIN);
		step = -step;      
	}		
	else   			//˳ʱ��
	{
		GPIO_ResetBits(ST1_DIR_GPIO_PORT,ST1_DIR_GPIO_PIN);		//����͵�ƽ		
		GPIO_ResetBits(ST2_DIR_GPIO_PORT,ST2_DIR_GPIO_PIN);	
		GPIO_ResetBits(ST3_DIR_GPIO_PORT,ST3_DIR_GPIO_PIN);	
		GPIO_ResetBits(ST4_DIR_GPIO_PORT,ST4_DIR_GPIO_PIN);	
	}        
    if(step == 1)   																											   // ���ֻ�ƶ�һ��
    {       
      srd.accel_count = -1; 																								 // ֻ�ƶ�һ��
        
      srd.run_state = DECEL;																								 // ����״̬
        
      srd.step_delay = 1000;																								 // ����ʱ

     }
    
    else if(step != 0)  																			 								// ������Ϊ����ƶ�
    {					
				srd.min_delay = (int32_t)(A_T_x10/speed);							 // ��������ٶȼ���, ����min_delay���ڶ�ʱ���ļ�������ֵmin_delay = (alpha / tt)/ 
				srd.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10);		// ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶ�,����accel��λΪ0.01rad/sec^2
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
					srd.decel_val = accel_lim - step;				//���ٶεĲ���
				}
				else
				{
					srd.decel_val = -(max_s_lim*accel/decel);	//���ٶεĲ��� 
				}
   
				if(srd.decel_val == 0) 				// ����һ�� ��һ������ 
				{
					srd.decel_val = -1;
				}    
				srd.decel_start = step + srd.decel_val;																//���㿪ʼ����ʱ�Ĳ���
 
				if(srd.step_delay <= srd.min_delay)																		// ���һ��ʼc0���ٶȱ����ٶ��ٶȻ��󣬾Ͳ���Ҫ���м����˶���ֱ�ӽ�������
				{
					srd.step_delay = srd.min_delay;
					srd.run_state = RUN;
				}
				else
				{
					srd.run_state = ACCEL;
				}
    
				srd.accel_count = 0;																									// ��λ���ٶȼ���ֵ
		
			}
			motor_sta = 1;  																												// ���Ϊ�˶�״̬
			tim_count = TIM_GetCounter(TIM3);																				//��ȡ����ֵ
			TIM_SetCompare1(TIM3,tim_count+srd.step_delay/2);												//���ö�ʱ���Ƚ�ֵ 
			TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);																		//ʹ�ܶ�ʱ��ͨ�� 
			TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Enable);
			TIM_Cmd(TIM3, ENABLE);																									//������ʱ��
}
void TIM3_IRQHandler(void)
{
	
}

