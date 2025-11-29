/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */

#ifndef _bsp_adaper_port_button_H_
#define _bsp_adaper_port_button_H_
//******************************** Includes *********************************//
#include "bsp_multi_button.h"
#include "debug.h"
#include "gpio.h"
#include "cmsis_os.h"
//******************************** Includes *********************************//
//******************************** Defines **********************************//
#define BUTTON1_GPIO_Port GPIOA
#define BUTTON1_Pin GPIO_PIN_0

#define BUTTON2_GPIO_Port GPIOA
#define BUTTON2_Pin GPIO_PIN_1
//******************************** Defines **********************************//


//******************************** Typedefs *********************************//

//******************************** Typedefs *********************************//


//**************************** Interface Structs ****************************//

//**************************** Interface Structs ****************************//

//******************************** Classes **********************************//

//******************************** Classes **********************************//


//******************************** APIs *************************************//
void drv_adapter_button_register(void);
//******************************** APIs *************************************//


#endif /* __BSP_TEMP_HUMI_XXX_HANDLER_H__ */
