/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file ec_bsp_externflash_i_xxx_handler.h
 * 
 * @par dependencies 
 * 
 * 
 * @author Simon | R&D Dept. | EternalChip 立芯嵌入式
 * 
 * @brief 
 * 
 * Processing flow:
 * 
 * Call directly.
 * 
 * @version V1.0 2023-12-03
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/
 
#ifndef __DMA_DEVICE_H__  /* Avoid repeated inclusion */
#define __DMA_DEVICE_H__


//******************************** Includes *********************************//

#include "Protocol_driver_Port.h"
//******************************** Includes *********************************//
//******************************** Typedefs *********************************//
typedef struct DMA_HandleTypeDef DMA_device_HandleTypeDef;
//// I2C配置结构体（抽象版，仅保留核心参数）
//typedef struct {
//    DMA_device_HandleTypeDef* DMA_handle;  // DMA硬件句柄（指针类型，支持前置声明）
//    uint32_t timeout_ms;            // 通信超时时间（ms）
//} DMAConfig;
//// DMA_device.h
//typedef struct {
//    BaseDevice base;
//    DMA_device_HandleTypeDef* DMA_handle;
//    uint32_t timeout_ms;
//} DMADevice;

// SPI设备创建函数
BaseDevice* DMA_device_create(DMA_device_HandleTypeDef* config);

//******************************** Typedefs *********************************//

//******************************** Defines **********************************//

//******************************** Defines **********************************//

//**************************** Interface Structs ****************************//

//**************************** Interface Structs ****************************//

//******************************** Classes **********************************//

//******************************** Classes **********************************//

//******************************** APIs *************************************//

//******************************** APIs *************************************//


#endif /* __BSP_TEMP_HUMI_XXX_HANDLER_H__ */



