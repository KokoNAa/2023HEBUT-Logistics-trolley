#ifndef _PWM_H
#define _PWM_H
#include "allFiles.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
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
