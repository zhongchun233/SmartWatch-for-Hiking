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




#define BUFFER_A 0
#define BUFFER_B 1

#define IRQ_SEND_TO_THREAD 0xA1A2A3A4
#define FRONT_SEND_TO_END  0xB1B2B3B4

#if 0 // AB-Buffer
uint8_t flag_AB = BUFFER_A;
uint8_t g_data_buffer_A[1] = {0x00};
uint8_t g_data_buffer_B[1] = {0x00};
#endif // end of AB-Buffer

#if 1 // circular buffer

uint8_t g_data_buffer = 0;

#endif // end of circular buffer
static circular_buffer_t * g_circular_buffer_irq_thread = NULL;//.bss
static QueueHandle_t       queue_uart_irq_thread        = NULL;

extern QueueHandle_t queue_irq_rec_A;
/*
 *前端：
 *1、buffer是否已满-》停下串口
 *2、将当前数据就绪的事件发送给后端
*/

void uart_driver_func(void *argument)
{
    uint32_t receive_data = 0;
    
    
    //0. 创建环形缓冲区 alloc the ring buffer
    circular_buffer_t * p_circular_buffer = create_empty_circular_buffer();
    if ( NULL == p_circular_buffer )
    {
        log_e("circular_buffer create failed");
    }
    
    //0.1 环形缓冲区空间挂载
    log_i( "p_circular_buffer = [0x%p]",p_circular_buffer);
    g_circular_buffer_irq_thread = p_circular_buffer;

    
    // 1.创建队列，用于系统硬件（串口）向我传递消息
    queue_uart_irq_thread = xQueueCreate( 1, 4 );
    if ( NULL == queue_uart_irq_thread )
    {
        log_e("circular_buffer create failed");
    }
    log_i("queue_uart_irq_thread create success");
	
    
    // 2.启动串口接收
	HAL_StatusTypeDef ret = HAL_OK;
	
    // 2.1单字节接收
	//ret = HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);
	
    // 2.2 串口空闲中断接收+DMA半满全满
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1,p_circular_buffer->data,CIRCULAR_BUFFER_SIZE);
    
    
	if(HAL_OK == ret)
	{
		log_i("HAL UART Init Success.");
	} 
	else
	{
		log_i("HAL UART Init Failed");
	}
	/* DEBUG USART */
    

    for(;;)
    {
        //3、判断是否有中断发送来队列通知环形缓冲区已有数据
        xQueueReceive(queue_uart_irq_thread,&receive_data,0xffff);
        log_i("front recive data from irq = [%x]",receive_data);
        if( IRQ_SEND_TO_THREAD == receive_data )
        {
            //4、将当前数据就绪的事件发送给后端
            uint32_t send_to_end = FRONT_SEND_TO_END;
            BaseType_t ret_queue = pdTRUE;
            ret_queue = xQueueGenericSend(  queue_irq_rec_A, \
                                               &send_to_end, \
                                                          0, \
                                              queueOVERWRITE);
            if( pdTRUE != ret_queue)
            {
                log_e("send error");
                return;
            }
            
            log_d("front to end send success"); 

        }
    }
  /* USER CODE END uart_rec_A_func */
}


/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
#if 0 // AB buffer test
	log_d("HAL_UART_RxCpltCallback");
	
    
    
    HAL_StatusTypeDef ret = HAL_OK;
    
    if ( BUFFER_A == flag_AB )
    {
        log_d("g_data_buffer_A = [%c]",g_data_buffer_A[0]);
        ret = HAL_UART_Receive_IT(&huart1, g_data_buffer_B, 1);
        if ( HAL_OK != ret )
        {
            log_e("HAL_UART_Receive_IT error!");
        }
        flag_AB = BUFFER_B;
    } 
    else 
    {
        log_d("g_data_buffer_B = [%c]",g_data_buffer_B[0]);
        ret = HAL_UART_Receive_IT(&huart1, g_data_buffer_A, 1);
        if ( HAL_OK != ret )
        {
            log_e("HAL_UART_Receive_IT error!");
        }
        flag_AB = BUFFER_A;
    }
#endif // end of AB buffer test
    
   
    
    
/*
2 将 1 实现的 ABCDEFG buffer 形式的串口通信架构实现为环形缓冲区
（视频中线程 A 此时强行搬运数据，暂时不需要解析）
1.ABbuffer切换为环形缓冲区
{
  1.1把数据的写入（在单字节中断中）指向环形缓冲区
  测试：写入后，立马读出，验证
}
*/
#if 0  // circular_buffer test
      if(NULL == g_circular_buffer_irq_thread)
      {
          log_e("error with NULL pointer of g_circular_buffer");
          return;
      }
      
      uint8_t ret = 0;
      
      ret = insert_data  (g_circular_buffer_irq_thread,\
                                          g_data_buffer);
      
      if( 0x00 == ret )
    {
        uint8_t temp_data = 0;
        if ( 0x00 == get_data(g_circular_buffer_irq_thread,
                                                &temp_data))
        {
            log_i("circular_buffer_get_success");
        }
        log_i("buffer_read_out = [%d]",temp_data );
    }
      
    HAL_StatusTypeDef ret_1 = HAL_OK;
	
    ret_1 = HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);
        
        
    if(HAL_OK == ret_1)
    {
        log_i("HAL UART Init Success.");
    } 
    else
    {
        log_i("HAL UART Init Failed");
    }
#endif //end of circular_buffer test

/*
 *中断：
 *1、将数据存入环形缓冲区
 *2、通知前端，数据已经就绪（队列）
 *3、开启下一次串口的接收
*/
#if 1  // parse_task
    
    // 1、将数据存入环形缓冲区
    if(NULL == g_circular_buffer_irq_thread)
    {
      log_e("error with NULL pointer of g_circular_buffer");
      return;
    }

    uint8_t ret = 0;

    ret = insert_data  (g_circular_buffer_irq_thread,\
                                      g_data_buffer);
    
    // 2、通知前端，数据已经就绪（队列）
    uint32_t send_to_thread = IRQ_SEND_TO_THREAD;
    BaseType_t ret_queue = pdTRUE;
    ret_queue = xQueueGenericSendFromISR( queue_uart_irq_thread, \
                                                 &send_to_thread,\
                                                            NULL,\
                                                   queueOVERWRITE);
	if( pdTRUE != ret_queue)
    {
        log_e("send error");
        return;
    }
    
    log_d("xQueueSend send to front success"); 
    
    // 3、开启下一次串口的接收
    HAL_StatusTypeDef ret_1 = HAL_OK;
	
    ret_1 = HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);
    
#endif //end of parse_task
    //g_data_buffer_A
	//ret = xQueueSendFromISR( queue_irq_rec_A, &send_data_to_rec_A,NULL);
	//log_d("ret = [%d]",ret);
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
circular_buffer_t * get_circular_buffer(void)
{
        
    if ( NULL == g_circular_buffer_irq_thread )
    {
        return NULL;
    }
    
    return g_circular_buffer_irq_thread;
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
    log_d("HAL_UARTEx_RxEventCallback size = [%d]",Size ); 
}



void dma_half_irq_callback(uint32_t number_of_data)
{
    //log_d("dma_half_irq = [%d]",number_of_data ); 
    /*比如,当前缓冲区长度为10（0~9)，
    head当前位置为32，产生半满说明数据已经到达4，
    于是32%5 = 2，
    所以将head添加到34即可（因为0~4中，它在2的位置，所以+2就到34）。*/
    uint32_t head_pos = 0;
    uint8_t ret = 0;
    //1.获取当前位置
    ret=get_head_pos(g_circular_buffer_irq_thread,&head_pos);
    if( 0x00 != ret)
    {
        log_d("get head pos error" ); 
    }
//    log_d("head_pos = [%d]",head_pos ); 
    //2.获取进入半满中断时，数据已经到达的位置：(CIRCULAR_BUFFER_SIZE/2)-1
    uint32_t current_data_pos = (CIRCULAR_BUFFER_SIZE/2);
//    log_d("current_data_pos = [%d]",current_data_pos  ); 
    
    //3.对heap进行取余数
    uint32_t pos_in_buffer = head_pos%(CIRCULAR_BUFFER_SIZE/2);
//    log_d("pos_in_buffer = [%d]",pos_in_buffer); 
    
    //4.算出当前应该偏移的数量
    uint32_t move_pos = 0x00;
    if(current_data_pos < pos_in_buffer)
    {
        move_pos = (current_data_pos+CIRCULAR_BUFFER_SIZE) - pos_in_buffer;
//        log_d("move_pos_dam_complete = [%d]",move_pos);
    } 
    else
    {
        move_pos = current_data_pos - pos_in_buffer;
//        log_d("move_pos_normal = [%d]",move_pos);
    }
    
    //5.对head位置进行累加
    head_pos_increment(g_circular_buffer_irq_thread,move_pos);
    
    //6.对前端线程进行通知
    //TBD
    uint32_t send_to_thread = IRQ_SEND_TO_THREAD;
    BaseType_t ret_queue = pdTRUE;
    ret_queue = xQueueGenericSendFromISR( queue_uart_irq_thread, \
                                                 &send_to_thread,\
                                                            NULL,\
                                                   queueOVERWRITE);
    log_d("dma_half_irq_callback ret_queue = [%d]",ret_queue); 
    //test:
//    ret=get_head_pos(g_circular_buffer_irq_thread,&head_pos);
//    if( 0x00 != ret)
//    {
//        log_d("get head pos error" ); 
//    }
//    log_d("head_pos = [%d]",head_pos ); 
//    
//    pos_in_buffer = head_pos%(CIRCULAR_BUFFER_SIZE/2);
//    log_d("pos_in_buffer = [%d]",pos_in_buffer); 
    
}

void dma_comp_irq_callback(uint32_t number_of_data)
{
    //log_d("dma_comp_irq = [%d]",number_of_data ); 
    /*比如,当前缓冲区长度为10（0~9)，
    head当前位置为37，产生全满说明数据已经到达9，
    于是37%10 = 7，
    将head添加到39即可（因为0~9中，它在7的位置，所以+2就到39）。
    */
    uint32_t head_pos = 0;
    uint8_t ret = 0;
    
    //1.获取当前位置
    ret=get_head_pos(g_circular_buffer_irq_thread,&head_pos);
    if( 0x00 != ret)
    {
        log_d("get head pos error" ); 
    }
//    log_d("[c]head_pos = [%d]",head_pos ); 
    //2.获取进入全满中断时，数据已经到达的位置：(CIRCULAR_BUFFER_SIZE)-1
    uint32_t current_data_pos = (CIRCULAR_BUFFER_SIZE);
//    log_d("[c]current_data_pos = [%d]",current_data_pos  ); 
    
    //3.对heap进行取余数
    uint32_t pos_in_buffer = head_pos%(CIRCULAR_BUFFER_SIZE);
//    log_d("[c]pos_in_buffer = [%d]",pos_in_buffer); 
    
    //4.算出当前应该偏移的数量
    uint32_t move_pos = 0x00;
    if(current_data_pos < pos_in_buffer)
    {
        move_pos = (current_data_pos+CIRCULAR_BUFFER_SIZE) - pos_in_buffer;
//        log_d("[c]move_pos_dam_complete = [%d]",move_pos);
    } 
    else
    {
        move_pos = current_data_pos - pos_in_buffer;
//        log_d("[c]move_pos_normal = [%d]",move_pos);
    }
    
    //5.对head位置进行累加
    head_pos_increment(g_circular_buffer_irq_thread,move_pos);
    
    //6.对前端线程通知
    //TBD
    uint32_t send_to_thread = IRQ_SEND_TO_THREAD;
    BaseType_t ret_queue = pdTRUE;
    ret_queue = xQueueGenericSendFromISR( queue_uart_irq_thread, \
                                                 &send_to_thread,\
                                                            NULL,\
                                                   queueOVERWRITE);
    log_d("dma_comp_irq_callback ret_queue = [%d]",ret_queue); 
    //test:
    ret=get_head_pos(g_circular_buffer_irq_thread,&head_pos);
    if( 0x00 != ret)
    {
        log_d("get head pos error" ); 
    }
//    log_d("[c]head_pos = [%d]",head_pos ); 
    
    pos_in_buffer = head_pos%(CIRCULAR_BUFFER_SIZE);
//    log_d("[c]pos_in_buffer = [%d]",pos_in_buffer); 
    
    
    
}

void uart_idle_irq_callback(uint32_t number_of_data)
{
    //log_d("uart_idle_irq = [%d]",number_of_data ); 
    /*
    比如,当前缓冲区长度为10（0~9)，
    head当前位置为35，
    产生空闲中断得到的Size数据为9,
    于是需要增加(9-1)-(35%10)= 3，
    将head添加到38即可。
    */
    uint32_t head_pos = 0;
    uint8_t ret = 0;
    
    //1.获取当前位置
    ret=get_head_pos(g_circular_buffer_irq_thread,&head_pos);
    if( 0x00 != ret)
    {
        log_d("get head pos error" ); 
    }
//    log_d("[U]head_pos = [%d]",head_pos ); 
    //2.获取进入空闲中断时，数据已经到达的位置：number_of_data-1
    uint32_t current_data_pos = number_of_data;
//    log_d("[U]current_data_pos = [%d]",current_data_pos  ); 
    
    //3.对heap进行取余数
    uint32_t pos_in_buffer = head_pos%(CIRCULAR_BUFFER_SIZE);
//    log_d("[U]pos_in_buffer = [%d]",pos_in_buffer); 
    
    //4.算出当前应该偏移的数量
    uint32_t move_pos = 0x00;
    if(current_data_pos < pos_in_buffer)
    {
        move_pos = (current_data_pos+CIRCULAR_BUFFER_SIZE) - pos_in_buffer;
//        log_d("[U]move_pos_dam_complete = [%d]",move_pos);
    } 
    else
    {
        move_pos = current_data_pos - pos_in_buffer;
//        log_d("[U]move_pos_normal = [%d]",move_pos);
    }
    //5.对head位置进行累加
    head_pos_increment(g_circular_buffer_irq_thread,move_pos);
    //6.进行通知
    
        uint32_t send_to_thread = IRQ_SEND_TO_THREAD;
    BaseType_t ret_queue = pdTRUE;
    ret_queue = xQueueGenericSendFromISR( queue_uart_irq_thread, \
                                                 &send_to_thread,\
                                                            NULL,\
                                                   queueOVERWRITE);
    log_d("uart_idle_irq_callback ret_queue = [%d]",ret_queue); 
    //test:
    ret=get_head_pos(g_circular_buffer_irq_thread,&head_pos);
    if( 0x00 != ret)
    {
        log_d("get head pos error" ); 
    }
//    log_d("[U]head_pos = [%d]",head_pos ); 
    
    pos_in_buffer = head_pos%(CIRCULAR_BUFFER_SIZE);
//    log_d("[U]pos_in_buffer = [%d]",pos_in_buffer); 
    
    
}





/* USER CODE END 1 */