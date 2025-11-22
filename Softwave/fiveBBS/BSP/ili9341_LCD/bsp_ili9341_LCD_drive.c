/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file ec_bsp_aht21_driver.h
 * 
 * @par dependencies 
 * - ec_bsp_aht21_reg.h
 * - stdio.h
 * - stdint.h
 * 
 * @author chengfeng26
 * 
 * @brief Provide the HAL APIs of AHT21 and corresponding opetions.
 * 
 * Processing flow:
 * 
 * call directly.
 * 
 * @version V1.0 2025-12-03
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/


//******************************** Includes *********************************//

#include "bsp_ili9341_LCD_drive.h"
#include <stdio.h>
#include <stdint.h>
//******************************** Includes *********************************//
//******************************** Defines **********************************//

#define SET_PIN      1
#define RESET_PIN    0

#define HOR_LEN 	40	//	Also mind the resolution of your screen!
#define ST7789_WIDTH 200//240
#define ST7789_HEIGHT 200//280
#define X_SHIFT 0
#define Y_SHIFT 20

#define ABS(x) ((x) > 0 ? (x) : -(x))
//******************************** Defines **********************************//

//******************************** Variables ********************************//
//******************************** Variables ********************************//

//******************************** Functions ********************************//

//static uint8_t st7789_write_command(bsp_st7789_driver_t * const driver_instance, const uint8_t cmd, uint32_t dataLength)
//{
//    driver_instance->p_st7789_basic_operation->pf_write_cs_pin(RESET_PIN);  //chip select
//    driver_instance->p_st7789_basic_operation->pf_write_dc_pin(RESET_PIN);

//    uint8_t dataSend = cmd;
//    driver_instance->p_st7789_basic_operation->pf_spi_transmit(&dataSend, 1);

//    driver_instance->p_st7789_basic_operation->pf_write_cs_pin(SET_PIN);  //chip unselect
//    return 0;
//}