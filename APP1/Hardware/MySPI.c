#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "myspi.h"
//写CS引脚
void MySPI_W_CS(u8 BitValue)
{
	GPIO_WriteBit(GPIOB,SPI_CS_Pin,BitValue);
}

void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	//GPIO初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode=SPI_SCK_MODE;
	GPIO_InitStructure.GPIO_Pin=SPI_MOSI_PIN|SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed=SPI_SCK_SPEED;
	GPIO_Init(GPIOB,&GPIO_InitStructure);					//初始化输出引脚
	
	GPIO_InitStructure.GPIO_Mode=SPI_MISO_MODE;
	GPIO_InitStructure.GPIO_Pin=SPI_MISO_PIN;
	GPIO_Init(GPIOB,&GPIO_InitStructure);					//初始化输入引脚
	
	GPIO_InitStructure.GPIO_Mode=SPI_CS_MODE;
	GPIO_InitStructure.GPIO_Pin=SPI_CS_Pin;
	GPIO_Init(GPIOB,&GPIO_InitStructure);					//初始化片选引脚
	
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//主机模式
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//双线全双工
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8位数据帧
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//高位先行
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;//分频系数
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//CPOL=0
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//CPHA=0
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//软件NSS
	SPI_InitStructure.SPI_CRCPolynomial = 7;//不用选默认值7
	SPI_Init(SPI2, &SPI_InitStructure);
	
	SPI_Cmd(SPI2, ENABLE);
	
	MySPI_W_CS(1);
	
}

void MySPI_Start(void)
{
	MySPI_W_CS(0);
}

void MySPI_Stop(void)
{
	MySPI_W_CS(1);
}

u8 MySPI_SwapByte(u8 ByteSend)
{
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)!=SET);	//发送寄存器非空时跳出循环
	
	SPI_I2S_SendData(SPI2,ByteSend);							//写入操作会清除发送非空标志位
	
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)!=SET);	//接受寄存器非空时跳出循环
		
	return SPI_I2S_ReceiveData(SPI2);							//读出来之后清除读非空标志位
}

