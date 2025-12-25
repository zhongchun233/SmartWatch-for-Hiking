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

#include "usart.h"   // hardware i2c
#include "uart_device.h" 

#include "osal_mutex.h"
#include "osal_internal_mutex.h"
#include "osal_heap.h"

//******************************** Includes *********************************//

//******************************** Defines **********************************//

//******************************** Defines **********************************//

//******************************** Variables ********************************//


// UART设备的方法实现
static int UART_device_init(BaseDevice* self, void* config) {
    UARTDevice* UART_dev = (UARTDevice*)self;
    UARTConfig* cfg = (UARTConfig*)config;
    
    if (!UART_dev || !cfg) {
			log_e("UART_device_init: !UART_dev || !cfg ");
        return-1;
    }
    
    // 初始化UART设备特定数据
    UART_dev->UART_handle = cfg->UART_handle;
    UART_dev->timeout_ms = cfg->timeout_ms;
		
		HAL_UART_MspInit((UART_HandleTypeDef *)UART_dev->UART_handle);
    
    self->state = DEVICE_STATE_READY;
    return 0;
}

static int UART_device_deinit(BaseDevice* self) {
    UARTDevice* UART_dev = (UARTDevice*)self;
    
    if (!UART_dev) {
			log_e("UART_device_deinit: UART_dev NULL ");
        return -1;
    }
    
		HAL_UART_MspDeInit((UART_HandleTypeDef *)UART_dev->UART_handle);
    
    self->state = DEVICE_STATE_UNINIT;
    return 0;
}

static int UART_device_read(BaseDevice* self, uint8_t* buffer, size_t length) {
    UARTDevice* UART_dev = (UARTDevice*)self;
    HAL_StatusTypeDef status;
    
    if (!UART_dev || !buffer || length == 0) {
			log_e("UART_device_read: !UART_dev || !buffer || length == 0 ");
        return -1;
    }
    
    if (self->state != DEVICE_STATE_READY) {
			log_e("UART_device_read: not DEVICE_STATE_READY");
        return -2;
    }
    
    self->state = DEVICE_STATE_BUSY;
    
	status = HAL_UART_Receive((UART_HandleTypeDef *)UART_dev->UART_handle,
																									buffer,
																									length,
																									UART_dev->timeout_ms);
									
    self->state = DEVICE_STATE_READY;
    
    if (status != HAL_OK) {
        self->state = DEVICE_STATE_ERROR;
        self->error_code = status;
			log_e("uart_device_read: UART receive failed "); 
        return -3;
    }
    
    return length;
}

static int UART_device_write(BaseDevice* self, const uint8_t* data, size_t length) {
    UARTDevice* UART_dev = (UARTDevice*)self;
    HAL_StatusTypeDef status;
    
    if (!UART_dev || !data || length == 0) {
			log_e("UART_device_write: !UART_dev || !data || length == 0 ");
        return -1;
    }
    
    if (self->state != DEVICE_STATE_READY) {
			log_e("UART_device_write: not DEVICE_STATE_READY ");
        return -2;
    }
    
    self->state = DEVICE_STATE_BUSY;
    
		status = HAL_UART_Transmit((UART_HandleTypeDef *)UART_dev->UART_handle,
																										data,
																										length,
																										UART_dev->timeout_ms);
    
    self->state = DEVICE_STATE_READY;
    
    if (status != HAL_OK) {
        self->state = DEVICE_STATE_ERROR;
        self->error_code = status;
			log_e("UART_device_write: status != HAL_OK ");
        return-3;
    }
    
    return length;
}

static int UART_device_ioctl(BaseDevice* self, IOCTL_CMD cmd, void* arg) {
    UARTDevice* UART_dev = (UARTDevice*)self;
    
    switch (cmd) {
        case UART_IOCTL_SET_TIMEOUT:
            if (arg) {
                UART_dev->timeout_ms = *(uint32_t*)arg;
                return 0;
            }
            break;
        case UART_IOCTL_GET_TIMEOUT:
            if (arg) {
                *(uint32_t*)arg = UART_dev->timeout_ms;
                return 0;
            }
            break;
        default:
					log_e("UART_device_ioctl: CMD error ");
            return-1;
    }
    return-1;
}

static void UART_device_destroy(BaseDevice* self) {
    if (self) {
        UART_device_deinit(self);
        osal_heap_free(self);
    }
}

// UART设备创建函数（构造函数）
BaseDevice* uart_device_create(UARTConfig* config) {
	
UARTDevice* UART_dev = (UARTDevice*)osal_heap_malloc(sizeof(UARTDevice));
    
    if (!UART_dev) {
				log_e("UART_device_create: create UART_dev failed ");
        return NULL;
    }
    
    // 初始化基类成员
    memset(UART_dev, 0, sizeof(UARTDevice));
    UART_dev->base.type = DEVICE_TYPE_UART;
    UART_dev->base.state = DEVICE_STATE_UNINIT;

    
    // 绑定虚函数（方法重写）
    UART_dev->base.init = UART_device_init;
    UART_dev->base.deinit = UART_device_deinit;
    UART_dev->base.read = UART_device_read;
    UART_dev->base.write = UART_device_write;
    UART_dev->base.ioctl = UART_device_ioctl;
    UART_dev->base.destroy = UART_device_destroy;
    int32_t ret = osal_mutex_create(&UART_dev->base.Device_mutex);
		UART_device_init((BaseDevice*)UART_dev,config);
		if (ret != OSAL_SUCCESS) {
        log_e("UART_device_create: create mutex failed ");
        osal_heap_free(UART_dev);  // 回滚内存，避免泄漏
        return NULL;
    }
    // 初始化UART特定数据
    if (config) {
        UART_dev->UART_handle = config->UART_handle;
        UART_dev->timeout_ms = config->timeout_ms;
    }
    
    return (BaseDevice*)UART_dev;
}

