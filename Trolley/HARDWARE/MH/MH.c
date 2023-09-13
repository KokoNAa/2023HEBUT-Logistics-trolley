#include "allFiles.h"

//���ź궨���.h�ļ�
double PI = 3.14159265358979;
//����ȷ���Բ���������̬����ʱ���û�ֻ��Ҫ����XYZ�����������ڳ�������ǰȷ����
double Xi = 8;//���������꣺
double Yi = 14;
double Zi = 8;
double Xoffest = 0;//ĩ�˵���������ϵ��Ի�е�۸�������ϵ��ƫ������
double Yoffest = 0;
double Zoffest = 0;
double A1 = 9.5;//��е�۵��ĸ��۳���
double A2 = 6.5;
double A3 = 13;
double A4 = 14;
double J1_abs_max = 180;//j1~j4�ĽǶȵľ���ֵ�����ֵ��(��������270��������ò������Ϊ135��180Ϊ360�������ֵ��Ҳ���������ֵ)
double J2_abs_max = 135;
double J3_abs_max = 135;
double J4_abs_max = 135;

//��������
double j1 = 0;//�����������ݣ�
double j2 = 0;
double j3 = 0;
double j4 = 0;
double Xo = 0;//�����������ݵ���������֤�����
double Yo = 0;
double Zo = 0;

//�߼�������
uint8_t valid_j1234 = 0;//�������������ݵ���Ч��
uint8_t valid_xyz = 0;//����������ĵ����ݵ���Ч��

//�洢������
int16_t  alpha_degree;//��0~180���ڱ仯�����ڱ������п��ܽǶ�
double alpha_radian;//alpha_degree�Ļ���ֵ
double length_I,height_I,L,H;//����Qi������������
double cosj3,sinj3;//����j3���м���
double k1,k2;//����j2���м���
int8_t sign_flip = 1;//ʹ��alpha���趨alpha�����������ĸ�������
int16_t grow_num = 1;//����ͬ�ϡ�
double length_O,height_O;//����Qo�����������Ϊ�м�����


uint16_t Servo1_CCR_Crouch = 3104;//Բ̨���
uint16_t Servo2_CCR_Crouch = 4620;//4620
uint16_t Servo3_CCR_Crouch = 1180;//1180
uint16_t Servo4_CCR_Crouch = 4644;//��צ�ı���ˮƽ�Ķ��4644
void M_Init()
{
	PWM_TIM4_Init();
	TIM5_PWM_Init();
	
}
uint8_t IsValid_j(uint8_t jointIndex,double jointAngle_radian)
{
	static double jointAngle_degree;
	if(pow(jointAngle_radian,2)<0.01)//�ӽ��㣬�����㡣
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
			return 0;//��ǰֻ������4���е��
	}
}
void Backward_Kinematics(void)
{
	//������λ��
	alpha_degree = 90;//��90�㿪ʼ��������㣺����������кܶ�alpha�⣬��alpha_degree��ֵ����������Ҫ�ӽ���alphaֵ��
	valid_j1234 = 0;
	j1 = 0;
	j2 = 0;
	j3 = 0;
	j4 = 0;
	//��ʼ���㣺
	while(((alpha_degree >= 0) && (alpha_degree <= 180)) && (!valid_j1234))//��������ң����ǻ�û�ҵ��Ļ���������
	{
        alpha_radian = alpha_degree*PI/180;
	    ///j1:
        length_I = sqrt(pow(Yi+Yoffest,2) + pow(Xi+Xoffest,2));
	    height_I = Zi + Zoffest;
	    if(length_I < 0.01)
	    	j1 = 0;
	    else
	    	j1 = atan2((Yi+Yoffest),(Xi+Xoffest));
	    //����ֵ��ȷ���жϣ�
	    if(height_I < 0)//ĩ�˵���ͣ����ǻ�е�����������
            break;//�������ݴ���
	    if((A2+A3+A4) < sqrt(pow(length_I,2)+pow(Zi-A1,2)))//ĩ�˵��Զ�����ǻ�е������������
	    	break;//�������ݴ���
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
 *��������е���˶�ѧ�����ܺ���
 *��������M_calculation()
 *��ע���Ի�е�۽����˶�ѧ����
���ڣ�2023.08.20
*�޸����ڣ�
* *********************************************/
void M_Caculation(double X,double Y,double Z)
{
	Xi = X;
	Yi = Y;
	Zi = Z;
//	uint16_t Servo1_CCR_Crouch = 3104;//Բ̨���
//	uint16_t Servo2_CCR_Crouch = 4620;
//	uint16_t Servo3_CCR_Crouch = 1180;
//	uint16_t Servo4_CCR_Crouch = 4644;//��צ�ı���ˮƽ�Ķ��
	//M_Init();
	Backward_Kinematics();
	//�����ϣ�
	if(valid_j1234)
	{
		Forward_Kinematics();
		LED0=!LED0;
		LED1=!LED1;
	}
	if((fabs(Xo - Xi)>0.5) || (fabs(Yo - Yi)>0.5) || (fabs(Zo - Zi)>0.5))
	{
		//����ʧ�ܡ�
	
	}
    else
	{
		//����ɹ���
		
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
	//����Ĭ����������270���������Ҷ����ת��������-135��~135�㣬������������ź�Ƶ��Ϊ200Hz��
	//return 4000/270 * radian * 180 / PI * dir
	if(dir>0)
		dir = 1;
	else
		dir = -1;
	return 8000/3/ PI * radian * dir  + 3000 + bias;
}

/*********************************************
 *��������е��Init����
 *��������takeout_ctrl()
 *��ע����е�۶�Ӧ��ִ�к���
���ڣ�2023.08.20
*�޸����ڣ�
* *********************************************/
void Init_ctrl()
{
	TIM_SetCompare1(TIM4, Servo1_CCR_Crouch);	
	TIM_SetCompare2(TIM4, Servo2_CCR_Crouch);	
	TIM_SetCompare3(TIM4, Servo3_CCR_Crouch);	
	TIM_SetCompare4(TIM4, Servo4_CCR_Crouch);	
}
/*********************************************
 *��������е��takeout����
 *��������takeout_ctrl()
 *��ע����е�۶�Ӧ��ִ�к���
���ڣ�2023.08.13
*�޸����ڣ�
* *********************************************/
void takeout_ctrl()
{
	TIM_SetCompare1(TIM4, 1000);//1	
	TIM_SetCompare2(TIM4, 3500);	//4
	TIM_SetCompare3(TIM4, 3500);//	3
	TIM_SetCompare4(TIM4, 3800);//2	
}
/*********************************************
 *��������е��takein����
 *��������takein_ctrl()
 *��ע����е�۶�Ӧ��ִ�к���
���ڣ�2023.08.13
*�޸����ڣ�
* *********************************************/
void takein_ctrl()
{
	
}
/*********************************************
 *��������е��QR����
 *��������QR_ctrl()
 *��ע����е�۽����ά��ʶ��״̬��ִ�к���
���ڣ�2023.08.13
*�޸����ڣ�
* *********************************************/
void QR_ctrl()
{
	TIM_SetCompare1(TIM4, 1000);//1	
	TIM_SetCompare2(TIM4, 3333);	//4
	TIM_SetCompare3(TIM4, 2500);//	3
	TIM_SetCompare4(TIM4, 3800);//2	
}
/*********************************************
 *��������е��find����
 *��������find_ctrl()
 *��ע����е�۽�������ʶ��״̬��ִ�к���
���ڣ�2023.08.13
*�޸����ڣ�
* *********************************************/
void find_ctrl()
{
	TIM_SetCompare1(TIM4, 1000);//1	
	TIM_SetCompare2(TIM4, 3000);	//4
	TIM_SetCompare3(TIM4, 3500);//	3
	TIM_SetCompare4(TIM4, 3400);//2	
}