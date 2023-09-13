#include "allFiles.h"

uint32_t motorSpeed = 0;
uint16_t a = 2;
int ccc = 0;
int main(void)
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
	
	ST2_Move(1600 * 10, 25, 25, 600);
	while(1)
	{
		
		LCD_DrawRectangle(10, 50, 230,160);
		LCD_ShowString(80,20,210,24,24,"STATE:");
		POINT_COLOR = BLUE;
		state_Init();
		if (state == garage_out)
		{
			LCD_ShowString(59,90,210,50,24,"garage_out");
		}
		else if (state == find_state)
		{
			LCD_ShowString(59,90,210,50,24,"FINDING");
		}
		LCD_ShowxNum(59, 180, op1_Data[0], 10, 24, 0);
		LCD_ShowxNum(100, 250, op1_State, 3, 24, 0);
		//LCD_ShowxNum(30, 100, sp, 10, 12, 0);
		//LED0 =!LED0;
		//Move_Transfrom(168,0,0,0);
//		if (ccc == 0)
//		{
//			//step_start();
//			delay_ms(1000);
//			Init_ctrl();
//			delay_ms(4000);
//			QR_ctrl();
//			delay_ms(4000);
//			find_ctrl();
////			step_start();
////			Move_Transfrom(168,0,0,0);
////			delay_ms(3000);
////			step_stop();
////			delay_ms(500);
////			step_start();
////			Move_Transfrom(168,0,0,0);
////			delay_ms(3000);
////			step_stop();//ЖўЮЌТы
////			delay_ms(3000);
////			step_start();
////			Move_Transfrom(168,0,0,0);
////			delay_ms(5000);
////			Move_Transfrom(0,0,-10,0);
////			delay_ms(5500);
////			step_stop();
//			ccc = 1;
//		}
//		state_Init();
//		state_ctrl();
		
		//GPIO_SetBits(ST3_DIR_GPIO_PORT,ST3_DIR_GPIO_PIN);
//		delay_ms(2000);
//		GPIO_ResetBits(ST3_DIR_GPIO_PORT,ST3_DIR_GPIO_PIN);
//		delay_ms(2000);
		//ST2_Move(1600 * 50, 1000, 1000, 200);
		//USART_SendData(USART3, 1);
//		Move_Transfrom(0,168,0,0);
//		delay_ms(800);
//		step_stop();
//		delay_ms(500);
//		step_start();
//		Move_Transfrom(168,0,0,0);
//		delay_ms(3000);
//		step_stop();//ЖўЮЌТы
//		delay_ms(800);
		
	}
} 
