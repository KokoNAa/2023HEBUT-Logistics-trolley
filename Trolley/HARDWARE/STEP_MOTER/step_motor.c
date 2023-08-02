#include "allFiles.h"

uint16_t CCR1 = 20000;

speedRampData srd= {STOP,0,0,0,0,0,0};


void step_motor_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ST1_DIR_GPIO_PIN  | ST1_EN_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����(�������ӷ���0Ϊ��Ч)
    GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
	
}

void TIM3_IRQHandler(void)
{
	
}

