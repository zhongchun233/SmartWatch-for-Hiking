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
#include "gpio_device.h"
#include "os_freertos.h"
#include "osal_mutex.h"
#include "cmsis_os2.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//

//******************************** Defines **********************************//

//******************************** Variables ********************************//
// 结构体重定义
void  GPIO_device_Init(GPIO_device_TypeDef  *GPIOx, GPIO_device_InitTypeDef *GPIO_Init)
{
		GPIO_device_Init(GPIOx,GPIO_Init);
}
void  GPIO_device_DeInit(GPIO_device_TypeDef  *GPIOx, uint32_t GPIO_Pin)
{
		GPIO_device_DeInit(GPIOx,GPIO_Pin);
}
GPIO_device_PinState GPIO_device_ReadPin(GPIO_device_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
		return GPIO_device_ReadPin(GPIOx, GPIO_Pin);
}
void GPIO_device_WritePin(GPIO_device_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_device_PinState PinState)
{
	GPIO_device_WritePin( GPIOx,  GPIO_Pin,  PinState);
}
void GPIO_device_TogglePin(GPIO_device_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_device_TogglePin( GPIOx, GPIO_Pin);

}