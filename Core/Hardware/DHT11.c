#include "DHT11.h"
#include "main.h"

#define DHT11_PIN GPIO_PIN_1
unsigned char Temp;
unsigned char Hum;
extern TIM_HandleTypeDef htim2;

void F_Delay_us(unsigned short us)
{
  __HAL_TIM_SET_COUNTER(&htim2, 0);  // �p�ƾ��k�s
  while (__HAL_TIM_GET_COUNTER(&htim2) < us);  // ���ݹF����w�L���
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
  HAL_GPIO_WritePin(GPIOB, DHT11_PIN, GPIO_PIN_RESET);  // �ԧC��ƽu
  F_Delay_us(18000);  // ���� 18 ms

  HAL_GPIO_WritePin(GPIOB, DHT11_PIN, GPIO_PIN_SET);  // �԰���ƽu
  F_Delay_us(20);     // ���� 20 us

  DHT11_SetPinInput();  // �]����J�Ҧ��A���� DHT11 �^��
}



unsigned char DHT11_CheckResponse(void)
{
  unsigned char response = 0;
  unsigned short timeout = 0;
  F_Delay_us(40);

  if (!HAL_GPIO_ReadPin(GPIOA, DHT11_PIN)) // DHT11 �ԧC 80 us
  {  
      F_Delay_us(80);
    if (HAL_GPIO_ReadPin(GPIOA, DHT11_PIN)) // DHT11 �԰� 80 us
    {  
      response = 1;
    }
  }

  timeout = 100;  // ���]�W�ɭp��
  while (HAL_GPIO_ReadPin(GPIOA, DHT11_PIN))
  {
    if (--timeout == 0)  // ��W�ɭp���k�s�ɸ��X
    {
      response = 0;  // �]�w response �� 0 ��ܥ���
      break;
    }
    F_Delay_us(1);  // �C����֭p�ƮɼW�[�@�I����A�H����L�֪�����
  }
    return response;
}


unsigned char DHT11_ReadByte(void)
{
  unsigned char i, byte = 0;
  
  for (i = 0; i < 8; i++)
  {
    while (!HAL_GPIO_ReadPin(GPIOA, DHT11_PIN));  // ���ݰT���԰�
    F_Delay_us(40);

    if (HAL_GPIO_ReadPin(GPIOA, DHT11_PIN))        // �p�G�԰��ɶ��W�L 40 us�A�N���Ƭ� 1
    {
      byte |= (1 << (7 - i));
      while (HAL_GPIO_ReadPin(GPIOA, DHT11_PIN));  // ���ݰT���ԧC
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
    if (data[4] == (data[0] + data[1] + data[2] + data[3])) // ���\Ū���ƾ�
    {
      Temp = data[2];
      Hum  = data[0];
    }
  }
  HAL_Delay(1000);
}








