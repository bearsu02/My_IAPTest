#include <stm32f10x.h>                  // Device header
int main(void)
{	
	RCC->APB2ENR |= (1 << 2);	//开启GPIOA时钟

	GPIOA->CRL &= ~(0xF << 0);	//给CRL寄存器的PA0模式位清0
	GPIOA->CRL |= (0X3<<0);     //给CRL寄存器的PA0模式位置位为推挽输出50MHZ
	
	GPIOA->BSRR |= (1<<0);		//PA0端口输出高电平
	
	while (1)
	{
	
	}
}
