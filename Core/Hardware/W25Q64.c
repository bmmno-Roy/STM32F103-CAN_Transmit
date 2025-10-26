#include "W25Q64.h"

#define PAGE_SIZE       0x100   // 256Bytes
#define SECTOR_SIZE     0x1000  // 4096Bytes
#define BLOCK32_SIZE    0x8000  // 32KB
#define BLOCK64_SIZE    0x10000 // 65535Bytes
extern SPI_HandleTypeDef hspi1;


void W25Q64_NSS_ENABLE(void)
{
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
}

void W25Q64_NSS_DISABLE(void)
{
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}


void W25Q64_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

void F_W25Q64_ReadID(unsigned char* Manu, unsigned char* MemoryType, unsigned char* Capacity)
{
  unsigned char CMD = 0x9F;
  unsigned char id[3];

  W25Q64_NSS_ENABLE();
  HAL_SPI_Transmit(&hspi1, &CMD, 1, 2);
  HAL_SPI_Receive(&hspi1, id, 3, HAL_MAX_DELAY);
  W25Q64_NSS_DISABLE();
  
  *Manu       = id[0];
  *MemoryType = id[1];
  *Capacity   = id[2];
}


void F_W25Q64_ReadData(unsigned int page, unsigned char* Buffer)
{
  unsigned int Address = page * PAGE_SIZE;
  unsigned char Address1 = (Address >> 16) & 0xFF;
  unsigned char Address2 = (Address >> 8)  & 0xFF;
  unsigned char Address3 = Address && 0xFF;
  unsigned char CMD[4] = {0x03, Address1, Address2, Address3};
  
  W25Q64_NSS_ENABLE();
  HAL_SPI_Transmit(&hspi1, CMD, 4, HAL_MAX_DELAY);
  HAL_SPI_Receive(&hspi1, Buffer, PAGE_SIZE, HAL_MAX_DELAY);
  W25Q64_NSS_DISABLE();
}


unsigned char F_W25Q64_Flash_Busy(void)
{
  unsigned char CMD = 0x05;
  unsigned char status;
  
  W25Q64_NSS_ENABLE();
  HAL_SPI_Transmit(&hspi1, &CMD, 1, HAL_MAX_DELAY);
  HAL_SPI_Receive(&hspi1, &status, 1, HAL_MAX_DELAY);
  W25Q64_NSS_DISABLE();
  
  return (status & 0x01);  // WIP bit
}


unsigned char F_W25Q64_WaitBusy(unsigned int waitTime)
{
  while(F_W25Q64_Flash_Busy())
  {
    waitTime--;
    if(waitTime == 0){
      return 1;
    }
  }
  return 0;
}
  
void F_W25Q64_WriteEnable(void)
{
  unsigned char CMD = 0x06;
  
  W25Q64_NSS_ENABLE();
  HAL_SPI_Transmit(&hspi1, &CMD, 1, HAL_MAX_DELAY);
  W25Q64_NSS_DISABLE();
}


unsigned char F_W25Q64_PageProgram(unsigned short page, unsigned char* Buffer)
{
  unsigned int Address = page * PAGE_SIZE;
  unsigned char Address1 = (Address >> 16) & 0xFF;
  unsigned char Address2 = (Address >> 8)  & 0xFF;
  unsigned char Address3 = Address && 0xFF;
  unsigned char CMD[4] = {0x02, Address1, Address2, Address3};
  
  if(F_W25Q64_WaitBusy(1000)){
    return 1;
  }
  F_W25Q64_WriteEnable();
  W25Q64_NSS_ENABLE();
  HAL_SPI_Transmit(&hspi1, CMD, 4, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi1, Buffer, PAGE_SIZE, HAL_MAX_DELAY);
  W25Q64_NSS_DISABLE();
  
  if(F_W25Q64_WaitBusy(1000)){
    return 1;
  }
  return 0;
}


unsigned char F_W25Q64_EraseSector(unsigned short Sector)
{/* Sector = Address / BLOCK32_SIZE 
    if 0x7F0000/ 0x8000 = 254       */
  unsigned int Address = Sector * SECTOR_SIZE;
  unsigned char Address1 = (Address >> 16) & 0xFF;
  unsigned char Address2 = (Address >> 8)  & 0xFF;
  unsigned char Address3 = Address & 0xFF;
  unsigned char CMD[4] = {0x20, Address1, Address2, Address3};
  if(F_W25Q64_WaitBusy(1000)){
    return 1;
  }
  F_W25Q64_WriteEnable();
  W25Q64_NSS_ENABLE();
  HAL_SPI_Transmit(&hspi1, CMD, 4, HAL_MAX_DELAY);
  W25Q64_NSS_DISABLE();
  if(F_W25Q64_WaitBusy(1000)){
    return 1;
  }
  return 0;
}


unsigned char F_W25Q64_EraseBlock32(unsigned short Block)
{
  /* Block = Address / BLOCK32_SIZE 
     if 0x7F0000/ 0x8000 = 254      */
  unsigned int Address = Block * BLOCK32_SIZE;
  unsigned char Address1 = (Address >> 16) & 0xFF;
  unsigned char Address2 = (Address >> 8)  & 0xFF;
  unsigned char Address3 = Address & 0xFF;
  unsigned char CMD[4] = {0x52, Address1, Address2, Address3};
  if(F_W25Q64_WaitBusy(1000)){
    return 1;
  }
  F_W25Q64_WriteEnable();
  W25Q64_NSS_ENABLE();
  HAL_SPI_Transmit(&hspi1, CMD, 4, HAL_MAX_DELAY);
  W25Q64_NSS_DISABLE();
  if(F_W25Q64_WaitBusy(1000)){
    return 1;
  }
  return 0;
}


unsigned char F_W25Q64_EraseBlock64(unsigned short Block)
{
  /* Block = Address / BLOCK65_SIZE 
     if 0x7F0000/ 0x8000 = 254      */
  unsigned int Address = Block * BLOCK64_SIZE;
  unsigned char Address1 = (Address >> 16) & 0xFF;
  unsigned char Address2 = (Address >> 8)  & 0xFF;
  unsigned char Address3 = Address & 0xFF;
  unsigned char CMD[4] = {0xD8, Address1, Address2, Address3};
  if(F_W25Q64_WaitBusy(1000)){
    return 1;
  }
  F_W25Q64_WriteEnable();
  W25Q64_NSS_ENABLE();
  HAL_SPI_Transmit(&hspi1, CMD, 4, HAL_MAX_DELAY);
  W25Q64_NSS_DISABLE();
  if(F_W25Q64_WaitBusy(1000)){
    return 1;
  }
  return 0;
}




