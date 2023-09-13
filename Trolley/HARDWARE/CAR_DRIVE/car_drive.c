#include "allFiles.h"

float dir_error = 0;
double Error_angle_out = 0;
float kp = 0;
int32_t step = 0;
double speed_x = 0, angle_speed = 0;
/*********************************************
 *函数：出库函数
 *函数名：out_ku()
 *备注：
 *日期：2023.08.11
 *修改日期：
 *********************************************/

/*********************************************
 *函数：方向环
 *函数名：direction_out()
 *备注：
 *日期：2023.08.11
 *修改日期：
 *********************************************/
void direction_out()
{
	Error_angle_out = dir_error * kp;
	//限幅
	
	
	
	//
	//Move_Transfrom(step, speed_x,Error_angle_out,angle_speed);
}