#include "allFiles.h"

float dir_error = 0;
double Error_angle_out = 0;
float kp = 0;
int32_t step = 0;
double speed_x = 0, angle_speed = 0;
/*********************************************
 *���������⺯��
 *��������out_ku()
 *��ע��
 *���ڣ�2023.08.11
 *�޸����ڣ�
 *********************************************/

/*********************************************
 *����������
 *��������direction_out()
 *��ע��
 *���ڣ�2023.08.11
 *�޸����ڣ�
 *********************************************/
void direction_out()
{
	Error_angle_out = dir_error * kp;
	//�޷�
	
	
	
	//
	//Move_Transfrom(step, speed_x,Error_angle_out,angle_speed);
}