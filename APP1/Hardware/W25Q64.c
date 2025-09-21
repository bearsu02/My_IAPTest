#include "stm32f10x.h"                  // Device header
#include "myspi.h"
#include "w25q64_ins.h"
void W25Q64_Init(void)
{
	MySPI_Init();
}

void W25Q64_ReadID(u8 *MID,u16 *DID)
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_JEDEC_ID);
	*MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID <<= 8;
	*DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	MySPI_Stop();
}

void W25Q64_WriteEnable(void)
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);
	MySPI_Stop();
}

void W25Q64_WaitBusy(void)
{
	u32 TimeOut=100000;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
	while((MySPI_SwapByte(W25Q64_DUMMY_BYTE))&&0x01==1)
	{
		TimeOut--;
		if(TimeOut==0)
		{
			break;
		}
	}
	MySPI_Stop();
}

void W25Q64_PageProgram(u32 Address,u8 *DataArray,u16 Count)
{
	u16 i;
	W25Q64_WaitBusy();//忙前等待，确保写入数据不会被覆盖
	W25Q64_WriteEnable();
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_PAGE_PROGRAM);
	MySPI_SwapByte(Address>>16);
	MySPI_SwapByte(Address>>8);
	MySPI_SwapByte(Address);
	for(i=0;i<Count;i++)
	{
		MySPI_SwapByte(DataArray[i]);
	}
	
	MySPI_Stop();
	W25Q64_WaitBusy();//忙后等待，确保数据写入完成
}

void W25Q64_SectorErase(u32 Address)
{
	W25Q64_WaitBusy();//忙前等待，确保擦除时芯片空闲
	W25Q64_WriteEnable();
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	MySPI_SwapByte(Address>>16);
	MySPI_SwapByte(Address>>8);
	MySPI_SwapByte(Address);
	MySPI_Stop();
	
	W25Q64_WaitBusy();//忙后等待，等待擦除操作完成
}

void W25Q64_ConstantPageProgram()
{
	
}

void W25Q64_ReadData(u32 Address,u8 *DataArray,u32 Count)
{
	u32 i;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_DATA);
	MySPI_SwapByte(Address>>16);
	MySPI_SwapByte(Address>>8);
	MySPI_SwapByte(Address);
	for(i=0;i<Count;i++)
	{
		DataArray[i]=MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	}
	MySPI_Stop();
}