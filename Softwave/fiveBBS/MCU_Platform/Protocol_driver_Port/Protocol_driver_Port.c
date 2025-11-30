#include "Protocol_driver_Port.h"
#include <stdlib.h>
#include "i2c_device.h"  
// 统一的设备创建接口（工厂模式）
BaseDevice* device_create(DeviceType type, void* config) {
    BaseDevice* device = NULL;
    
    switch (type) {
        case DEVICE_TYPE_SPI:
            device = spi_device_create((SPIConfig*)config);
            break;
        case DEVICE_TYPE_I2C:
            device = i2c_device_create((I2CConfig*)config);
            break;
        case DEVICE_TYPE_UART:
						device = uart_device_create((UARTConfig*)config);
            // UART设备实现类似
            break;
        default:
            return NULL;
    }
    
    return device;
}

// 统一的设备销毁接口
void device_destroy(BaseDevice* device) {
    if (device && device->destroy) {
        device->destroy(device);
    }
}

// 统一的初始化接口（多态调用）
int16_t device_init(BaseDevice* device, void* config) {

    return device->init(device, config);
}

// 统一的读取接口（多态调用）
int16_t device_read(BaseDevice* device, uint8_t* buffer, size_t length) {

    return device->read(device, buffer, length);
}

// 统一的写入接口（多态调用）
int16_t device_write(BaseDevice* device, const uint8_t* data, size_t length) {

    return device->write(device, data, length);
}

// 统一的控制接口（多态调用）
int16_t device_ioctl(BaseDevice* device, IOCTL_CMD cmd, void* arg) {

    return device->ioctl(device, cmd, arg);
}

// 获取设备状态
DeviceState device_get_state(BaseDevice* device) {

    return device->state;
}

// 获取设备类型
DeviceType device_get_type_string(BaseDevice* device) {
    if (!device) {
        return DEVICE_TYPE_ERROR;
    }
    
    switch (device->type) {
        case DEVICE_TYPE_SPI:
            return DEVICE_TYPE_SPI;
        case DEVICE_TYPE_I2C:
            return DEVICE_TYPE_I2C;
        case DEVICE_TYPE_UART:
            return DEVICE_TYPE_UART;
        default:
            return DEVICE_TYPE_ERROR;
    }
}

