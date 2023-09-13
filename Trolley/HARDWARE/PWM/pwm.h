#ifndef _PWM_H
#define _PWM_H
#include "allFiles.h"

#define ST1_PWM_PIN GPIO_Pin_6
#define ST2_PWM_PIN GPIO_Pin_1
#define ST3_PWM_PIN GPIO_Pin_7
#define ST4_PWM_PIN GPIO_Pin_5

#define ST1_PWM_GPIO GPIOA
#define ST2_PWM_GPIO GPIOA
#define ST3_PWM_GPIO GPIOA
#define ST4_PWM_GPIO GPIOE

void ST_PWM_Init(void);
void Motor_Init();
void SE_Init(u32 arr,u32 psc);
void PWM_TIM4_Init(void);
void TIM5_PWM_Init(void);
#endif;

