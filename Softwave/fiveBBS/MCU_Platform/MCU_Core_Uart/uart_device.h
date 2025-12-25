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
 
#ifndef __UART_DEVICE_H__  /* Avoid repeated inclusion */
#define __UART_DEVICE_H__


//******************************** Includes *********************************//

#include "gpio_device.h"
#include "os_freertos.h"
#include "osal_mutex.h"

#include "Protocol_driver_Port.h"
//******************************** Includes *********************************//
//******************************** Typedefs *********************************//
typedef struct UART_HandleTypeDef UART_device_HandleTypeDef;
typedef struct DMA_HandleTypeDef DMA_device_HandleTypeDef;
// I2C配置结构体（抽象版，仅保留核心参数）
typedef struct {
    UART_device_HandleTypeDef* UART_handle;  // UART硬件句柄（指针类型，支持前置声明）
		DMA_device_HandleTypeDef* DMA_handle; // DMA硬件句柄(指针类型)
		bool Enable_DMA;
    uint32_t timeout_ms;            // 通信超时时间（ms）
} UARTConfig;
// uart_device.h
typedef struct {
    BaseDevice base;
    UART_device_HandleTypeDef* UART_handle;
    uint32_t timeout_ms;
} UARTDevice;

// SPI设备创建函数
BaseDevice* uart_device_create(UARTConfig* config);

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



