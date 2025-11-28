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
 
#ifndef __I2C_PORT_H__  /* Avoid repeated inclusion */
#define __I2C_PORT_H__

//******************************** Includes *********************************//
#include "main.h"
#include "os_freertos.h"
#include "osal_mutex.h"
//******************************** Includes *********************************//
//******************************** Typedefs *********************************//
typedef enum
{
    CORE_I2C_OK = 0,
    CORE_I2C_ERROR
}en_core_gpio_status_t;

typedef enum
{
    CORE_I2C_BUS_1 = 0,
    CORE_I2C_BUS_2,
    CORE_I2C_BUS_MAX
}en_core_i2c_bus_t;

typedef enum
{
    CORE_SPI_OK = 0,
    CORE_SPI_ERROR
}en_core_spi_status_t;

typedef enum
{
    CORE_SPI_BUS_1 = 0,         //Flash
    CORE_SPI_BUS_2,             //LCD
    CORE_SPI_BUS_MAX
}en_core_spi_bus_t;

typedef struct
{
    SPI_HandleTypeDef* st_SPI_HandleTypeDef;
    GPIO_TypeDef* cs_port;
    uint16_t cs_pin;
    uint32_t timeout;
    uint8_t spi_mode;
    osal_mutex_handle_t st_osMutexId;
} st_spi_port_t;

//******************************** Typedefs *********************************//

//******************************** Defines **********************************//
#define SENSOR_SPI1_HARDWARE_TRANSMIT(data,size,timeout)\
        core_spi_transmit(CORE_SPI_BUS_1,data,size,timeout)

#define SENSOR_SPI1_HARDWARE_RECEIVE(data,size,timeout)\
        core_spi_receive(CORE_SPI_BUS_1,data,size,timeout)

#define SENSOR_SPI1_HARDWARE_TRANSMIT_DMA(data,size)\
        core_spi_transmit_dma(CORE_SPI_BUS_1,data,size)

#define SENSOR_SPI1_HARDWARE_RECEIVE_WITH_CS(data,size,timeout)\
        core_spi_write_with_cs(CORE_SPI_BUS_1,data,size,timeout)

#define SENSOR_SPI1_HARDWARE_WRITE_CS(pinState)\
        core_spi_write_cs(CORE_SPI_BUS_1,pinState)

#define SENSOR_SPI2_HARDWARE_TRANSMIT(data,size,timeout)\
        core_spi_transmit(CORE_SPI_BUS_2,data,size,timeout)

#define SENSOR_SPI2_HARDWARE_RECEIVE(data,size,timeout)\
        core_spi_receive(CORE_SPI_BUS_2,data,size,timeout)

#define SENSOR_SPI2_HARDWARE_TRANSMIT_DMA(data,size)\
        core_spi_transmit_dma(CORE_SPI_BUS_2,data,size)

#define SENSOR_SPI2_HARDWARE_RECEIVE_WITH_CS(data,size,timeout)\
        core_spi_write_with_cs(CORE_SPI_BUS_2,data,size,timeout)

#define SENSOR_SPI2_HARDWARE_WRITE_CS(pinState)\
        core_spi_write_cs(CORE_SPI_BUS_2,pinState)
//******************************** Defines **********************************//

//**************************** Interface Structs ****************************//

//**************************** Interface Structs ****************************//

//******************************** Classes **********************************//

//******************************** Classes **********************************//

//******************************** APIs *************************************//
void core_i2c_register_mutex(en_core_i2c_bus_t bus, osal_mutex_handle_t mutex);
//******************************** APIs *************************************//


#endif /* __BSP_TEMP_HUMI_XXX_HANDLER_H__ */



