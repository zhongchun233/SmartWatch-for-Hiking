/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file bsp_led.c
 * 
 * @par dependencies 
 * - bsp_led.h       // LED相关宏定义、枚举、函数声明（必须包含）
 * - stdio.h         // 标准输入输出（log_d函数依赖）
 * - stdint.h        // 标准整数类型（uint32_t等）
 * 
 * @author Jack | R&D Dept. | EternalChip 立芯嵌入式
 * 
 * @brief 基于STM32 HAL库的LED硬件抽象层（BSP）实现
 *        提供LED的基础操作（点亮、熄灭、翻转、闪烁3次），并通过FreeRTOS任务+队列实现异步控制
 * 
 * Processing flow:
 * 1. 初始化LED任务和消息队列；
 * 2. 任务循环中监听队列消息；
 * 3. 接收LED操作指令（如LED_ON/LED_OFF），调用led_on_off执行具体硬件操作。
 * 
 * @version V1.0 2024-10-30
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/

#include "bsp_LED.h"  // 包含LED相关的宏定义、枚举类型、函数声明
#include "debug.h" //******************************** 宏定义区 **********************************//

//****************************************************************************//

//********************* FreeRTOS任务相关定义 **********************//
osThreadId_t led_TaskHandle;  // LED任务句柄（用于操作任务，如暂停、删除）
osThreadId_t Green_led_TaskHandle;  // LED任务句柄（用于操作任务，如暂停、删除）
// LED任务属性配置结构体（FreeRTOS任务创建必需）
const osThreadAttr_t led_Task_attributes = {
  .name = "led_Task",          // 任务名称（调试用，便于识别）
  .stack_size = 128 * 4,       // 任务栈大小：128字 × 4字节/字 = 512字节（足够LED控制逻辑）
  .priority = (osPriority_t) osPriorityHigh,  // 任务优先级：高优先级（确保LED响应及时）
};

const osThreadAttr_t Green_led_Task_attributes = {
  .name = "Green_led_Task",          // 任务名称（调试用，便于识别）
  .stack_size = 128 * 4,       // 任务栈大小：128字 × 4字节/字 = 512字节（足够LED控制逻辑）
  .priority = (osPriority_t) osPriorityHigh,  // 任务优先级：高优先级（确保LED响应及时）
};
//********************* FreeRTOS任务相关定义 **********************//

//********************* FreeRTOS队列相关定义 ********************//
QueueHandle_t led_queue;  // LED消息队列句柄（用于接收上层下发的LED操作指令）
//********************* FreeRTOS队列相关定义 ********************//

/**
 * @brief  LED核心操作函数（点亮、熄灭、翻转、闪烁3次）
 * @param  led_operation: LED操作指令（枚举类型，取值：LED_ON/LED_OFF/LED_TOGGLE/LED_BLINK_3_TIMES）
 * @return led_status_t: 函数执行状态（LED_OK表示操作成功，其他值为异常）
 * @note   底层调用STM32 HAL库GPIO操作函数，直接操作硬件引脚：
 *         - 假设LED硬件连接为「低电平点亮」（GPIO_PIN_RESET=点亮，GPIO_PIN_SET=熄灭）
 *         - 闪烁3次逻辑：通过for循环切换引脚状态6次（3次亮灭），每次延时300ms
 */
led_status_t led_on_off(led_operation_t led_operation)
{
    led_status_t ret = LED_OK;  // 函数返回状态，默认初始化成功

    // 操作1：LED点亮（低电平有效）
    if (LED_ON == led_operation)
    {
        // 调用HAL库函数：设置LED对应GPIO引脚为低电平（GPIO_PIN_RESET）
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); 
    }

    // 操作2：LED熄灭（高电平有效）
    if (LED_OFF == led_operation)
    {
        // 调用HAL库函数：设置LED对应GPIO引脚为高电平（GPIO_PIN_SET）
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    }

    // 操作3：LED状态翻转（当前亮→灭，当前灭→亮）
    if (LED_TOGGLE == led_operation)
    {
        // 调用HAL库函数：翻转LED对应GPIO引脚电平
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }

    // 操作4：LED闪烁3次（亮300ms→灭300ms，循环3次）
    if (LED_BLINK_3_TIMES == led_operation)
    {
        // for循环6次：每次翻转电平，实现3次完整闪烁（6次翻转=3次亮+3次灭）
        for (int i = 0; i < 6; i++)
        {
            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);  // 翻转LED电平
            vTaskDelay(300);  // 延时300ms（FreeRTOS延时函数，单位：ms）
        }
    }

    return ret;  // 返回操作状态（始终返回LED_OK，可扩展异常处理）
}

/**
 * @brief  FreeRTOS LED任务函数（独立任务，循环运行）
 * @param  argument: 任务入口参数（未使用，FreeRTOS任务标准参数）
 * @return 无返回值（FreeRTOS任务需无限循环，不可返回）
 * @note   核心逻辑：
 *         1. 创建LED消息队列（用于接收上层下发的LED操作指令）；
 *         2. 任务循环中周期性打印调试信息，监听队列消息；
 *         3. 收到队列消息后，调用led_on_off函数执行对应LED操作；
 *         4. 任务延时100ms，降低CPU占用率。
 */
void led_task_func(void *argument)
{

	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		osDelay(1000);	
	
	}
    /* USER CODE END 5 */
}
void Green_led_task_func(void *argument)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		osDelay(1000);	
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
		osDelay(1000);	
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
		osDelay(1000);	
	}


}
//******************************** 宏定义区 **********************************//
// （文件末尾无额外宏定义，此处为格式对齐）
//****************************************************************************//