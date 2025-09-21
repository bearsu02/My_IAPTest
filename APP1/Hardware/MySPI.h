#ifndef __MYSPI_H
#define __MYSPI_H

//GPIO引脚初始化
#define		SPI_CS_Pin		GPIO_Pin_12
#define		SPI_SCK_PIN		GPIO_Pin_13
#define		SPI_MISO_PIN	GPIO_Pin_14
#define		SPI_MOSI_PIN	GPIO_Pin_15



#define		SPI_SCK_MODE	GPIO_Mode_AF_PP
#define		SPI_MOSI_MODE	GPIO_Mode_AF_PP
#define     SPI_MISO_MODE	GPIO_Mode_IPU
#define     SPI_CS_MODE		GPIO_Mode_Out_PP

#define		SPI_SCK_SPEED		GPIO_Speed_50MHz
#define		SPI_MOSI_SPEED		GPIO_Speed_50MHz	
#define     SPI_MISO_SPEED		GPIO_Speed_50MHz		
#define     SPI_CS_SPEED		GPIO_Speed_50MHz

//函数声明

void MySPI_W_CS(u8 BitValue);
void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
u8 MySPI_SwapByte(u8 ByteSend);

#endif
