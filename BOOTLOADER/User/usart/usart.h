#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>

#define  USART1_TX_GPIO_PORT       GPIOA   
#define  USART1_TX_GPIO_PIN        GPIO_Pin_9
#define  USART1_RX_GPIO_PORT       GPIOA
#define  USART1_RX_GPIO_PIN        GPIO_Pin_10


#define DEBUG_USART   					USART1	 
#define DEBUG_USART_IRQN				USART1_IRQn
#define DEBUG_USART_IRQHandler			USART1_IRQHandler

#define Rx_MaxSize	1024			//数据包大小

void USART1_Config(u32 BAUDRATE);
void USART2_Config(u32 BAUDRATE);
void USART3_Config(u32 BAUDRATE);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
unsigned char Usart_ReceivByte(USART_TypeDef * pUSARTx);
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);
void UARTWrite(u8 *Data, u16 NByte,u8 port);
#endif /* __USART_H*/

