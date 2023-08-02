#ifndef _PWM_H
#define _PWM_H
#include "allFiles.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#define ST1_PWM_PIN GPIO_Pin_6
#define ST2_PWM_PIN GPIO_Pin_7
#define ST3_PWM_PIN GPIO_Pin_6
#define ST4_PWM_PIN GPIO_Pin_5

#define ST1_PWM_GPIO GPIOA
#define ST2_PWM_GPIO GPIOF
#define ST3_PWM_GPIO GPIOF
#define ST4_PWM_GPIO GPIOE

void ST_PWM_Init(void);

#endif
