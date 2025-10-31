# 目標
本項目基於STM32F103外接TJA1050，發送CAN匯流排上的資料

# 使用
1. 下載原碼
2. IDE為IAR
3. 外接TJA1050 
4. F103採集DHT20的溫濕度與電壓值 發送到CAN 匯流排


# 注意事項
1.TJA1050 使用5V 需另外供電並與電路共地

2.根據需求，需要幾路收發就需要幾個TJA1050

3.因模擬為採集端，且Filter設置為全通，一定要將RxFIFO資料讀出，
  否則會溢出，但讀出可以不處理

4. 根據標準電路要使用 120歐姆的終端電阻，因為電路有3個模組，
   中間的模組記得拆掉120歐姆的電阻。

# 基本設置
Bitrate : 5000000

Sample Point : 87.5%

# 主要程式

#### 發送CAN
```
void CAN_Transmit(CAN_TxHeaderTypeDef *CAN_TxHeader, unsigned char* Data)
{
  uint32_t TxMailbox;
  
  if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan) > 0)
  {
    if(HAL_CAN_AddTxMessage(&hcan, CAN_TxHeader, Data, &TxMailbox) != HAL_OK)
    {
      OLED_ShowString(4, 1, "Send to MailBiox ERROR!\n");
      return;
    }
    unsigned int timeout = 0;
    while (HAL_CAN_IsTxMessagePending(&hcan, TxMailbox))
    {
      if(timeout++ > 100000){
        break;
      }
    }
  }
}
```

#### 中斷接收讀出 但不處理

*或是Filter 可以設置為關閉 那就不用讀出
```
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef CAN_RxHeader;
  unsigned char Data[8];
  
  if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxHeader, Data) == HAL_OK)
  {
    
  }
}
```
# 硬體
![image](https://github.com/bmmno-Roy/F407_FreeRTOS_CAN/blob/main/IMG/hardware.jpg)

# 最終呈現
![image](https://github.com/bmmno-Roy/F407_FreeRTOS_CAN/blob/main/IMG/UART.png)

### cangaroo 監控匯流排資料
![image](https://github.com/bmmno-Roy/F407_FreeRTOS_CAN/blob/main/IMG/cangaroo.png)


