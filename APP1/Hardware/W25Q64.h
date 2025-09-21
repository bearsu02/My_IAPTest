#ifndef __W25Q64_H
#define __W25Q64_H
void W25Q64_Init(void);
void W25Q64_ReadID(u8 *MID,u16 *DID);
void W25Q64_PageProgram(u32 Address,u8 *DataArray,u16 Count);
void W25Q64_SectorErase(u32 Address);
void W25Q64_ReadData(u32 Address,u8 *DataArray,u32 Count);







#endif
