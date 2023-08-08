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

#define TIM_FREQ 84000000U
#define TIM_PRESCALER 41 //根据细分的要求更改
#define T1_FREQ (TIM_FREQ/(TIM_PRESCALER + 1))  //定时器频率

#define STEP_ANGLE 1.8
#define FSPR 200
#define MICRO_STEP 8  //细分数
#define SPR (FSPR*MICRO_STEP)

//数学常数
#define ALPHA  ((float)(2*3.14/SPR))
#define A_T_x10 ((float)10*ALPHA*T1_FREQ)
#define T1_FREQ_148 ((float)((T1_FREQ*0.676)/10))
#define A_SQ ((float)(2*100000*ALPHA)) 
#define A_x200 ((float)(200*ALPHA))
	
//方向使能引脚定义
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

#endif
