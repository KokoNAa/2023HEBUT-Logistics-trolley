#ifndef _STEP_MOTOR_H
#define _STEP_MOTOR_H
#include "allFiles.h""
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

#define TIM_PRESCALER 37.3 //����ϸ�ֵ�Ҫ�����
#define T1_FREQ (84000000/(TIM_PRESCALER + 1))  //��ʱ��Ƶ��

#define STEP_ANGLE 1.8
#define FSPR 200
#define MICRO_STEP 8  //ϸ����
#define SPR (FSPR*MICRO_STEP)

//��ѧ����
#define ALPHA  ((float)(2*3.14/SPR))
#define A_T_x10 ((float)10*ALPHA*T1_FREQ)

#define ST1_DIR_GPIO_PORT GPIOA
#define ST1_DIR_GPIO_PIN GPIO_Pin_7
#define ST1_EN_GPIO_PORT GPIOA
#define ST1_EN_GPIO_PIN GPIO_Pin_8

void step_motor_init(void);

#endif
