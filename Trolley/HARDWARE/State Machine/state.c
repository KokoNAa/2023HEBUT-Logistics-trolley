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
int i = 0;
int out_ku = 0;
u8 first_catch[3] = {0, 0, 0};
u8 next_catch[3] = {0, 0, 0};
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
		Move_Transfrom(200,0,0,1600*0.8);
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
		state = findstate_Pre;
//		if (op2_Data == find_state)
//		{
//			state = find_state;
//		}
		
	}
	else if (state == findstate_Pre)
	{
		//��������ƶ���ԭ�ϴ�
		if (i >2)
		{
			state = move_Rough;
		}
		if (first_catch[i] == 1 && i <= 2)
		{
			USART_SendData(USART3,'r');
			while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
			state = findstate_wait;
		}
		else if (first_catch[i] == 2 && i <= 2)
		{
			USART_SendData(USART3,'g');
			while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
			state = findstate_wait;
		}
		else if (first_catch[i] == 3 && i <= 2)
		{
			USART_SendData(USART3,'b');
			while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
			state = findstate_wait;
		}
//		Move_Transfrom(0,0,200,1600*1.45);
//		while (motor_sta != STOP);
//		//
//		
//		
//		
//		//
//		Move_Transfrom(0,-300,0,1600*3);
//		while (motor_sta != STOP);
//		//��е��ִ��ʶ����
		
	}
	else if (state == findstate_wait)
	{
		if (op1_State == 'R')
		{
			//�˴�Ϊ��е��ִ��ץȡ��ɫ���ĺ���
			
			//
			delay_ms(3000);//Ϊ�˱�֤ʱ����ȷ������ʱ��������߶�������ܷ��ض��״̬���Բ���ʱ
			op1_State = 'W';
			state = findstate_Pre;
			i++;
		}
		else if (op1_State == 'G')
		{
			//�˴�Ϊ��е��ִ��ץȡ��ɫ���ĺ���
			
			//
			delay_ms(3000);//Ϊ�˱�֤ʱ����ȷ������ʱ��������߶�������ܷ��ض��״̬���Բ���ʱ
			op1_State = 'W';
			state = findstate_Pre;
			i++;
		}
		else if (op1_State == 'B')
		{
			//�˴�Ϊ��е��ִ��ץȡ��ɫ���ĺ���
			
			//
			delay_ms(3000);//Ϊ�˱�֤ʱ����ȷ������ʱ��������߶�������ܷ��ض��״̬���Բ���ʱ
			op1_State = 'W';
			state = findstate_Pre;
			i++;
		}
	}
	else if (state == move_Rough)
	{
		
	}
}
