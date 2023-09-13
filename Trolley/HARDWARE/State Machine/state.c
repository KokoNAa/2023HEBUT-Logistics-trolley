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

void state_Init()
{
	delay_ms(1000);
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
		Move_Transfrom(0,168,0,0);
		delay_ms(1500);
		//状态切换至二维码
		state = QR_state;
		step_stop();
		delay_ms(500);
	}
	else if (state == QR_state)
	{
		//识别二维码状态
		
		//USART_SendData(USART2, 0); //openmv收到指令开始执行
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
		//电机开环移动至原料处
		TIM_Cmd(TIM11, ENABLE);
		Move_Transfrom(168,0,0,0);
		//
		delay_ms(5000);
		//
		TIM_Cmd(TIM11, DISABLE);
		//机械臂执行识别函数
//		if (op2_Data == 1)
//		{
//			//执行抓取
//			
//		}
		//
		
	}
}
