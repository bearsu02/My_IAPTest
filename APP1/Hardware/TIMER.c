#include <stm32f10x.h>
#include "led.h"
void Timer_Init(u32 Period_us)
{
	LED_Init();
	/*开启定时器时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 启用TIM1时钟
	
	TIM_InternalClockConfig(TIM3);//开启内部时钟
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 10000-1; // 自动重装载值
	TIM_TimeBaseStructure.TIM_Prescaler = 7200-1; // 预分频系数
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	
	TIM_ClearFlag(TIM3,TIM_FLAG_Update); 
	TIM_ITConfig(TIM3,TIM_FLAG_Update,ENABLE);
	
	
	/*NVIC_Config*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3,ENABLE);
}



void TIM3_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM3,TIM_IT_Update)==SET)
	{
		LED2_Turn();
		TIM_ClearFlag(TIM3,TIM_FLAG_Update); 
	}
}
