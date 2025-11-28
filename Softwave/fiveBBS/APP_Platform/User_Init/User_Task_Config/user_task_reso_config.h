/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file User_Task_Reso_Config.h
 * 
 * @par dependencies 
 * - User_Task_Reso_Config.h
 * 
 * @author Simon | R&D Dept. | EternalChip 立芯嵌入式
 * 
 * @brief Provide all the debugging tools in this project.
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
#ifndef __USER_TASK_RESO_CONFIG_H__  //Avoid repeated including same files later
#define __USER_TASK_RESO_CONFIG_H__

//******************************** Includes *********************************//
#include "FreeRTOSConfig.h"
#include"osal.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//

#define USER_TASK_NUM          6
//Task  Priority Classification
#define PRI_EMERGENCY     (configMAX_PRIORITIES-1)  // 紧急事件（最高）（保留2个优先级）
#define PRI_HARD_REALTIME (PRI_EMERGENCY-4)         // 硬件实时任务（传感器采样）（4个优先级跨度）
#define PRI_SOFT_REALTIME (PRI_HARD_REALTIME-5)     // 软件实时任务（协议解析）（5个优先级跨度）
#define PRI_NORMAL        (PRI_SOFT_REALTIME-7)     // 常规业务任务  
#define PRI_BACKGROUND    (1)                       // 后台任务（日志上传）

//******************************** Defines **********************************//
//******************************** Typedef **********************************//
typedef struct 
{
    const char *        task_name;
    osal_task_entry     func_pointer; 
    size_t              stack_size;
    osal_priority_t     priority; 
    osal_task_handle_t  task_handle;
    void *              argument;
}st_usertaskcfg_t;

//******************************** Typedef **********************************//

//******************************** Declaring ********************************//

//******************************** Declaring ********************************//

#endif /* __USER_TASK_RESO_CONFIG_H__ */
