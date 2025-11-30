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

#include "spi.h"   // hardware i2c


#include "osal_mutex.h"
#include "osal_internal_mutex.h"
#include "osal_heap.h"

//******************************** Includes *********************************//

//******************************** Defines **********************************//

//******************************** Defines **********************************//

//******************************** Variables ********************************//


// SPI设备的方法实现
static int spi_device_init(BaseDevice* self, void* config) {
    SPIDevice* spi_dev = (SPIDevice*)self;
    SPIConfig* cfg = (SPIConfig*)config;
    
    if (!spi_dev || !cfg) {
        return-1;
    }
    
    // 初始化SPI设备特定数据
    spi_dev->spi_handle = cfg->spi_handle;
    spi_dev->cs_port = cfg->cs_port;
    spi_dev->cs_pin = cfg->cs_pin;
    spi_dev->timeout_ms = cfg->timeout_ms;
    
		
    // 初始化CS引脚
		GPIO_device_InitTypeDef SPI_device_CS;
		SPI_device_CS.Mode = GPIO_MODE_OUTPUT_PP;
		SPI_device_CS.Pin = spi_dev->cs_pin;
		SPI_device_CS.Pull = GPIO_NOPULL;
		SPI_device_CS.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_device_Init(spi_dev->cs_port, &SPI_device_CS);
    GPIO_device_WritePin(spi_dev->cs_port, spi_dev->cs_pin, GPIO_device_PIN_SET);
    
    self->state = DEVICE_STATE_READY;
    return 0;
}

static int spi_device_deinit(BaseDevice* self) {
    SPIDevice* spi_dev = (SPIDevice*)self;
    
    if (!spi_dev) {
        return-1;
    }
    
    // 释放CS引脚
    GPIO_device_WritePin(spi_dev->cs_port, spi_dev->cs_pin, GPIO_device_PIN_SET);
    
    self->state = DEVICE_STATE_UNINIT;
    return 0;
}

static int spi_device_read(BaseDevice* self, uint8_t* buffer, size_t length) {
    SPIDevice* spi_dev = (SPIDevice*)self;
    HAL_StatusTypeDef status;
    
    if (!spi_dev || !buffer || length == 0) {
        return-1;
    }
    
    if (self->state != DEVICE_STATE_READY) {
        return-2;
    }
    
    self->state = DEVICE_STATE_BUSY;
    
    // 片选拉低
    GPIO_device_WritePin(spi_dev->cs_port, spi_dev->cs_pin, GPIO_device_PIN_RESET);
    
    // SPI读取
    status = HAL_SPI_Receive((SPI_HandleTypeDef*)spi_dev->spi_handle, buffer, length, 
                             spi_dev->timeout_ms);
    
    // 片选拉高
    GPIO_device_WritePin(spi_dev->cs_port, spi_dev->cs_pin, GPIO_device_PIN_SET);
    
    self->state = DEVICE_STATE_READY;
    
    if (status != HAL_OK) {
        self->state = DEVICE_STATE_ERROR;
        self->error_code = status;
        return-3;
    }
    
    return length;
}

static int spi_device_write(BaseDevice* self, const uint8_t* data, size_t length) {
    SPIDevice* spi_dev = (SPIDevice*)self;
    HAL_StatusTypeDef status;
    
    if (!spi_dev || !data || length == 0) {
        return-1;
    }
    
    if (self->state != DEVICE_STATE_READY) {
        return-2;
    }
    
    self->state = DEVICE_STATE_BUSY;
    
    // 片选拉低
    GPIO_device_WritePin(spi_dev->cs_port, spi_dev->cs_pin, GPIO_device_PIN_RESET);
    
    // SPI写入
    status = HAL_SPI_Transmit((SPI_HandleTypeDef*)spi_dev->spi_handle, (uint8_t*)data, length, 
                              spi_dev->timeout_ms);
    
    // 片选拉高
    GPIO_device_WritePin(spi_dev->cs_port, spi_dev->cs_pin, GPIO_device_PIN_SET);
    
    self->state = DEVICE_STATE_READY;
    
    if (status != HAL_OK) {
        self->state = DEVICE_STATE_ERROR;
        self->error_code = status;
        return-3;
    }
    
    return length;
}

static int spi_device_ioctl(BaseDevice* self, IOCTL_CMD cmd, void* arg) {
    SPIDevice* spi_dev = (SPIDevice*)self;
    
    switch (cmd) {
        case SPI_IOCTL_SET_TIMEOUT:
            if (arg) {
                spi_dev->timeout_ms = *(uint32_t*)arg;
                return 0;
            }
            break;
        case SPI_IOCTL_GET_TIMEOUT:
            if (arg) {
                *(uint32_t*)arg = spi_dev->timeout_ms;
                return 0;
            }
            break;
        default:
            return-1;
    }
    return-1;
}

static void spi_device_destroy(BaseDevice* self) {
    if (self) {
        spi_device_deinit(self);
        osal_heap_free(self);
    }
}

// SPI设备创建函数（构造函数）
BaseDevice* spi_device_create(SPIConfig* config) {
	
SPIDevice* spi_dev = (SPIDevice*)osal_heap_malloc(sizeof(SPIDevice));
    
    if (!spi_dev) {
				log_e("spi_device_create: create spi_dev failed ");
        return NULL;
    }
    
    // 初始化基类成员
    memset(spi_dev, 0, sizeof(SPIDevice));
    spi_dev->base.type = DEVICE_TYPE_SPI;
    spi_dev->base.state = DEVICE_STATE_UNINIT;
    spi_dev->base.id = 0;
    spi_dev->base.error_code = 0;
    
    // 绑定虚函数（方法重写）
    spi_dev->base.init = spi_device_init;
    spi_dev->base.deinit = spi_device_deinit;
    spi_dev->base.read = spi_device_read;
    spi_dev->base.write = spi_device_write;
    spi_dev->base.ioctl = spi_device_ioctl;
    spi_dev->base.destroy = spi_device_destroy;
    int32_t ret = osal_mutex_create(&spi_dev->base.Device_mutex);
		if (ret != OSAL_SUCCESS) {
        log_e("spi_device_create: create mutex failed ");
        osal_heap_free(spi_dev);  // 回滚内存，避免泄漏
        return NULL;
    }
    // 初始化SPI特定数据
    if (config) {
        spi_dev->spi_handle = config->spi_handle;
        spi_dev->cs_port = config->cs_port;
        spi_dev->cs_pin = config->cs_pin;
        spi_dev->timeout_ms = config->timeout_ms;
    }
    
    return (BaseDevice*)spi_dev;
}



