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
 
#ifndef __i2c_device_H__  /* Avoid repeated inclusion */
#define __i2c_device_H__

//typedef struct GPIO_TypeDef GPIO_device_TypeDef;
//******************************** Includes *********************************//
#include "Protocol_driver_Port.h"

#include "os_freertos.h"
#include "osal_mutex.h"
//******************************** Includes *********************************//
//******************************** Typedefs *********************************//
typedef struct I2C_HandleTypeDef I2C_device_HandleTypeDef;


// I2C配置结构体（抽象版，仅保留核心参数）
typedef struct {
    I2C_device_HandleTypeDef* i2c_handle;  // I2C硬件句柄（指针类型，支持前置声明）
    uint8_t  device_addr;           // 设备I2C地址（7/10位）
    uint32_t timeout_ms;            // 通信超时时间（ms）
} I2CConfig;

// I2C设备结构体（继承BaseDevice，解耦HAL库）
typedef struct {
    BaseDevice base;                // 基类（必须放第一个成员，保证类型转换兼容）
    I2C_device_HandleTypeDef* i2c_handle;  // I2C硬件句柄
    uint8_t  device_addr;           // 设备地址
    uint32_t timeout_ms;            // 超时时间
} I2CDevice;

BaseDevice* i2c_device_create(I2CConfig* config);
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



