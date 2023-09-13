#ifndef _STEP_MOTOR_H
#define _STEP_MOTOR_H
#include "allFiles.h"
typedef struct
{
	uint8_t run_state;//״̬
	uint8_t dir;//����
	int step_delay;
	int decel_start;//��������
	int decel_val;//���ٽ׶β��� 
	int min_delay;//��С��������
	int accel_count;
}speedRampData;

/*������ĸ�״̬*/
#define STOP 0  //ֹͣ
#define ACCEL 1  //����
#define DECEL 2  //����
#define RUN 3  //����

#define TIM3_FREQ 84000000U
#define TIM_FREQ 168000000U
#define TIM3_PRESCALER 42 //����ϸ�ֵ�Ҫ�����
#define TIM_PRESCALER 84 //����ϸ�ֵ�Ҫ�����
#define T1_FREQ (TIM3_FREQ/TIM3_PRESCALER)  //��ʱ��3Ƶ��
#define T2_FREQ (TIM_FREQ/TIM_PRESCALER)  //��ʱ��otherƵ��

#define STEP_ANGLE 1.8
#define FSPR 200
#define MICRO_STEP 8  //ϸ����
#define SPR (FSPR*MICRO_STEP)

//��ѧ����
#define ALPHA  ((float)(2*3.14/SPR))
#define A_T_x10 ((float)10*ALPHA*T1_FREQ)
#define A1_T_x10 ((float)10*ALPHA*T2_FREQ)
#define T1_FREQ_148 ((float)((T1_FREQ*0.676)/10))
#define T2_FREQ_148 ((float)((T2_FREQ*0.676)/10))	
#define A_SQ ((float)(2*100000*ALPHA)) 
#define A_x200 ((float)(200*ALPHA))
	
//����ʹ�����Ŷ���
#define ST1_DIR_GPIO_PORT GPIOA
#define ST1_DIR_GPIO_PIN GPIO_Pin_11
#define ST1_EN_GPIO_PORT GPIOA
#define ST1_EN_GPIO_PIN GPIO_Pin_12

#define ST2_DIR_GPIO_PORT GPIOF
#define ST2_DIR_GPIO_PIN GPIO_Pin_5
#define ST2_EN_GPIO_PORT GPIOF
#define ST2_EN_GPIO_PIN GPIO_Pin_13

#define ST3_DIR_GPIO_PORT GPIOF
#define ST3_DIR_GPIO_PIN GPIO_Pin_8
#define ST3_EN_GPIO_PORT GPIOF
#define ST3_EN_GPIO_PIN GPIO_Pin_11

#define ST4_DIR_GPIO_PORT GPIOE
#define ST4_DIR_GPIO_PIN GPIO_Pin_2
#define ST4_EN_GPIO_PORT GPIOE
#define ST4_EN_GPIO_PIN GPIO_Pin_3
void step_motor_init(void);
void Motor1_Run(uint32_t dir,uint32_t num,uint32_t speed);
void Motor2_Run(uint32_t dir,uint32_t num,uint32_t speed);
void Motor3_Run(uint32_t dir,uint32_t num,uint32_t speed);
void Motor4_Run(uint32_t dir,uint32_t num,uint32_t speed);
void ST1_Move(int32_t step, uint32_t accel, uint32_t decel, int32_t speed);
void ST2_Move(int32_t step, uint32_t accel, uint32_t decel, int32_t speed);
void ST3_Move(int32_t step, uint32_t accel, uint32_t decel, int32_t speed);
//void ST4_Move(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);
void Move_Transfrom(int16_t Vx,int16_t Vy,int16_t Vz,uint32_t step);
extern void speed1_decision(void);
extern void speed2_decision(void);
extern void speed3_decision(void);
extern uint32_t sp; 
void step_stop();
void step_start();
//extern void speed4_decision(void);

#endif
