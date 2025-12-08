/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file drv_adapter_port_temphumioment.c
 * 
 * @par dependencies 
 * - i2c_port.h
 * - i2c.h
 * - iic_hal.h
 * - os_freertos
 * - osal_mutex
 * @author Simon | R&D Dept. | EternalChip 立芯嵌入式
 * 
 * @brief Provide the HAL APIs of AHT21 and corresponding opetions.
 * 
 * Processing flow:
 * 
 * 2.Then the users could all the IOs from instances of bsp_aht21_driver_t.
 * 
 * @version V1.0 2023-12-03
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/
//******************************** Includes *********************************//
#include "Protocol_driver_Port.h"

#include "DMA_device.h" 
#include "stm32f1xx_hal.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//

//******************************** Defines **********************************//

//******************************** Variables ********************************//


//// DMA设备的方法实现
//static int DMA_device_init(BaseDevice* self) {
//// 定义DMA句柄
// DMA_HandleTypeDef hdma_usart1_tx;
// // DMA初始化函数

//   __HAL_RCC_DMA1_CLK_ENABLE(); // 使能DMA1时钟
//   hdma_usart1_tx.Instance = DMA1_Channel4; // 指定DMA通道
//   hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH; // 内存到外设方向
//   hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不递增
//   hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE; // 内存地址递增
//   hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // 外设数据宽度为字节
//   hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; // 内存数据宽度为字节
//   hdma_usart1_tx.Init.Mode = DMA_NORMAL; // 正常模式
//   hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW; // 低优先级
//   if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   // 关联DMA通道与串口1_TX
//   __HAL_LINKDMA(&huart1, hdmatx, hdma_usart1_tx);
//    return 0;
//}

//static int DMA_device_deinit(BaseDevice* self) {

//    return 0;
//}

