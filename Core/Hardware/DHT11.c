#include "DHT11.h"
#include "main.h"

#define DHT11_PIN GPIO_PIN_1
unsigned char Temp;
unsigned char Hum;
extern TIM_HandleTypeDef htim2;

void F_Delay_us(unsigned short us)
{
  __HAL_TIM_SET_COUNTER(&htim2, 0);  // 計數器歸零
  while (__HAL_TIM_GET_COUNTER(&htim2) < us);  // 等待達到指定微秒數
}

void DHT11_SetPinOutput(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DHT11_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void DHT11_SetPinInput(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DHT11_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


void DHT11_Start(void)
{
  DHT11_SetPinOutput();
  F_Delay_us(20); 
  HAL_GPIO_WritePin(GPIOB, DHT11_PIN, GPIO_PIN_RESET);  // 拉低資料線
  F_Delay_us(18000);  // 延遲 18 ms

  HAL_GPIO_WritePin(GPIOB, DHT11_PIN, GPIO_PIN_SET);  // 拉高資料線
  F_Delay_us(20);     // 延遲 20 us

  DHT11_SetPinInput();  // 設為輸入模式，等待 DHT11 回應
}



unsigned char DHT11_CheckResponse(void)
{
  unsigned char response = 0;
  unsigned short timeout = 0;
  F_Delay_us(40);

  if (!HAL_GPIO_ReadPin(GPIOA, DHT11_PIN)) // DHT11 拉低 80 us
  {  
      F_Delay_us(80);
    if (HAL_GPIO_ReadPin(GPIOA, DHT11_PIN)) // DHT11 拉高 80 us
    {  
      response = 1;
    }
  }

  timeout = 100;  // 重設超時計數
  while (HAL_GPIO_ReadPin(GPIOA, DHT11_PIN))
  {
    if (--timeout == 0)  // 當超時計數歸零時跳出
    {
      response = 0;  // 設定 response 為 0 表示失敗
      break;
    }
    F_Delay_us(1);  // 每次減少計數時增加一點延遲，以防止過快的執行
  }
    return response;
}


unsigned char DHT11_ReadByte(void)
{
  unsigned char i, byte = 0;
  
  for (i = 0; i < 8; i++)
  {
    while (!HAL_GPIO_ReadPin(GPIOA, DHT11_PIN));  // 等待訊號拉高
    F_Delay_us(40);

    if (HAL_GPIO_ReadPin(GPIOA, DHT11_PIN))        // 如果拉高時間超過 40 us，代表資料為 1
    {
      byte |= (1 << (7 - i));
      while (HAL_GPIO_ReadPin(GPIOA, DHT11_PIN));  // 等待訊號拉低
    }
  }
  return byte;
}


void DHT11_Read(void)
{
  unsigned char data[5];
  
  if (DHT11_CheckResponse())
  {
    data[0] = DHT11_ReadByte();
    data[1] = DHT11_ReadByte();
    data[2] = DHT11_ReadByte();
    data[3] = DHT11_ReadByte();
    data[4] = DHT11_ReadByte();
    if (data[4] == (data[0] + data[1] + data[2] + data[3])) // 成功讀取數據
    {
      Temp = data[2];
      Hum  = data[0];
    }
  }
  HAL_Delay(1000);
}








