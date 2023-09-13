#include "allFiles.h"
int op1_Data[3] = {100,120,140};
int op1_State = 1;
int data[6] = {0, 0, 0, 0, 0, 0};
u8 u3_count = 0;
uint8_t  center_x = 0, center_y = 0;
uint8_t color_type = 0;
double  center_x_cm = 0, center_y_cm = 0;
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F4探索者开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/6/10
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif

void uart3_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); 
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure); 
	
	USART_Cmd(USART3, ENABLE);
	USART_ClearFlag(USART3, USART_FLAG_TC);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
	
}

void USART3_IRQHandler(void) 
{
	static int i =0;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
		if (state == PositionCheck_state)
		{
			data[i++] = USART_ReceiveData(USART3);
			if(data[0]!=0xa3) i=0;
			if((i == 2) && (data[1] != 0xb3)) i =0;
			if(i == 6)
			{
				if (data[5] == 0xc3) 
				{
					op1_Data[0] = data[2];
					op1_Data[1] = data[3];
					op1_Data[2] = data[4];
				}
			}
		}
		else
		{
			op1_State = USART_ReceiveData(USART3);
			while(USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET);
		}
	}
}

//串口一

void uart1_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//GPIOA 和 USART1 时钟使能①
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能 GPIOA 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能 USART1 时钟
	//USART_DeInit(USART1); //复位串口 1 ②
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //PA9 复用为 USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //PA10 复用为 USART1
	//USART1_TX PA.9 PA.10 ③
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9 与 GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //速度 50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化 PA9，PA10
	//USART 初始化设置 ④
	USART_InitStructure.USART_BaudRate = bound;//一般设置为 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为 8 位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2; //响应优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ 通道使能
	NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化 VIC 寄存器、
	
	USART_Cmd(USART1, ENABLE); //使能串口 ⑥
}

void USART1_IRQHandler(void) 
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		//op2_Data = USART_ReceiveData(USART1);	
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);  //清除RXNE标志位		
	}
}
void openmv_receive_data(int16_t com_data)
{
	/*循环体变量*/
    uint8_t i;
    /*计数变量*/
    static uint8_t rx_counter_1 = 0;//计数
    /*数据接收数组*/
    static uint16_t rx_buffer_1[10] = { 0 };
    /*数据传输状态位*/
    static uint8_t rx_state = 0;
	
	/*对数据进行校准，判断是否为有效数据*/
    if (rx_state == 0 && com_data == 0x2C)  //0x2c帧头
    {
        rx_state = 1;
        rx_buffer_1[rx_counter_1++] = com_data;

    }

    else if (rx_state == 1 && com_data == 0x12)  //0x12帧头
    {
        rx_state = 2;
        rx_buffer_1[rx_counter_1++] = com_data;

    }
    else if (rx_state == 2)
    {

        rx_buffer_1[rx_counter_1++] = com_data;
        if (rx_counter_1 >= 6 || com_data == 0xFF)   //RxBuffer1接受满了,接收数据结束
        //使用0xFF作为结束位的原因是，避免OpenMV回传的数据与结束位重合导致接收提前结束
        {
            rx_state = 3;
            color_type = rx_buffer_1[rx_counter_1 - 4];
            center_x = rx_buffer_1[rx_counter_1 - 3];
            center_y = rx_buffer_1[rx_counter_1 - 2];
            //    				for(k = 0;k < RxCounter1;k ++)
            //						{
            //							printf("%d ",RxBuffer1[k]);
            //						}
            //						printf("\r\n\n");
            printf("颜色种类: %c   ", color_type);
            printf("中心x坐标：%d   ", center_x);
            printf("中心y坐标：%d\r\n   ", center_y);

        }
    }

    else if (rx_state == 3)//检测是否接受到结束标志
    {
        if (rx_buffer_1[rx_counter_1 - 1] == 0x5B)
        {
            //RxFlag1 = 0;
            rx_counter_1 = 0;
            rx_state = 0;
        }
        else   //接收错误
        {
            rx_state = 0;
            rx_counter_1 = 0;
            for (i = 0; i < 10; i ++)
            {
                rx_buffer_1[i] = 0x00;      //将存放数据数组清零
            }
        }
    }

    else   //接收异常
    {
        rx_state = 0;
        rx_counter_1 = 0;
        for (i = 0; i < 10; i++)
        {
            rx_buffer_1[i] = 0x00;      //将存放数据数组清零
        }
    }
}
