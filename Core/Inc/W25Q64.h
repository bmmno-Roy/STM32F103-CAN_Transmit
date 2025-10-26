#ifndef __W25Q64_H__
#define __W25Q64_H__

#include "main.h"

void W25Q64_Init(void);
void W25Q64_ReadID(unsigned char* Manu, unsigned char* MemoryType, unsigned char* Capacity);
void W25Q64_ReadData(unsigned int page, unsigned char* Buffer);
unsigned char W25Q64_PageProgram(unsigned short page, unsigned char* Buffer);
unsigned char W25Q64_EraseSector(unsigned short Sector);
unsigned char W25Q64_EraseBlock32(unsigned short Block);

#endif