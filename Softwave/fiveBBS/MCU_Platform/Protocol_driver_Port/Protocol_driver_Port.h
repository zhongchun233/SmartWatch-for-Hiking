#ifndef __Protocol_driver_Port_H__
#define __Protocol_driver_Port_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "osal_mutex.h"



#include "debug.h" 
// 设备状态枚举
typedef enum {
    DEVICE_STATE_UNINIT = 0,
    DEVICE_STATE_INIT,
    DEVICE_STATE_READY,
    DEVICE_STATE_BUSY,
    DEVICE_STATE_ERROR
} DeviceState;

// 设备类型枚举
typedef enum {
    DEVICE_TYPE_SPI = 0,
    DEVICE_TYPE_I2C,
    DEVICE_TYPE_UART,
    DEVICE_TYPE_MAX,
	DEVICE_TYPE_ERROR
} DeviceType;
typedef enum
{
  SPI_IOCTL_SET_TIMEOUT,
  SPI_IOCTL_GET_TIMEOUT,
  I2C_IOCTL_SET_ADDR,
  I2C_IOCTL_GET_ADDR,
	UART_IOCTL_SET_TIMEOUT,
	UART_IOCTL_GET_TIMEOUT
	
} IOCTL_CMD;

// 前向声明
typedef struct BaseDevice BaseDevice;

// 基类结构体（虚函数表 + 数据成员）
struct BaseDevice {
    // 数据成员
    DeviceType type;
    DeviceState state;
    uint32_t id;
    uint32_t error_code;
		osal_mutex_handle_t Device_mutex;
    
    // 虚函数表（函数指针）
    int (*init)(BaseDevice* self, void* config);
    int (*deinit)(BaseDevice* self);
    int (*read)(BaseDevice* self, uint8_t* buffer, size_t length);
    int (*write)(BaseDevice* self, const uint8_t* data, size_t length);
    int (*ioctl)(BaseDevice* self, IOCTL_CMD cmd, void* arg);
    void (*destroy)(BaseDevice* self);
    
    // 私有数据指针（用于存储派生类特定数据）
    void* private_data;
};

// 公共接口函数
BaseDevice* device_create(DeviceType type, void* config);
void device_destroy(BaseDevice* device);
int16_t device_init(BaseDevice* device, void* config);
int16_t device_read(BaseDevice* device, uint8_t* buffer, size_t length);
int16_t device_write(BaseDevice* device, const uint8_t* data, size_t length);
int16_t device_ioctl(BaseDevice* device, IOCTL_CMD cmd, void* arg);
DeviceState device_get_state(BaseDevice* device);
DeviceType device_get_type_string(BaseDevice* device);

#include "i2c_device.h"
#include "soft_i2c_device.h" // software i2c
#include "i2c_device.h" // software i2c
#include "gpio_device.h"
#include "delay.h"
#include "spi_device.h"
#include "uart_device.h" 

#endif // __OSAL_ERROR_H__
