#include "allFiles.h"

//////////////////////////////////////////////////////////////////////////////////	 
//״̬��
//�޸�����:2023/8/13
//�汾��V1.0
//********************************************************************************
//V1.0 ˵����
//ͨ�����ڽ��ܵ������ݸı��־λ��������ָ��ͬ��״̬��ִ����Ӧ��ִ�к���
////////////////////////////////////////////////////////////////////////////////// 	 

int state;

void state_Init()
{
	delay_ms(2000);
	state = garage_out;//������Դ������ִ�г���
}

//void state_calculation()
//{
//	
//}
void state_ctrl()
{ 
	if (state == garage_out)
	{
		Init_ctrl();
		//������⺯��
		LCD_ShowString(59,90,210,50,24,"garage_out");
		Move_Transfrom(200,0,0,1600*3);
		while (motor_sta != STOP);//�ȴ����������
		//״̬�л�����ά��
		state = QR_state;
	}
	else if (state == QR_state)
	{
		LCD_ShowString(59,90,210,50,24,"QR_CODE");
		//ʶ���ά��״̬
		
		//USART_SendData(USART2, 0); //openmv�յ�ָ�ʼִ��
		Move_Transfrom(300,0,0,1600*6.5);
		while (motor_sta != STOP);
		state = find_state;
//		if (op2_Data == find_state)
//		{
//			state = find_state;
//		}
		
	}
	else if (state == find_state)
	{
		//��������ƶ���ԭ�ϴ�
		Move_Transfrom(0,0,200,1600*1.45);
		while (motor_sta != STOP);
		//
		
		
		
		//
		Move_Transfrom(0,-300,0,1600*3);
		while (motor_sta != STOP);
		//��е��ִ��ʶ����
//		if (op2_Data == 1)
//		{
//			//ִ��ץȡ
//			
//		}
		//
		
	}
}
