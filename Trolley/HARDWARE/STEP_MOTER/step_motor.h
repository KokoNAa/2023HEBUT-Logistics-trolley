#ifndef _STEP_MOTOR_H
#define _STEP_MOTOR_H
#include "allFiles.h""
typedef struct
{
	uint8_t run_state;//状态
	uint8_t dir;//方向
	int step_delay;
	int decel_start;//启动减速
	int decel_val;//减速阶段步数
	int min_delay;//最小脉冲周期
	int accel_count;
}speedRampData;

/*电机的四个状态*/
#define STOP 0  //停止
#define ACCEL 1  //加速
#define DECEL 2  //减速
#define RUN 3  //匀速

#define TIM_PRESCALER 37.3 //根据细分的要求更改
#define T1_FREQ (84000000/(TIM_PRESCALER + 1))  //定时器频率

#define STEP_ANGLE 1.8
#define FSPR 200
#define MICRO_STEP 8  //细分数
#define SPR (FSPR*MICRO_STEP)

//数学常数
#define ALPHA  ((float)(2*3.14/SPR))
#define A_T_x10 ((float)10*ALPHA*T1_FREQ)

#define ST1_DIR_GPIO_PORT GPIOA
#define ST1_DIR_GPIO_PIN GPIO_Pin_7
#define ST1_EN_GPIO_PORT GPIOA
#define ST1_EN_GPIO_PIN GPIO_Pin_8

void step_motor_init(void);

#endif
