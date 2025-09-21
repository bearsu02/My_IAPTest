#include "main.h"
#include "usart.h"
#include "stm32f10x_flash.h"
#include "bsp_key.h"
#include "led.h"
#include "string.h"
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t EraseCounter = 0x0;				//循环擦除APP1时的参数
uint32_t NbrOfPage = 0;						//写入APP1数据包的总页数
FLASH_Status FLASHStatus = FLASH_COMPLETE;	//写入允许标志位
uint32_t FileSize=0;

#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 #define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
 #define FLASH_SIZE                        (0x10000)  /* 64 KBytes */
#elif defined STM32F10X_CL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
#elif defined STM32F10X_HD || defined (STM32F10X_HD_VL)
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x80000)  /* 512 KBytes */
#elif defined STM32F10X_XL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x100000) /* 1 MByte */
#else 
 #error "Please select first the STM32 device to be used (in stm32f10x.h)"    
#endif
/*
bootload   app1      app2      total
 0x3000   0x6800    0x6800    0x10000
   12k      26k       26k       64k
*/
#define BL_SIZE 							0x3000
#define APP_SIZE 							((FLASH_SIZE-BL_SIZE)/2)
#define ApplicationAddress    (0x08000000+BL_SIZE)
#define UpdateFlagAddress			(ApplicationAddress-4)//升级标志位存储地址，BootLoader区域最后一个word
#define FileSizeAddress				(ApplicationAddress-8)//文件大小 地址为bootloader的倒数第二个word

uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
  uint32_t pagenumber = 0x0;
  uint32_t size = Size;

  if ((size % PAGE_SIZE) != 0)
  {
    pagenumber = (size / PAGE_SIZE) + 1;
  }
  else
  {
    pagenumber = size / PAGE_SIZE;
  }
  return pagenumber;
}

u8 Copy_APP2_TO_APP1(void)
{
	uint32_t i=0,NumOfWord=0;
	uint32_t ReadAddress = ApplicationAddress + APP_SIZE;		//APP2的起始地址
	uint32_t WriteAddress = ApplicationAddress;					//APP1地址
	
	__set_PRIMASK(1); 											//关总中断
	FLASH_Unlock();												//解锁Flash

	FileSize = *(__IO uint32_t*)FileSizeAddress;				//获取数据包大小

	NbrOfPage = FLASH_PagesMask(FileSize);						//分页
	//开始擦除APP1区域
	for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
	{
		FLASHStatus = FLASH_ErasePage(ApplicationAddress + (PAGE_SIZE * EraseCounter));
	}
	//擦除错误
	if(FLASHStatus != FLASH_COMPLETE)
	{
		printf("FLASH_ErasePage error\r\n");
		return 0;
	}
	//擦除完成后打印
	printf("FLASH_ErasePage OK\r\n");
	//如果数据大小不能整除4加1（filesize单位为字）
	if(FileSize%4)
	{
		NumOfWord= FileSize/4+1;
	}
	else
	{
		NumOfWord= FileSize/4;
	}
	//准备将数据写入APP1区域
	for(i = 0; i < NumOfWord; i++)
	{
		FLASH_ProgramWord(WriteAddress, *(uint32_t*)ReadAddress);
		if (*(uint32_t*)WriteAddress != *(uint32_t*)ReadAddress)
		{
			printf("Program Word ERROR %d\r\n",i);
			return 0;
		}
		WriteAddress += 4;
		ReadAddress += 4;
	}
	printf("ProgramWord OK\r\n");
	
	FLASH_Lock();
  __set_PRIMASK(0);
	return 1;
}
/*******************************************************************************
						BOOTLOADER主程序
*******************************************************************************/
extern volatile u16 Rx_Sum;			
extern volatile u8 Rx_Buffer[Rx_MaxSize];
u16 Last_Count=0;
u16 Current_Count=0;
u8 Update_Flag=0,Have_Packet=0;
int main(void)
{
	u8 retry=5;
	memset((void*)Rx_Buffer,0,sizeof(Rx_Buffer));
	Key_GPIO_Config();
	USART1_Config(115200);
	LED_Init();
	printf("DEBUG-->---------------------------\r\n");
	printf("DEBUG-->                           \r\n");
	printf("DEBUG-->       BOOTLOADER run!     \r\n");
	printf("DEBUG-->                           \r\n");
	printf("DEBUG-->---------------------------\r\n");
	
	printf("UpdateFlag : %08X    ",*(__IO uint32_t*)UpdateFlagAddress);	//更新标志位
	
	while(1)
	{
		Last_Count = Current_Count;
		Current_Count = Rx_Sum;
		//检测数据包大小
		if((Last_Count==Current_Count)&&(Rx_Sum!=0))
		{
			Update_Flag=1;
		}
		if((Update_Flag==1)&&(Have_Packet==1))
		{
			Have_Packet=0;
			printf("Total Packet is %d Byte\r\n",Rx_Sum);
			
		}
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN)==KEY_ON)
		{
			__disable_irq(); 							//关中断，准备写入
			uint32_t writeAddr = ApplicationAddress + APP_SIZE;
			uint32_t i, numWord;
			uint32_t *pBuf = (uint32_t*)Rx_Buffer;
			FLASH_Unlock();								//解锁Flash准备擦除
			
			uint32_t pages = FLASH_PagesMask(Rx_Sum); 	// 计算需要擦除的页数
			for(i = 0; i < pages; i++) 
			{
				FLASH_ErasePage(writeAddr + i * PAGE_SIZE);	//擦除APP2地址内的数据
			}
			
			// 写入数据
			numWord = (Rx_Sum + 3) / 4; 					// 向上取整到 word
			for(i = 0; i < numWord; i++)
			{
				FLASH_ProgramWord(writeAddr, *pBuf++);		//将数据包循环写入APP2地址
				writeAddr += 4;
			}
			//擦除倒数第二个word，注意此处将直接擦除一页，如有其他需要需重新对BOOTLOADER分区
			FLASH_ErasePage(FileSizeAddress); 			
			FLASH_ProgramWord(FileSizeAddress, Rx_Sum); 	//写入数据包大小
			
			FLASH_ProgramWord(UpdateFlagAddress,0xAAAAAAAA); //写入更新标志位
			
			FLASH_Lock();									//数据包重新上锁
			Rx_Sum = 0;
			printf("APP2 Receive Packet Successful!\r\n");	
		}
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN)==KEY_ON)
		{
			if((*(__IO uint32_t*)UpdateFlagAddress) == 0xAAAAAAAA)
			{
				printf("Start Copy App2 To App1!\r\n");
				Copy_APP2_TO_APP1();
				LED1_OFF();
			}
		}
		//升级程序后则直接跳转APP1
		if(Key_Scan(KEY3_GPIO_PORT,KEY3_GPIO_PIN)==KEY_ON)
		{
			if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
			{
				printf("BOOTLOADER-->  Jump to APP1!\r\n");
				/* 跳转至APP1代码部分 */
				JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);//跳转到程序入口
				Jump_To_Application = (pFunction) JumpAddress;			 //将地址转为函数指针
				/* 将主栈指针设置为APP1的第一个字 */
				__set_MSP(*(__IO uint32_t*) ApplicationAddress);
				Jump_To_Application();									 //执行APP1的复位函数，开始执行APP1
			}
		}
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/




