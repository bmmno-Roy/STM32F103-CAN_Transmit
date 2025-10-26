#ifndef __DHT20_H__
#define __DHT20_H__
#include "main.h"


extern unsigned short Humidity;
extern unsigned short Temperature;
HAL_StatusTypeDef DHT20_Init(void);
void DHT20_Read(void);


#endif
