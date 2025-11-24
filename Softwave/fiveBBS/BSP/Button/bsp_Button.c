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
#include "bsp_Button.h"
#include "debug.h"
//******************************** Defines **********************************//
#define FALING_TYPE 0
#define RASING_TYPE 1
QueueHandle_t              key_queue = NULL;
QueueHandle_t        inter_key_queue = NULL;
osThreadId_t key_TaskHandle;

const osThreadAttr_t key_Task_attributes = {
  .name = "key_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
//******************************** Defines **********************************//
/**
 * @brief Instantiates the bsp_led_handler_t target.
 * 
 * Steps:
 *  1.doing the specific operations
 *  
 * @param[in] void 
 * 
 * @return KEY_OK            : the key has been pressed
 *         KEY_ERRORTIMEOUT  : the key has been released
 * */
key_status_t key_scan(key_press_status_t * key_value)
{
    key_press_status_t key_status_value = KEY_NOT_PRESSED;
    
    if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
    {
        key_status_value = KEY_PRESSED;
        *key_value = key_status_value;
        return KEY_OK;
    }

    *key_value = key_status_value;
    
    return KEY_ERRORTIMEOUT;//始终没有按键被按下，返回超时
}
/**
  * @brief  FreeRTOS按键处理任务（独立任务，循环运行）
  * @param  argument: 任务入口参数（未使用，FreeRTOS任务标准参数）
  * @note   核心设计：结合「中断队列+状态机」实现按键短按/长按检测，无阻塞、抗误触
  *         1. 先创建2个队列：key_queue（最终按键结果队列）、inter_key_queue（中断事件过渡队列）
  *         2. 从inter_key_queue接收中断传递的按键边沿事件（下降沿=按下，上升沿=松开）
  *         3. 通过状态机（event_index）管理事件流程：下降沿→上升沿的完整按键周期
  *         4. 加入10ms抗误触判断、短按/长按时间阈值区分，输出结果到key_queue供上层使用
  */
void key_task_func(void *argument)
{
    /** 栈上局部变量（任务栈分配，无需手动释放） */
    uint32_t           event_index = 0;                /* 状态机索引：0=初始态，1=已捕获下降沿 */
    uint32_t           frist_trigger_tick = 0;         /* 首次触发时间戳（记录按键按下时刻） */
    uint32_t           short_press_time = SHORT_PRESS_TIME; /* 短按判定阈值（宏定义，单位ms） */
    key_press_event_t  key_press_event =                /* 按键事件结构体（存储中断传递的边沿信息） */
    { 
     .edge_type = RASING,       /* 初始化为上升沿（无实际意义，仅初始化） */
     .trigger_tick = 0          /* 初始时间戳0 */
    };
    /** 栈上局部变量结束 */

    /** 堆上OS对象（FreeRTOS队列，需通过xQueueCreate创建，占用堆内存） */
    // key_queue：存储最终按键结果（短按/长按），队列长度10，每个元素为key_press_status_t类型
    key_queue = xQueueCreate(10, sizeof(key_press_status_t));
    // inter_key_queue：存储中断传递的原始按键事件（边沿类型+时间戳），队列长度10，元素为key_press_event_t类型
    inter_key_queue = xQueueCreate(10, sizeof(key_press_event_t)); // 过渡队列：中断→任务的事件缓冲
    /** 堆上OS对象结束 */

    // 第一步：检查队列创建是否成功（队列创建失败会导致后续功能异常）
    if (NULL == key_queue || NULL == inter_key_queue)
    {
        log_d("按键队列创建失败！\r\n");
        return; // 队列创建失败，任务直接退出
    } 
    else
    {
        log_d("按键队列创建成功！\r\n");
    }

    // 任务主循环（FreeRTOS任务必须包含无限循环，不可返回）
    for(;;)
    {
        // 调试打印：任务运行状态（可根据需求关闭，减少串口占用）
			log_d("KEY is running:[%d] ms \r\n", HAL_GetTick());

        // 1. 从中断事件队列（inter_key_queue）接收数据（非阻塞：超时时间0ms，无数据立即返回）
        // 参数1：目标队列句柄（inter_key_queue，中断回调函数往此队列发事件）
        // 参数2：接收数据缓冲区（存储读取到的按键事件）
        // 参数3：超时时间（0ms=非阻塞，有数据则读，无数据则跳过）
        if (pdTRUE == xQueueReceive(inter_key_queue,  
                                   &(key_press_event), 
                                   (TickType_t)0))
        {
            // 调试打印：接收到的事件时间戳和当前系统时间
            log_d("接收到按键事件，事件触发时间：[%d] ms \r\n", key_press_event.trigger_tick);
            log_d("任务接收按键事件，当前系统时间：[%d] ms \r\n", HAL_GetTick());

            // 1.1 状态机处理：根据事件边沿类型和当前状态（event_index）处理事件
            // 状态机逻辑：仅允许「初始态(0)→下降沿→状态1→上升沿→初始态(0)」的合法流程

            // 异常情况1：初始态（event_index=0）下接收到上升沿事件（无先按下直接松开，非法）
            if (RASING == key_press_event.edge_type && 0 == event_index)
            {    
                log_d("异常：未检测到按键按下，直接收到松开事件！\r\n");
            }

            // 合法流程1：初始态（event_index=0）下接收到下降沿事件（按键按下，进入状态1）
            if (FAILING == key_press_event.edge_type && 0 == event_index)
            {
                log_d("检测到按键按下（下降沿），状态机进入[1]态\r\n");
                event_index += 1; // 状态机索引更新为1（标记已捕获按下事件）
                frist_trigger_tick = key_press_event.trigger_tick; // 记录按下时刻的时间戳
            }

            // 合法流程2：状态1（event_index=1）下接收到上升沿事件（按键松开，完成一次按键周期）
            if (RASING == key_press_event.edge_type && 1 == event_index)
            {
                log_d("检测到按键松开（上升沿），开始判定短按/长按\r\n");

                // 1.1.1 抗误触判断：按下→松开的时间间隔<10ms，判定为误触（抖动），忽略该事件
                if (key_press_event.trigger_tick - frist_trigger_tick < 10)
                {
                    log_d("误触：按键按下松开间隔<10ms，事件无效！当前时间：[%d] ms \r\n", HAL_GetTick());
                    event_index = 0; // 状态机重置为初始态，等待下一次合法事件
                    continue; // 跳过后续处理，直接进入下一次循环
                }

                // 1.1.2 事件有效：按下→松开间隔≥10ms，开始区分短按/长按
                // 计算按键按下时长 = 松开时间戳 - 按下时间戳
                uint32_t press_duration = key_press_event.trigger_tick - frist_trigger_tick;

                // 1.1.2.1 短按判定：按下时长 < 短按阈值（short_press_time）
                if (press_duration < short_press_time)
                {
                    key_press_status_t key_result = KEY_SHORT_PRESSED; // 短按结果

                    // 将短按结果发送到key_queue（上层任务从该队列获取结果）
                    // 非阻塞发送（超时0ms）：队列满则发送失败
                    if (pdTRUE == xQueueSendToFront(key_queue, &key_result, 0))
                    {
                        log_d("短按判定成功！结果已发送到key_queue，当前时间：[%d] ms \r\n", HAL_GetTick());
                        event_index = 0; // 状态机重置为初始态，准备下一次按键检测
                    }
                    else
                    {
                        log_d("短按结果发送失败！key_queue队列满！当前时间：[%d] ms \r\n", HAL_GetTick());
                    }
                }

                // 1.1.2.2 长按判定：按下时长 > 短按阈值（short_press_time）
                // 注：此处用>而非≥，避免与短按阈值边界冲突（若需包含等于，可改为≥）
                if (press_duration > short_press_time)
                {
                    key_press_status_t key_result = KEY_LONG_PRESSED; // 长按结果

                    // 将长按结果发送到key_queue
                    if (pdTRUE == xQueueSendToFront(key_queue, &key_result, 0))
                    {
                        log_d("长按判定成功！结果已发送到key_queue，当前时间：[%d] ms \r\n", HAL_GetTick());
                        event_index = 0; // 状态机重置为初始态
                    }
                    else
                    {
                        log_d("长按结果发送失败！key_queue队列满！当前时间：[%d] ms \r\n", HAL_GetTick());
                    }
                }

                // 注：若press_duration == short_press_time，此处无处理→默认忽略该情况
                // 可根据需求补充：添加等于条件，判定为短按或长按
            }
        }

        // 任务延时100ms（降低CPU占用率，FreeRTOS任务必须包含延时/阻塞操作，否则会独占CPU）
        osDelay(100);
    }
    /* USER CODE END 5 */
}
/**
  * @brief  按键短按/长按检测函数
  * @param  key_value: 输出参数（指针），存储检测到的按键状态（KEY_SHORT_PRESSED/KEY_LONG_PRESSED）
  * @param  short_press_time: 短按判定阈值（单位：ms），超过该时间未松开则判定为长按
  * @retval key_status_t: 函数执行状态（KEY_OK表示检测有效，其他值表示检测异常）
  * @note   核心逻辑：
  *         1. 先检测按键是否按下（通过key_scan底层扫描函数）
  *         2. 按下后计时，若在short_press_time内松开 → 短按
  *         3. 若超过short_press_time仍未松开 → 长按，直到按键松开才返回
  *         4. 依赖底层key_scan函数获取实时按键状态（需确保key_scan返回准确）
  */
key_status_t key_scan_short_long_press(key_press_status_t *key_value, 
                                       uint32_t short_press_time)
{
    /** 局部变量定义 */
    key_status_t ret_key_status = KEY_OK;        /* 函数返回状态：默认初始化成功 */
    key_press_status_t key_value_temp = KEY_NOT_PRESSED; /* 临时变量：存储key_scan的实时状态 */
    uint32_t counter_tick = 0;                   /* 计时变量：记录按键首次按下的时间戳 */
    /** 局部变量定义结束 */

    // 1. 第一步：调用底层按键扫描函数，检测按键当前是否被按下
    ret_key_status = key_scan(&key_value_temp);
    // TBD : critical section → 待优化：此处需添加临界区保护（如FreeRTOS的vTaskSuspendAll）
    // 原因：若在多任务环境下，其他任务可能操作按键相关资源，导致状态读取异常

    // 1.1 若扫描函数执行正常（KEY_OK），且检测到按键已按下（KEY_PRESSED）
    if (KEY_OK == ret_key_status)
    {
        if (KEY_PRESSED == key_value_temp)
        {
            // 记录按键首次按下的时间戳（HAL_GetTick()返回系统运行毫秒数，溢出周期约50天）
            counter_tick = HAL_GetTick(); 

            // 1.1.1 阻塞等待：直到达到短按判定时间（short_press_time）
            // 期间不做任何处理，仅等待时间流逝
            while (HAL_GetTick() < counter_tick + short_press_time)
                ;
            
            // 1.1.2 时间到后，再次扫描按键状态：判断用户是否已松开
            ret_key_status = key_scan(&key_value_temp);
            
            // 若此时按键状态为"未按下" → 说明用户在短按时间内松开，判定为短按
            if (KEY_NOT_PRESSED == key_value_temp)
            {
                // 1.1.3 给输出参数赋值：短按状态
                *key_value = KEY_SHORT_PRESSED;
                return KEY_OK; // 检测完成，返回成功
            } 
            else // 若此时按键仍为"按下"状态 → 说明按压时间超过短按阈值，判定为长按
            {
                // 1.2 给输出参数赋值：长按状态
                *key_value = KEY_LONG_PRESSED;
                
                // 1.2.1 阻塞等待：直到用户松开按键（避免长按后立即触发短按误判）
                // 循环扫描按键状态，直到key_scan返回非KEY_OK（或状态变为未按下）
                while (KEY_OK == key_scan(&key_value_temp)) // 只要按键仍按下，就持续循环
                    ;
                
                return KEY_OK; // 按键松开后，返回成功
            }
        }
    }

    // 若上述逻辑未触发（如按键未按下、扫描函数异常），返回原始执行状态
    return ret_key_status;
}

/**
  * @brief  按键中断回调函数（GPIO中断触发时自动执行）
  * @note   核心逻辑：采用「双边沿触发切换」机制，实现按键按下（下降沿）和松开（上升沿）事件捕获
  *         1. 首次触发为下降沿（按键按下），发送按下事件到队列，切换中断为上升沿
  *         2. 再次触发为上升沿（按键松开），发送松开事件到队列，切换中断回下降沿
  *         3. 基于FreeRTOS队列实现中断与应用层解耦，避免中断中处理复杂逻辑
  */
KEY_CALLBACK
{
    // 静态变量：记录当前中断触发类型（仅初始化一次，断电前保持状态）
    // FALING_TYPE：下降沿触发（对应按键按下）；RASING_TYPE：上升沿触发（对应按键松开）
    static uint32_t irq_type = FALING_TYPE;
    
    /* 局部变量：FreeRTOS中断安全API专用，标记是否唤醒了更高优先级任务
     * 必须初始化（默认0），用于xQueueSendToFrontFromISR的第三个参数
     */
    BaseType_t xHigherPrioritTaskWoken = pdFALSE;
    
    // ========== 分支1：当前为下降沿触发（按键按下事件） ==========
    if (FALING_TYPE == irq_type)
    {
        // 定义按键按下事件结构体，存储事件类型和触发时间戳
        key_press_event_t key_press_event_1 = 
        {
            .edge_type    = FAILING,          // 事件类型：下降沿（按键按下）
            .trigger_tick = HAL_GetTick()     // 记录事件触发时间（系统运行毫秒数）
        };
        
        // 容错判断：检查队列是否已创建（避免空指针访问崩溃）
        if (NULL == inter_key_queue)
        {
            log_d("按键事件队列未创建！触发时间：[%d] ms \r\n", HAL_GetTick());
        }        
        else
        {
            /* 中断安全地向队列头部发送按键事件（FromISR后缀表示中断上下文专用）
             * 参数1：目标队列句柄（inter_key_queue需提前通过xQueueCreate创建）
             * 参数2：发送的数据（按键事件结构体）
             * 参数3：是否唤醒更高优先级任务的标记
             * 返回值：pdTRUE表示发送成功，pdFALSE表示队列满（事件丢失）
             */
            if (pdTRUE == xQueueSendToFrontFromISR(inter_key_queue, 
                                                  &key_press_event_1, 
                                                  &xHigherPrioritTaskWoken))
            {
                log_d("按键按下事件（下降沿）发送成功！触发时间：[%d] ms \r\n", HAL_GetTick());
            }
            else
            {
                // 可选：添加队列满时的异常处理（如丢弃 oldest 事件、打印警告等）
                log_d("按键事件队列满！按下事件发送失败！时间：[%d] ms \r\n", HAL_GetTick());
            }
        }

        /* 1.1 更新中断触发类型标记：下一次中断切换为上升沿（捕获按键松开） */
        irq_type = RASING_TYPE;

        /* 1.2 重新配置GPIO中断触发方式：从下降沿改为上升沿
         * 注意：HAL_GPIO_Init会覆盖该引脚原配置，需完整填写Pin、Mode、Pull参数
         */
        GPIO_InitTypeDef GPIO_InitStruct = {0};  // 初始化结构体（避免随机值导致配置异常）
            
        GPIO_InitStruct.Pin = KEY_Pin;           // 按键对应的GPIO引脚（宏定义，需提前配置）
        GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // 中断模式：上升沿触发
        GPIO_InitStruct.Pull = GPIO_PULLUP;      // 上拉电阻使能（按键未按下时引脚为高电平）
        HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct); // 应用配置到硬件
    }
    // ========== 分支2：当前为上升沿触发（按键松开事件） ==========
    else if (RASING_TYPE == irq_type)
    {
        // 定义按键松开事件结构体，存储事件类型和触发时间戳
        key_press_event_t key_press_event_2 = 
        {
            .edge_type    = RASING,           // 事件类型：上升沿（按键松开）
            .trigger_tick = HAL_GetTick()     // 记录事件触发时间（用于计算按键按下时长）
        };
    
        // 容错判断：检查队列是否已创建
        if (NULL == inter_key_queue)
        {
            log_d("按键事件队列未创建！触发时间：[%d] ms \r\n", HAL_GetTick());
        }        
        else
        {
            /* 中断安全地向队列头部发送按键松开事件 */
            if (pdTRUE == xQueueSendToFrontFromISR(inter_key_queue, 
                                                  &key_press_event_2, 
                                                  &xHigherPrioritTaskWoken))
            {
                log_d("按键松开事件（上升沿）发送成功！触发时间：[%d] ms \r\n", HAL_GetTick());
            }
            else
            {
                log_d("按键事件队列满！松开事件发送失败！时间：[%d] ms \r\n", HAL_GetTick());
            }
        }
        
        /* 1.1 更新中断触发类型标记：下一次中断切换回下降沿（捕获下一次按键按下） */
        irq_type = FALING_TYPE;

        /* 1.2 重新配置GPIO中断触发方式：从上升沿改回下降沿 */
        GPIO_InitTypeDef GPIO_InitStruct = {0};
            
        GPIO_InitStruct.Pin = KEY_Pin;           // 按键对应的GPIO引脚
        GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // 中断模式：下降沿触发
        GPIO_InitStruct.Pull = GPIO_PULLUP;      // 上拉电阻使能（保持引脚默认高电平）
        HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct); // 应用配置到硬件
    }

    /* 关键：中断退出前检查是否需要触发任务调度（FreeRTOS中断安全机制）
     * 若xHigherPrioritTaskWoken为pdTRUE，表示队列操作唤醒了更高优先级任务
     * 调用portYIELD_FROM_ISR触发上下文切换，确保高优先级任务及时执行
     * （注：部分FreeRTOS移植版本需用portEND_SWITCHING_ISR，需根据实际移植调整）
     */
    portYIELD_FROM_ISR(xHigherPrioritTaskWoken);
}