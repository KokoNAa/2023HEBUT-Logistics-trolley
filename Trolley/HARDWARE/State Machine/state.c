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
	delay_ms(1000);
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
		Move_Transfrom(0,168,0,0);
		delay_ms(1500);
		//״̬�л�����ά��
		state = QR_state;
		step_stop();
		delay_ms(500);
	}
	else if (state == QR_state)
	{
		//ʶ���ά��״̬
		
		//USART_SendData(USART2, 0); //openmv�յ�ָ�ʼִ��
		step_start();
		Move_Transfrom(168,0,0,0);
		delay_ms(3000);
		step_stop();
		delay_ms(1000);
		state = find_state;
//		if (op2_Data == find_state)
//		{
//			state = find_state;
//		}
		
	}
	else if (state == find_state)
	{
		//��������ƶ���ԭ�ϴ�
		TIM_Cmd(TIM11, ENABLE);
		Move_Transfrom(168,0,0,0);
		//
		delay_ms(5000);
		//
		TIM_Cmd(TIM11, DISABLE);
		//��е��ִ��ʶ����
//		if (op2_Data == 1)
//		{
//			//ִ��ץȡ
//			
//		}
		//
		
	}
}
