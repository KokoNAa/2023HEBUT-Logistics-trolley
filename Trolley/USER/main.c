#include "allFiles.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	delay_init(84);
	ST_PWM_Init();
	step_motor_init();
	while(1)
	{
		
	}

}

