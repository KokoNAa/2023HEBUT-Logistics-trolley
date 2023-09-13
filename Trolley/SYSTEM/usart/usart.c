#include "allFiles.h"
int op1_Data[3] = {100,120,140};
int op1_State = 1;
int data[6] = {0, 0, 0, 0, 0, 0};
u8 u3_count = 0;
uint8_t  center_x = 0, center_y = 0;
uint8_t color_type = 0;
double  center_x_cm = 0, center_y_cm = 0;
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F4̽���߿�����
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/6/10
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
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

//����һ

void uart1_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//GPIOA �� USART1 ʱ��ʹ�ܢ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ�� GPIOA ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ�� USART1 ʱ��
	//USART_DeInit(USART1); //��λ���� 1 ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //PA9 ����Ϊ USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //PA10 ����Ϊ USART1
	//USART1_TX PA.9 PA.10 ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9 �� GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //�ٶ� 50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ�� PA9��PA10
	//USART ��ʼ������ ��
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ 8 λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2; //��Ӧ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ�� VIC �Ĵ�����
	
	USART_Cmd(USART1, ENABLE); //ʹ�ܴ��� ��
}

void USART1_IRQHandler(void) 
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		//op2_Data = USART_ReceiveData(USART1);	
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);  //���RXNE��־λ		
	}
}
void openmv_receive_data(int16_t com_data)
{
	/*ѭ�������*/
    uint8_t i;
    /*��������*/
    static uint8_t rx_counter_1 = 0;//����
    /*���ݽ�������*/
    static uint16_t rx_buffer_1[10] = { 0 };
    /*���ݴ���״̬λ*/
    static uint8_t rx_state = 0;
	
	/*�����ݽ���У׼���ж��Ƿ�Ϊ��Ч����*/
    if (rx_state == 0 && com_data == 0x2C)  //0x2c֡ͷ
    {
        rx_state = 1;
        rx_buffer_1[rx_counter_1++] = com_data;

    }

    else if (rx_state == 1 && com_data == 0x12)  //0x12֡ͷ
    {
        rx_state = 2;
        rx_buffer_1[rx_counter_1++] = com_data;

    }
    else if (rx_state == 2)
    {

        rx_buffer_1[rx_counter_1++] = com_data;
        if (rx_counter_1 >= 6 || com_data == 0xFF)   //RxBuffer1��������,�������ݽ���
        //ʹ��0xFF��Ϊ����λ��ԭ���ǣ�����OpenMV�ش������������λ�غϵ��½�����ǰ����
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
            printf("��ɫ����: %c   ", color_type);
            printf("����x���꣺%d   ", center_x);
            printf("����y���꣺%d\r\n   ", center_y);

        }
    }

    else if (rx_state == 3)//����Ƿ���ܵ�������־
    {
        if (rx_buffer_1[rx_counter_1 - 1] == 0x5B)
        {
            //RxFlag1 = 0;
            rx_counter_1 = 0;
            rx_state = 0;
        }
        else   //���մ���
        {
            rx_state = 0;
            rx_counter_1 = 0;
            for (i = 0; i < 10; i ++)
            {
                rx_buffer_1[i] = 0x00;      //�����������������
            }
        }
    }

    else   //�����쳣
    {
        rx_state = 0;
        rx_counter_1 = 0;
        for (i = 0; i < 10; i++)
        {
            rx_buffer_1[i] = 0x00;      //�����������������
        }
    }
}
