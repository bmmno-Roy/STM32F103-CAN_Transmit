/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
HAL_StatusTypeDef CAN_SetFilter(void);
/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 6;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
  CAN_SetFilter();
  if(HAL_CAN_Start(&hcan) != HAL_OK){
    OLED_ShowString(1, 1, "can start fail!:");
    while(1);
  }
  printf("Can Start");
  __HAL_CAN_ENABLE_IT(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
HAL_StatusTypeDef CAN_SetFilter(void)
{
  CAN_FilterTypeDef CAN_Filter;
  CAN_Filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  CAN_Filter.FilterMode = CAN_FILTERMODE_IDMASK;
  CAN_Filter.FilterScale = CAN_FILTERSCALE_32BIT;
  
  CAN_Filter.FilterBank = 0;
  CAN_Filter.FilterIdHigh = 0x0000;
  CAN_Filter.FilterIdLow = 0x0000;
  CAN_Filter.FilterMaskIdHigh = 0x0000;
  CAN_Filter.FilterMaskIdLow = 0x0000;
  
  CAN_Filter.FilterActivation = CAN_FILTER_ENABLE;
  CAN_Filter.SlaveStartFilterBank = 14;
  
  return HAL_CAN_ConfigFilter(&hcan, &CAN_Filter);
}

/**
  * @brief  CAN_Transmit
  * @param  This parameter must be a number between Min_Data = 0 and Max_Data = 0x7FF
  * @param  CAN_remote_transmission_request
  * @param  This parameter must be a number between Min_Data = 0 and Max_Data = 8
  * @param  Data[]
  * @retval None
  */

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

void CAN_Receive(void)
{
  if(HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0))
  {
    CAN_RxHeaderTypeDef CAN_RxHeader;
    unsigned char Data[8];
    if(HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &CAN_RxHeader, Data) == HAL_OK)
    {
      if(CAN_RxHeader.IDE == CAN_ID_STD){
        OLED_ShowHexNum(1, 5, CAN_RxHeader.StdId, 3);
      }
      else if(CAN_RxHeader.IDE == CAN_ID_EXT){
        OLED_ShowHexNum(1, 5, CAN_RxHeader.ExtId, 8);
      }
      OLED_ShowHexNum(2, 4, CAN_RxHeader.RTR, 2);
      OLED_ShowHexNum(2, 9, CAN_RxHeader.DLC, 2);

      for(int i = 0; i<CAN_RxHeader.DLC; i++){
        OLED_ShowHexNum(4, i*3+1, Data[i], 2);
      }
    }
  }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef CAN_RxHeader;
  unsigned char Data[8];
  
  if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxHeader, Data) == HAL_OK)
  {
    
  }
}

/* USER CODE END 1 */
