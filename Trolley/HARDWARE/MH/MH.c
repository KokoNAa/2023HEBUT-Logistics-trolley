#include "allFiles.h"

//引脚宏定义见.h文件
double PI = 3.14159265358979;
//输入确定性参数：（姿态解算时，用户只需要输入XYZ，其他参数在程序运行前确定）
double Xi = 8;//输入点的坐标：
double Yi = 14;
double Zi = 8;
double Xoffest = 0;//末端点所处坐标系相对机械臂根点坐标系的偏移量：
double Yoffest = 0;
double Zoffest = 0;
double A1 = 9.5;//机械臂的四个臂长：
double A2 = 6.5;
double A3 = 13;
double A4 = 14;
double J1_abs_max = 180;//j1~j4的角度的绝对值的最大值。(如果舵机是270°舵机，则该参数最大为135。180为360°舵机最大值，也是理论最大值)
double J2_abs_max = 135;
double J3_abs_max = 135;
double J4_abs_max = 135;

//输出结果：
double j1 = 0;//逆解算输出数据：
double j2 = 0;
double j3 = 0;
double j4 = 0;
double Xo = 0;//逆解算输出数据的正解算验证结果：
double Yo = 0;
double Zo = 0;

//逻辑变量：
uint8_t valid_j1234 = 0;//逆解算输出的数据的有效性
uint8_t valid_xyz = 0;//正解算输出的的数据的有效性

//存储变量：
int16_t  alpha_degree;//在0~180度内变化，用于遍历所有可能角度
double alpha_radian;//alpha_degree的弧度值
double length_I,height_I,L,H;//根据Qi坐标计算而来。
double cosj3,sinj3;//计算j3的中间量
double k1,k2;//计算j2的中间量
int8_t sign_flip = 1;//使得alpha从设定alpha度向两侧计算的辅助变量
int16_t grow_num = 1;//作用同上。
double length_O,height_O;//根据Qo计算而来，作为中间量。


uint16_t Servo1_CCR_Crouch = 3104;//圆台舵机
uint16_t Servo2_CCR_Crouch = 4620;//4620
uint16_t Servo3_CCR_Crouch = 1180;//1180
uint16_t Servo4_CCR_Crouch = 4644;//夹爪的保持水平的舵机4644
void M_Init()
{
	PWM_TIM4_Init();
	TIM5_PWM_Init();
	
}
uint8_t IsValid_j(uint8_t jointIndex,double jointAngle_radian)
{
	static double jointAngle_degree;
	if(pow(jointAngle_radian,2)<0.01)//接近零，视作零。
		return 0;
	else
		jointAngle_degree = jointAngle_radian * 180 / PI;
	switch(jointIndex)
	{
		case 1:
			if((jointAngle_degree>=-J1_abs_max)&&(jointAngle_degree<=J1_abs_max))
				return 1;			
			else
				return 0;
		case 2:
			if((jointAngle_degree>=-J2_abs_max)&&(jointAngle_degree<=J2_abs_max))
				return 1;
			else
				return 0;
		case 3:
			if((jointAngle_degree>=-J3_abs_max)&&(jointAngle_degree<=J3_abs_max))
				return 1;
			else
				return 0;
		case 4:
			if((jointAngle_degree>=-J4_abs_max)&&(jointAngle_degree<=J4_abs_max))
				return 1;
			else
				return 0;
		default:
			return 0;//当前只适用于4轴机械臂
	}
}
void Backward_Kinematics(void)
{
	//变量复位：
	alpha_degree = 90;//从90°开始往两侧计算：逆解算往往有很多alpha解，而alpha_degree的值就是我们想要接近的alpha值。
	valid_j1234 = 0;
	j1 = 0;
	j2 = 0;
	j3 = 0;
	j4 = 0;
	//开始解算：
	while(((alpha_degree >= 0) && (alpha_degree <= 180)) && (!valid_j1234))//如果还能找，但是还没找到的话，继续找
	{
        alpha_radian = alpha_degree*PI/180;
	    ///j1:
        length_I = sqrt(pow(Yi+Yoffest,2) + pow(Xi+Xoffest,2));
	    height_I = Zi + Zoffest;
	    if(length_I < 0.01)
	    	j1 = 0;
	    else
	    	j1 = atan2((Yi+Yoffest),(Xi+Xoffest));
	    //输入值正确性判断：
	    if(height_I < 0)//末端点过低！除非机械臂有钻地能力
            break;//输入数据错误
	    if((A2+A3+A4) < sqrt(pow(length_I,2)+pow(Zi-A1,2)))//末端点过远！除非机械臂有拉伸能力
	    	break;//输入数据错误
        ///j3:
	    if(IsValid_j(1,j1))
	    {
	    	L = length_I - A4 * sin(alpha_radian);
	    	H = height_I - A4 * cos(alpha_radian) - A1;
            ////
            cosj3 = (pow(L,2)+pow(H,2)-pow(A2,2)-pow(A3,2)) / (2*A2*A3);
	        if(pow(cosj3,2)>1)
	        	j3 = 0;
	        else
	        {
	        	sinj3 = sqrt(1 - pow(cosj3,2));
	        	j3 = atan2(sinj3,cosj3);
	        }
            ////
	    	if(IsValid_j(3,j3))
	    	{
                //j2:
                k1 = A2 + A3 * cos(j3);
	            k2 = A3 * sin(j3);
	            j2 = atan2(L,H) - atan2(k2,k1);
	    		if(IsValid_j(2,j2))
	    		{
	    			//j4:
                    j4 = alpha_radian - j2 - j3;                
	    			if(IsValid_j(4,j4))
	    				valid_j1234 = 1;
	    		}
	    	}
	    }
		if(!valid_j1234)
		{
			alpha_degree += grow_num * sign_flip;
			grow_num++;
			sign_flip = -sign_flip;
		}
	}
}


void Forward_Kinematics(void)
{
	valid_xyz = 0;
	if((!IsValid_j(1,j1)) || (!IsValid_j(2,j2)) || (!IsValid_j(3,j3)) || (!IsValid_j(4,j4)))
	{
		valid_xyz = 0;
		Xo = 0;
		Yo = 0;
		Zo = 0;
	}
	else
	{
	    length_O = A2*sin(j2) + A3*sin(j2+j3) + A4*sin(j2+j3+j4);
		height_O = A1 + A2*cos(j2) + A3*cos(j2+j3) + A4*cos(j2+j3+j4);
		Xo  = length_O * cos(j1) - Xoffest;
		Yo = length_O * sin(j1) - Yoffest;
		Zo = height_O;
		if(Zo>=0)
			valid_xyz = 1;
	}
}

/*********************************************
 *函数：机械臂运动学解算总函数
 *函数名：M_calculation()
 *备注：对机械臂进行运动学解算
日期：2023.08.20
*修改日期：
* *********************************************/
void M_Caculation(double X,double Y,double Z)
{
	Xi = X;
	Yi = Y;
	Zi = Z;
//	uint16_t Servo1_CCR_Crouch = 3104;//圆台舵机
//	uint16_t Servo2_CCR_Crouch = 4620;
//	uint16_t Servo3_CCR_Crouch = 1180;
//	uint16_t Servo4_CCR_Crouch = 4644;//夹爪的保持水平的舵机
	//M_Init();
	Backward_Kinematics();
	//逆解完毕；
	if(valid_j1234)
	{
		Forward_Kinematics();
		LED0=!LED0;
		LED1=!LED1;
	}
	if((fabs(Xo - Xi)>0.5) || (fabs(Yo - Yi)>0.5) || (fabs(Zo - Zi)>0.5))
	{
		//解算失败、
	
	}
    else
	{
		//解算成功、
		
	}
	Servo1_CCR_Crouch =Servo_RadianToCCR(j1,20,1);
	Servo2_CCR_Crouch =Servo_RadianToCCR(j2,-10,1);
	Servo3_CCR_Crouch =Servo_RadianToCCR(j3,35,1);
	Servo4_CCR_Crouch =Servo_RadianToCCR(j4,-80,-1);
	TIM_SetCompare1(TIM4, Servo1_CCR_Crouch);	
	TIM_SetCompare2(TIM4, Servo2_CCR_Crouch);	
	TIM_SetCompare3(TIM4, Servo3_CCR_Crouch);	
	TIM_SetCompare4(TIM4, Servo4_CCR_Crouch);	
}

int32_t Servo_RadianToCCR(double radian,int16_t bias,int8_t dir)
{
	//这里默认驱动的是270°舵机，并且舵机的转动量程是-135°~135°，且驱动舵机的信号频率为200Hz。
	//return 4000/270 * radian * 180 / PI * dir
	if(dir>0)
		dir = 1;
	else
		dir = -1;
	return 8000/3/ PI * radian * dir  + 3000 + bias;
}

/*********************************************
 *函数：机械臂Init函数
 *函数名：takeout_ctrl()
 *备注：机械臂对应的执行函数
日期：2023.08.20
*修改日期：
* *********************************************/
void Init_ctrl()
{
	TIM_SetCompare1(TIM4, Servo1_CCR_Crouch);	
	TIM_SetCompare2(TIM4, Servo2_CCR_Crouch);	
	TIM_SetCompare3(TIM4, Servo3_CCR_Crouch);	
	TIM_SetCompare4(TIM4, Servo4_CCR_Crouch);	
}
/*********************************************
 *函数：机械臂takeout函数
 *函数名：takeout_ctrl()
 *备注：机械臂对应的执行函数
日期：2023.08.13
*修改日期：
* *********************************************/
void takeout_ctrl()
{
	TIM_SetCompare1(TIM4, 1000);//1	
	TIM_SetCompare2(TIM4, 3500);	//4
	TIM_SetCompare3(TIM4, 3500);//	3
	TIM_SetCompare4(TIM4, 3800);//2	
}
/*********************************************
 *函数：机械臂takein函数
 *函数名：takein_ctrl()
 *备注：机械臂对应的执行函数
日期：2023.08.13
*修改日期：
* *********************************************/
void takein_ctrl()
{
	
}
/*********************************************
 *函数：机械臂QR函数
 *函数名：QR_ctrl()
 *备注：机械臂进入二维码识别状态的执行函数
日期：2023.08.13
*修改日期：
* *********************************************/
void QR_ctrl()
{
	TIM_SetCompare1(TIM4, 1000);//1	
	TIM_SetCompare2(TIM4, 3333);	//4
	TIM_SetCompare3(TIM4, 2500);//	3
	TIM_SetCompare4(TIM4, 3800);//2	
}
/*********************************************
 *函数：机械臂find函数
 *函数名：find_ctrl()
 *备注：机械臂进入物料识别状态的执行函数
日期：2023.08.13
*修改日期：
* *********************************************/
void find_ctrl()
{
	TIM_SetCompare1(TIM4, 1000);//1	
	TIM_SetCompare2(TIM4, 3000);	//4
	TIM_SetCompare3(TIM4, 3500);//	3
	TIM_SetCompare4(TIM4, 3400);//2	
}