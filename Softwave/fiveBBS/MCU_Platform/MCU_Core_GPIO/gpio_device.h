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

//******************************** Typedefs *********************************//

//******************************** Defines **********************************//

//******************************** Defines **********************************//

//**************************** Interface Structs ****************************//

//**************************** Interface Structs ****************************//

//******************************** Classes **********************************//

//******************************** Classes **********************************//

//******************************** APIs *************************************//
int spi_device_init(BaseDevice* self, void* config
void  HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void  HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
//******************************** APIs *************************************//


#endif /* __BSP_TEMP_HUMI_XXX_HANDLER_H__ */



