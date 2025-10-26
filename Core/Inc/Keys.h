#ifndef __Keys_H__
#define __Keys_H__

#include "main.h"
typedef enum {
    Key_NONE,
    Key_1,
    Key_2,
} KEYS;


void Keys_Init(void);
KEYS Key_ScanKeys(void);
unsigned char Key_BootKey(void);



#endif