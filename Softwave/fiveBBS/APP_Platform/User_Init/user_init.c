/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file User_Init.c
 * 
 * @par dependencies 
 * - User_Init.h
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
#include "user_init.h"
#include "osal.h"
//#include "temp_hui_port.h"
//#include "motion_port.h"
//#include "externflash_manage_port.h"
//#include "user_sensor.h"
//#include "user_externflashmanage.h"
//#include "hartrate_port.h"
//#include "User_Display.h"
#include "cmsis_os.h"
#include "event_groups.h"

#include "debug.h"
//#include "i2c_port.h"
//#include "bsp_temp_humi_xxx_handler.h"
#include "user_task_reso_config.h"


//******************************** Includes *********************************//

//******************************** Variable **********************************//
/*线程定义*/
osal_task_handle_t userTaskInitHandle;
/*事件组*/
EventGroupHandle_t userExtFlashEveGropHandle;
/*信号量*/
osal_sema_handle_t userExtFlashSemaHandle;
/*i2c互斥锁*/
osal_mutex_handle_t sensor_i2c_mutex_handler;
/*事件组句柄*/
EventGroupHandle_t xtemphumi_event_flags_handle;

extern st_usertaskcfg_t st_usertaskcfg[USER_TASK_NUM];
//******************************** Variable **********************************//

//******************************** Function definitions ********************************//
/**
 * @brief User initialization task thread
 * @param[in] :None
 * @param[Out] :None
 * @return None
 * 
 * */
static void userTaskInitFunction(void *argument)
{
  /* USER CODE BEGIN userTaskFunction */
  int32_t ret;

  //hartrate_init();

  for(uint8_t i = 0; i < USER_TASK_NUM; i++)
  {
    ret = osal_task_create \
        (st_usertaskcfg[i].task_name, \
         st_usertaskcfg[i].func_pointer, \
         st_usertaskcfg[i].stack_size, \
         st_usertaskcfg[i].priority, \
         st_usertaskcfg[i].task_handle,\
         st_usertaskcfg[i].argument);
    if(OSAL_ERROR == ret)
    {
#ifdef DEBUG_User_Init
      DEBUG_OUT("%s task init failed ",st_usertaskcfg[i].task_name);
#endif
      //TODO:线程创建失败需要关闭对应的相关功能
    }
  }

  {
    //创建事件唤醒
    xtemphumi_event_flags_handle = xEventGroupCreate();
    if (xtemphumi_event_flags_handle == NULL) 
    {
        // 事件组创建失败
        while(1);
    }

    /*创建互斥锁保护I2C执行*/
    ret = osal_mutex_create(&sensor_i2c_mutex_handler);
    if(OSAL_SUCCESS == ret)
    {
//      core_i2c_register_mutex(0,sensor_i2c_mutex_handler);
#ifdef DEBUG_User_Init
      DEBUG_OUT("i2c mutex creat success");
#endif
    }
  }
  //外部flash初始化
  {
    /*创建外部flash事件组*/
    userExtFlashEveGropHandle = xEventGroupCreate();
    ret = osal_sema_binary_create(&userExtFlashSemaHandle);
    if(OSAL_ERROR == ret)
    {
#ifdef DEBUG_User_Init
      DEBUG_OUT("Flash sema binary creat error");
#endif
    }
  }
  osal_task_delete(NULL);
  /* USER CODE END userTaskFunction */
}

/**
 * @brief OS creation initialization thread task
 * @param[in] :None
 * @param[Out] :None
 * @return None
 * 
 * */
void UserAppTask_Init(void)
{
  osal_task_create("userTask", userTaskInitFunction, 128 * 4,
    PRI_EMERGENCY, userTaskInitHandle,NULL);
}

//******************************** Function definitions ********************************//
