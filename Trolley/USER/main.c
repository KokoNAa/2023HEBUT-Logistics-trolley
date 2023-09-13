#include "allFiles.h"

uint32_t motorSpeed = 0;
uint16_t a = 2;
int ccc = 0;
u8 send_buff[1] = {QR_state};
int main(void)
	/*****
整车架构由状态机驱动，不同的状态执行不同的动作
具体见 state文件


******/
{
	u8 lcd_id[12];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	//Motor_Init();
	step_motor_init();
	ST_PWM_Init();
	LED_Init();
	uart3_init(9600);
	uart1_init(115200);
	//uart1_init(9600);
	//M_Init();
	//delay_ms(1000);
	//Init_ctrl();
	LCD_Init();
	POINT_COLOR = BLACK;
	while(1)
	{
		//LCD_ShowxNum(30, 100, sp, 10, 12, 0);
		LCD_DrawRectangle(10, 50, 230,160);
		//LCD_ShowString(80,20,210,24,24,"STATE:");
		LCD_ShowChar(80,50,op1_State,24,1);
		POINT_COLOR = BLUE;
		//USART_SendData(USART3,'4');
		//while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
		
		//ST2_Move(1600*50,50,50,100);
		
//		if (ccc == 0)
//		{
//			state_Init();//状态机初始化
//			ccc = 1;
//		}//
//		state_ctrl();//状态机执行
		//Move_Transfrom(0,0,100,1600*5);
//		if (state == garage_out)
//		{
//			LCD_ShowString(59,90,210,50,24,"garage_out");
//		}
//		else if (state == QR_state)
//		{
//			LCD_ShowString(59,90,210,50,24,"QR_CODE");
//		}
//		else if (state == find_state)
//		{
//			LCD_ShowString(59,90,210,50,24,"FINDING");
//		}
		//ST1_Move(1600 * 10, 500, 500, 250);
		
		//LCD_ShowxNum(59, 180, op1_Data[0], 10, 24, 0);
		//LCD_ShowxNum(100, 250, op1_State, 3, 24, 0);
	}
} 
