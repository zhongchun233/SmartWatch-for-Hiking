/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file bsp_uart_driver.h
 * 
 * @par dependencies 
 * - ec_bsp_aht21_reg.h
 * - stdio.h
 * - stdint.h
 * 
 * @author Jack | R&D Dept. | EternalChip 立芯嵌入式
 * 
 * @brief Provide the HAL APIs of AHT21 and corresponding opetions.
 * 
 * Processing flow:
 * 
 *帧头：0xFE
 *帧尾：0xFF
 *状态机的实现：Switch
 *中断（系统硬件）将数据传输完成的事情告诉串口驱动程序（前端），串口驱动程序再通知APP解包（后端）
 *系统硬件（中断）-》前端-》后端
 *
 *中断：
 *1、将数据存入环形缓冲区
 *2、通知前端，数据已经就绪（队列）
 *3、开启下一次串口的接收
 *
 *前端：
 *1、buffer是否已满-》停下串口
 *2、将当前数据就绪的事件发送给后端
 *
 *后端：
 *1、对数据进行解包
 *1.1、检测到包头，则开始输出
 *1.2、检测到包尾，停止输出
 * 
 * 
 * 
 * 
 * call directly.
 * 
 * @version V1.0 2023-12-03
 *
 * @note 1 tab == 4 spaces!
 * 
 * *****************************************************************************/
#include "bsp_uart_driver.h"






void BSP_uart_driver_func(void *argument)
{

}


/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

}

/**
 * @brief pass the pointer to eternal caller.
 * 
 *  
 * @param[in] void 
 * 
 * @return    circular_buffer_t : 
                        pointer to the circcular buffer
 * 
 * */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
   
}






/* USER CODE END 1 */