#ifndef __DHT11_H__
#define __DHT11_H__

extern unsigned char Temp;
extern unsigned char Hum;
void F_Delay_us(unsigned short us);
void DHT11_SetPinOutput(void);
void DHT11_SetPinInput(void);
void DHT11_Start(void);
unsigned char DHT11_CheckResponse(void);
unsigned char DHT11_ReadByte(void);
void DHT11_Read(void);


#endif