#include "allFiles.h"

//////////////////////////////////////////////////////////////////////////////////	 
//状态机
//修改日期:2023/8/13
//版本：V1.0
//********************************************************************************
//V1.0 说明：
//通过串口接受到的数据改变标志位，将程序指向不同的状态，执行相应的执行函数
////////////////////////////////////////////////////////////////////////////////// 	 

int state;
int i = 0;
int out_ku = 0;
u8 first_catch[3] = {0, 0, 0};
u8 next_catch[3] = {0, 0, 0};
void state_Init()
{
	delay_ms(2000);
	state = garage_out;//启动电源后首先执行出库
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
		//电机出库函数
		LCD_ShowString(59,90,210,50,24,"garage_out");
		Move_Transfrom(200,0,0,1600*0.8);
		while (motor_sta != STOP);//等待电机运行完
		//状态切换至二维码
		state = QR_state;
	}
	else if (state == QR_state)
	{
		LCD_ShowString(59,90,210,50,24,"QR_CODE");
		//识别二维码状态
		
		//USART_SendData(USART2, 0); //openmv收到指令开始执行
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
		//电机开环移动至原料处
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
//		//机械臂执行识别函数
		
	}
	else if (state == findstate_wait)
	{
		if (op1_State == 'R')
		{
			//此处为机械臂执行抓取红色物块的函数
			
			//
			delay_ms(3000);//为了保证时序正确启用延时，如果总线舵机串口能返回舵机状态可以不延时
			op1_State = 'W';
			state = findstate_Pre;
			i++;
		}
		else if (op1_State == 'G')
		{
			//此处为机械臂执行抓取绿色物块的函数
			
			//
			delay_ms(3000);//为了保证时序正确启用延时，如果总线舵机串口能返回舵机状态可以不延时
			op1_State = 'W';
			state = findstate_Pre;
			i++;
		}
		else if (op1_State == 'B')
		{
			//此处为机械臂执行抓取蓝色物块的函数
			
			//
			delay_ms(3000);//为了保证时序正确启用延时，如果总线舵机串口能返回舵机状态可以不延时
			op1_State = 'W';
			state = findstate_Pre;
			i++;
		}
	}
	else if (state == move_Rough)
	{
		
	}
}
