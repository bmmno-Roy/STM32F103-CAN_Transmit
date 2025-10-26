#include "DHT20.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c2;
unsigned short Humidity;
unsigned short Temperature;
#define DHT20_ADDR      0x38 << 1 // 地址左移 1



HAL_StatusTypeDef DHT20_Register_Init()
{
  unsigned char init_data[2];

  init_data[0] = 0x1B;
  init_data[1] = 0x00; // 寫入初始化值
  HAL_I2C_Master_Transmit(&hi2c2, DHT20_ADDR, init_data, 2, HAL_MAX_DELAY);

  init_data[0] = 0x1C;
  init_data[1] = 0x00; // 寫入初始化值
  HAL_I2C_Master_Transmit(&hi2c2, DHT20_ADDR, init_data, 2, HAL_MAX_DELAY);

  init_data[0] = 0x1E;
  init_data[1] = 0x00; // 寫入初始化值
  if(HAL_I2C_Master_Transmit(&hi2c2, DHT20_ADDR, init_data, 2, HAL_MAX_DELAY) != HAL_OK){
    return HAL_ERROR;
  }
  return HAL_OK;
}

HAL_StatusTypeDef DHT20_Init(void)
{
  unsigned char sendBuffer = 0x71;
  unsigned char readBuffer;
  HAL_Delay(40);
  HAL_I2C_Master_Transmit(&hi2c2, DHT20_ADDR, &sendBuffer, 1, HAL_MAX_DELAY);
  HAL_I2C_Master_Receive(&hi2c2, DHT20_ADDR, &readBuffer, 1, HAL_MAX_DELAY);
  
  if((readBuffer & 0x18) != 0x18){
    return HAL_ERROR;
  }
  return HAL_OK;
}


void DHT20_Read(void)
{
  unsigned char sendBuffer[3] = {0xAC, 0x33, 0x00};
  unsigned char data[6] = {0};
  unsigned int HumTemp = 0 , TemperTemp = 0;
  
  HAL_I2C_Master_Transmit(&hi2c2, DHT20_ADDR, sendBuffer, 3, HAL_MAX_DELAY);
  HAL_Delay(75);
  
  HAL_I2C_Master_Receive(&hi2c2, DHT20_ADDR, data, 6, HAL_MAX_DELAY);
  
  if((data[0] & 0x80) == 0x00)
  {
    HumTemp = (data[1] << 12) + (data[2] << 8) + (data[3] >> 4);
    Humidity = (unsigned short)((HumTemp * 100.0) / (1 << 20)*100);
    DataBuff[2] = Humidity >> 8;
    DataBuff[3] = Humidity & 0xFF;
    
    TemperTemp = ((data[3] & 0x0F) << 16) + (data[4] << 8) + data[5];
    Temperature = (unsigned short)((TemperTemp* 200.0/ (1 << 20)  - 50)*100);
    DataBuff[4] = Temperature >> 8;
    DataBuff[5] = Temperature & 0xFF;
  }
}








