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
#include "soft_i2c_device.h"   // hardware i2c
#include "i2c.h"   // hardware i2c

#include "osal_mutex.h"
#include "osal_internal_mutex.h"
#include "osal_heap.h"
#include "cmsis_os2.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//

//******************************** Defines **********************************//

//******************************** Variables ********************************//
static int i2c_device_init(BaseDevice* self, void* config) {
    I2CDevice* i2c_dev = (I2CDevice*)self;
    I2CConfig* cfg = (I2CConfig*)config;
    
    if (!i2c_dev || !cfg) {
        return -1;
    }
    
    i2c_dev->i2c_handle = cfg->i2c_handle;
    i2c_dev->device_addr = cfg->device_addr;
    i2c_dev->timeout_ms = cfg->timeout_ms;
    
    self->state = DEVICE_STATE_READY;
    return 0;
}

static int i2c_device_deinit(BaseDevice* self) {
    if (!self) {
        return -1;
    }
    
    self->state = DEVICE_STATE_UNINIT;
    return 0;
}

static int i2c_device_read(BaseDevice* self, uint8_t* buffer, size_t length) {
    I2CDevice* i2c_dev = (I2CDevice*)self;
    HAL_StatusTypeDef status;
    
    if (!i2c_dev || !buffer || length == 0) {
        return -1;
    }
    
    if (self->state != DEVICE_STATE_READY) {
        return -2;
    }
    
    self->state = DEVICE_STATE_BUSY;
    
    // I2C读取
    status = HAL_I2C_Master_Receive((I2C_HandleTypeDef*)i2c_dev->i2c_handle,
                                    i2c_dev->device_addr << 1,
                                    buffer, length,
                                    i2c_dev->timeout_ms);
    
    self->state = DEVICE_STATE_READY;
    
    if (status != HAL_OK) {
        self->state = DEVICE_STATE_ERROR;
        self->error_code = status;
        return -3;
    }
    
    return length;
}
static int i2c_device_write(BaseDevice* self, const uint8_t* data, size_t length) {
    I2CDevice* i2c_dev = (I2CDevice*)self;
    HAL_StatusTypeDef status;
    
    if (!i2c_dev || !data || length == 0) {
        return -1;
    }
    
    if (self->state != DEVICE_STATE_READY) {
        return -2;
    }
    
    self->state = DEVICE_STATE_BUSY;
    
    // I2C写入
    status = HAL_I2C_Master_Transmit((I2C_HandleTypeDef*)i2c_dev->i2c_handle,
                                     i2c_dev->device_addr << 1,
                                     (uint8_t*)data, length,
                                     i2c_dev->timeout_ms);
    
    self->state = DEVICE_STATE_READY;
    
    if (status != HAL_OK) {
        self->state = DEVICE_STATE_ERROR;
        self->error_code = status;
        return -3;
    }
    
    return length;
}
static int i2c_device_ioctl(BaseDevice* self, IOCTL_CMD cmd, void* arg) {
    I2CDevice* i2c_dev = (I2CDevice*)self;
    
    switch (cmd) {
        case I2C_IOCTL_SET_ADDR:
            if (arg) {
                i2c_dev->device_addr = *(uint8_t*)arg;
                return 0;
            }
            break;
        case I2C_IOCTL_GET_ADDR:
            if (arg) {
                *(uint8_t*)arg = i2c_dev->device_addr;
                return 0;
            }
            break;
        default:
					log_e("cmd input ERROR");
            return -1;
    }
    return -1;
}

static void i2c_device_destroy(BaseDevice* self) {
    if (self) {
        i2c_device_deinit(self);
        osal_heap_free(self);
    }
}

BaseDevice* i2c_device_create(I2CConfig* config) {
    I2CDevice* i2c_dev = (I2CDevice*)osal_heap_malloc(sizeof(I2CDevice));
    
    if (!i2c_dev) {
			log_e("I2C_device_create: create I2C_dev failed ");
        return NULL;
    }
    
    memset(i2c_dev, 0, sizeof(I2CDevice));
    i2c_dev->base.type = DEVICE_TYPE_I2C;
    i2c_dev->base.state = DEVICE_STATE_UNINIT;
    
    // 绑定虚函数
    i2c_dev->base.init = i2c_device_init;
    i2c_dev->base.deinit = i2c_device_deinit;
    i2c_dev->base.read = i2c_device_read;
    i2c_dev->base.write = i2c_device_write;
    i2c_dev->base.ioctl = i2c_device_ioctl;
    i2c_dev->base.destroy = i2c_device_destroy;
		int32_t ret = osal_mutex_create(&i2c_dev->base.Device_mutex);
		if (ret != OSAL_SUCCESS) {
        log_e("I2C_device_create: create mutex failed ");
        osal_heap_free(i2c_dev);  // 回滚内存，避免泄漏
        return NULL;
    }
    
    if (config) {
        i2c_dev->i2c_handle = config->i2c_handle;
        i2c_dev->device_addr = config->device_addr;
        i2c_dev->timeout_ms = config->timeout_ms;
    }
    
    return (BaseDevice*)i2c_dev;
}