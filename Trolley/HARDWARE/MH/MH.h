#ifndef _MH_H
#define _MH_H
#include "allFiles.h"

#define MA1_PORT GPIOD
#define MA5_PORT GPIOA

#define MA1_PIN GPIO_Pin_12
#define MA2_PIN GPIO_Pin_13
#define MA3_PIN GPIO_Pin_14
#define MA4_PIN GPIO_Pin_15
#define MA5_PIN GPIO_Pin_2

int32_t Servo_RadianToCCR(double radian,int16_t bias,int8_t dir);
void M_Init();
void takeout_ctrl();
void takein_ctrl();
void QR_ctrl();
void find_ctrl();
void M_Caculation(double X,double Y,double Z);
void Init_ctrl();

#endif
