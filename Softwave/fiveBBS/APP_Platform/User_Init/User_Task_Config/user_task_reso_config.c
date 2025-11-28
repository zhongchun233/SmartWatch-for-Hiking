/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file User_Task_Reso_Config.c
 * 
 * @par dependencies
 * - User_Task_Reso_Config.c
 * 
 * @author Simon | R&D Dept. | EternalChip 立芯嵌入式
 * 
 * @brief For application layer task initialization
 * 
 * Processing flow:
 * 
 * call directly.
 * 
 * @version V1.0 2025-2-24
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/
//******************************** Includes *********************************//
#include "user_task_reso_config.h"
#include "osal.h"
//#include "bsp_temp_humi_xxx_handler.h"
//#include "bsp_mpuxxx_handler.h"
//#include "bsp_flash_handler.h"
//******************************** Includes *********************************//

//******************************** Variable **********************************//
osal_task_handle_t userSensorHandle;
osal_task_handle_t userStorgeHandle;
osal_task_handle_t LVGLTaskHandle;
//extern temp_humi_handler_all_input_arg_t input_arg;
//extern mpuxxx_handler_input_args_t mpu6050_input_args;
//extern flash_handler_all_input_arg_t flash_handler_all_input_arg;
//******************************** Variable **********************************//
//******************************** Function **********************************//
void temp_humi_handler_thread(void *argument);
void sensor_polling_task(void *argument);
void mpuxxx_handler_thread(void *argument);
void storage_manager_task(void *argument);
void flash_handler_thread(void *argument);
void display_refresh_task(void *argument);
//******************************** Function **********************************//
st_usertaskcfg_t st_usertaskcfg[USER_TASK_NUM] = 
{
/*    Name                  funct_ptr                       stack size   priority             		task_handle   argument*/
    {"SensorTask",          sensor_polling_task,            1024,        PRI_SOFT_REALTIME,   		  NULL,         NULL},
    {"ExtFlashTask",        storage_manager_task,           1024,        PRI_SOFT_REALTIME + 1,    NULL,         NULL},
    {"LVGLTask",            display_refresh_task,           1024,       PRI_SOFT_REALTIME + 2,    NULL,         NULL},
//    {"tempHandlerTask",     temp_humi_handler_thread,       1024,       PRI_HARD_REALTIME + 1,   	NULL,         &input_arg},
//    {"MpuHandlerTask",      mpuxxx_handler_thread,          1024,       PRI_HARD_REALTIME + 1,    NULL,         &mpu6050_input_args},
//    {"ExtFlashDrv",         flash_handler_thread,           1024,        PRI_HARD_REALTIME + 1,    NULL,         &flash_handler_all_input_arg}
};

__WEAK void display_refresh_task(void *argument)
{

}

  __WEAK void flash_handler_thread(void *argument)
{

}

__WEAK void temp_humi_handler_thread(void *argument)
{

}

__WEAK void sensor_polling_task(void *argument)
{

}

__WEAK void mpuxxx_handler_thread(void *argument)
{

}

__WEAK void storage_manager_task(void *argument)
{

}
