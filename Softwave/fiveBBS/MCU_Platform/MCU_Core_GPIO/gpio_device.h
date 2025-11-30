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
 
#ifndef __GPIO_device_H__  /* Avoid repeated inclusion */
#define __GPIO_device_H__

//******************************** Includes *********************************//
#include "os_freertos.h"
//******************************** Includes *********************************//
//******************************** Typedefs *********************************//
typedef enum
{
  GPIO_device_PIN_RESET = 0u,
  GPIO_device_PIN_SET
} GPIO_device_PinState;

typedef struct
{
  uint32_t Pin;       /*!< Specifies the GPIO pins to be configured.
                           This parameter can be any value of @ref GPIO_pins_define */

  uint32_t Mode;      /*!< Specifies the operating mode for the selected pins.
                           This parameter can be a value of @ref GPIO_mode_define */

  uint32_t Pull;      /*!< Specifies the Pull-up or Pull-Down activation for the selected pins.
                           This parameter can be a value of @ref GPIO_pull_define */

  uint32_t Speed;     /*!< Specifies the speed for the selected pins.
                           This parameter can be a value of @ref GPIO_speed_define */
} GPIO_device_InitTypeDef;
typedef struct GPIO_TypeDef GPIO_device_TypeDef;
//******************************** Typedefs *********************************//

//******************************** Defines **********************************//
#define GPIO_device_PIN_0                 ((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_device_PIN_1                 ((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_device_PIN_2                 ((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_device_PIN_3                 ((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_device_PIN_4                 ((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_device_PIN_5                 ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_device_PIN_6                 ((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_device_PIN_7                 ((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_device_PIN_8                 ((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_device_PIN_9                 ((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_device_PIN_10                ((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_device_PIN_11                ((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_device_PIN_12                ((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_device_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_device_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_device_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_device_PIN_All               ((uint16_t)0xFFFF)  /* All pins selected */
//******************************** Defines **********************************//

//**************************** Interface Structs ****************************//

//**************************** Interface Structs ****************************//

//******************************** Classes **********************************//

//******************************** Classes **********************************//

//******************************** APIs *************************************//
void  GPIO_device_Init(GPIO_device_TypeDef  *GPIOx, GPIO_device_InitTypeDef *GPIO_Init);
void  GPIO_device_DeInit(GPIO_device_TypeDef  *GPIOx, uint32_t GPIO_Pin);
GPIO_device_PinState GPIO_device_ReadPin(GPIO_device_TypeDef *GPIOx, uint16_t GPIO_Pin);
void GPIO_device_WritePin(GPIO_device_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_device_PinState PinState);
void GPIO_device_TogglePin(GPIO_device_TypeDef *GPIOx, uint16_t GPIO_Pin);

//******************************** APIs *************************************//


#endif /* __BSP_TEMP_HUMI_XXX_HANDLER_H__ */



