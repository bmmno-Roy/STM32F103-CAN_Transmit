#include "OLED.h"
#include "OLED_Font.h"

static void OLED_W_SCK_H(void)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
}

static void OLED_W_SCK_L(void)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
}

static void OLED_W_SDA_H(void)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
}

static void OLED_W_SDA_L(void)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
}


static void OLED_I2C_Init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  OLED_W_SCK_H();
  OLED_W_SDA_H();
}

static void OLED_I2C_Start(void)
{
  OLED_W_SDA_H();
  OLED_W_SCK_H();
  OLED_W_SDA_L();
  OLED_W_SCK_L();
}


static void OLED_I2C_Stop(void)
{
  OLED_W_SDA_L();
  OLED_W_SCK_H();
  OLED_W_SDA_H();
}

static void OLED_I2C_SendByte(uint8_t Byte)
{
  uint8_t i;
  for (i = 0; i < 8; i++)
  {
    if((Byte & (0x80 >> i)) >= 1){
      OLED_W_SDA_H();
    }
    else OLED_W_SDA_L();
    OLED_W_SCK_H();
    OLED_W_SCK_L();
  }
  OLED_W_SCK_H();
  OLED_W_SCK_L();
}


static void OLED_WriteCommand(uint8_t Command)
{
  OLED_I2C_Start();
  OLED_I2C_SendByte(0x78);
  OLED_I2C_SendByte(0x00);
  OLED_I2C_SendByte(Command); 
  OLED_I2C_Stop();
}


static void OLED_WriteData(uint8_t Data)
{
  OLED_I2C_Start();
  OLED_I2C_SendByte(0x78);
  OLED_I2C_SendByte(0x40);	
  OLED_I2C_SendByte(Data);
  OLED_I2C_Stop();
}


static void OLED_SetCursor(uint8_t Y, uint8_t X)
{
  OLED_WriteCommand(0xB0 | Y);
  OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));
  OLED_WriteCommand(0x00 | (X & 0x0F));	
}

void OLED_Clear(void)
{  
  uint8_t i, j;
  for (j = 0; j < 8; j++)
  {
    OLED_SetCursor(j, 0);
    for(i = 0; i < 128; i++)
    {
      OLED_WriteData(0x00);
    }
  }
}


void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
  uint8_t i;
  OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
  for (i = 0; i < 8; i++)
  {
    OLED_WriteData(OLED_F8x16[Char - ' '][i]);
  }
  OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
  for (i = 0; i < 8; i++)
  {
    OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);
  }
}


void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
  uint8_t i;
  for (i = 0; String[i] != '\0'; i++)
  {
    OLED_ShowChar(Line, Column + i, String[i]);
  }
}


uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
  uint32_t Result = 1;
  while (Y--)
  {
    Result *= X;
  }
  return Result;
}


void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
  uint8_t i;
  for (i = 0; i < Length; i++)							
  {
    OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
  }
}


void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
  uint8_t i;
  uint32_t Number1;
  if (Number >= 0)
  {
    OLED_ShowChar(Line, Column, '+');
    Number1 = Number;
  }
  else
  {
    OLED_ShowChar(Line, Column, '-');
    Number1 = -Number;
  }
  for (i = 0; i < Length; i++)							
  {
    OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
  }
}


void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
  uint8_t i, SingleNumber;
  for (i = 0; i < Length; i++)							
  {
    SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
    if (SingleNumber < 10)
    {
      OLED_ShowChar(Line, Column + i, SingleNumber + '0');
    }
    else
    {
      OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
    }
  }
}


void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
  uint8_t i;
  for (i = 0; i < Length; i++)							
  {
    OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
  }
}


void OLED_Init(void)
{
  uint32_t i, j;
  
  for (i = 0; i < 1000; i++)
  {
    for (j = 0; j < 1000; j++);
  }
  
  OLED_I2C_Init();
  
  OLED_WriteCommand(0xAE);
  
  OLED_WriteCommand(0xD5);
  OLED_WriteCommand(0x80);
  
  OLED_WriteCommand(0xA8);
  OLED_WriteCommand(0x3F);
  
  OLED_WriteCommand(0xD3);
  OLED_WriteCommand(0x00);
  
  OLED_WriteCommand(0x40);
  
  OLED_WriteCommand(0xA1);
  
  OLED_WriteCommand(0xC8);

  OLED_WriteCommand(0xDA);
  OLED_WriteCommand(0x12);
  
  OLED_WriteCommand(0x81);
  OLED_WriteCommand(0xCF);

  OLED_WriteCommand(0xD9);
  OLED_WriteCommand(0xF1);

  OLED_WriteCommand(0xDB);
  OLED_WriteCommand(0x30);

  OLED_WriteCommand(0xA4);

  OLED_WriteCommand(0xA6);

  OLED_WriteCommand(0x8D);
  OLED_WriteCommand(0x14);

  OLED_WriteCommand(0xAF);
          
  OLED_Clear();	
}




