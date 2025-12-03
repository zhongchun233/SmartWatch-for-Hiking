#include "uart_parse_task.h"



QueueHandle_t queue_irq_rec_A = NULL;
static circular_buffer_t * g_circular_buffer_from_driver = NULL;
static uint8_t app_subscribe_number = 0;
//APP订阅者注册数组
app_subscriber_t app_subscribers[PROTO_MAX_SUBSCRIBERS];

uint8_t app_subscribe_array(uint8_t cmd[PROTO_MAX_CMD_SUBSCRIBERS], QueueHandle_t rx_handle)
{
    for ( uint8_t i = 0; i < PROTO_MAX_CMD_SUBSCRIBERS; i++)
    {
        if ( app_subscribe_number >= PROTO_MAX_SUBSCRIBERS)
        {
            return 0;
        }
        app_subscribers[app_subscribe_number++].cmd[i] = cmd[i];
    }
    app_subscribers[app_subscribe_number++].rx_handle = rx_handle;
    return 1;
}
void uart_rec_A_func(void *argument)
{
  /* USER CODE BEGIN uart_rec_A_func */
  /* Infinite loop */
	static uint32_t receive_data = 0;
    static uint32_t receive_data_cnt = 0;
	log_i("uart_rec_A_func Init Success");
	queue_irq_rec_A = NULL;
	queue_irq_rec_A = xQueueCreate( 1, 4 );
	if ( NULL == queue_irq_rec_A)
	{
		log_i("queue_irq_rec_A Init failed");
		
	} else {
		log_i("queue_irq_rec_A Init Success");
		log_i("queue_irq_rec_A = [%x]",queue_irq_rec_A);
	}
    
    g_circular_buffer_from_driver = get_circular_buffer();
    
    for(;;)
    {
		
		xQueueReceive(queue_irq_rec_A,&receive_data,0xffff);
		log_i("receive_data = [%x]",receive_data);

        /*
        APP（后端）线程中获得通知以后，
        第零步、依次去将环形缓冲区的数据进行解析
        第一步、寻找帧头，如果检测到帧头，则开始输出帧头后的数据
        第二步，持续输出数据，并时刻检查帧尾
        第三步，检测到帧尾，发送一个换行
        */
      
        //第零步、依次去将环形缓冲区的数据进行解析
        if ( NULL == g_circular_buffer_from_driver )
        {
            log_e("error pointer");
            return;
        }
       
        while( 0x00 != buffer_is_empty(g_circular_buffer_from_driver) )
        {
            uint8_t temp_data = 0;
            
            static uint8_t data_counter        =      0;
            static uint8_t temp_data_array[20] = {0x00};
            
            
           
            if ( 0x00 == get_data(g_circular_buffer_from_driver,&temp_data))
            {
                //log_i("buffer_get_success");
            }
            osDelay(2);
            log_i("buffer_read_out from APP = [%d]",temp_data );
            log_i("g_circular_buffer_from_driver->head = [%d]",g_circular_buffer_from_driver->head);
            
            
            
            // Parse data
            // 第一步、寻找帧头，如果检测到帧头，则开始输出帧头后的数据
            static uint32_t status = FRAME_NOT_DETECTED;
            
            switch(status)
            {
                case FRAME_NOT_DETECTED:
                       // check if FRAME_HEAD is comming 
                       if( FRAME_HEAD_FLAG == temp_data )
                       {
                           status = FRAME_HEAD;
                           log_i("Data packet start");
                       }
                break;
                
                case FRAME_HEAD:
                       // 数据包校验    
                       /*
                           总结：检测到帧尾之前，
                                 将数据全部存起来，
                                 检测到帧尾后，开始计算校验和，
                                 若相等，则输出。
                       */
                       if( FRAME_END_FLAG == temp_data )
                       {
                            log_i("Data packet end");
                            status = FRAME_NOT_DETECTED;
                            
                            // 校验和开始计算
                            uint32_t data_sum = temp_data_array[data_counter-1];
                            uint32_t data_sum_temp = 0;
                            
                            log_i("data_sum = [%d]",data_sum);
                       
                             for(int i = 0; i < (data_counter-1); i++)
                            {
                                log_i("receive data = [%d]",\
                                           temp_data_array[i]);
                            }

                            // 计算校验和
                            for(int i = 0; i < (data_counter-1); i++)
                            {
                                data_sum_temp += temp_data_array[i];
                            }
                            
                            // 计算出来的校验和
                            log_i("calculated data_sum = [%d]",data_sum_temp);
                            
                            // 打印净荷数据
                            if( data_sum == data_sum_temp )
                            {
                                for(int i = 0; i < (data_counter-1); i++)
                                {
                                    log_i("calculated receive data = [%d]",\
                                               temp_data_array[i]);
                                }
                                /*
                                // APP 订阅者描述
                                typedef struct {
                                    uint8_t cmd[10];
                                    QueueHandle_t rx_handle;
                                } app_subscriber_t;
                                */

                                // 发送给APP 订阅者的数据包
                                //typedef struct {
                                //    uint32_t data_cnt;
                                //    uint8_t data[PROTO_MAX_LENGTH];
                                //} 

                                //To Do:
                                //原有协议：HEAD(1) + DATA(N) + CHK(1) + TAIL(1)
                                //协议：HEAD(1) + CMD(1) + LEN(1) + DATA(N) + CHK(1) + TAIL(1)
                                //1.解析协议中的CMD数据，CMD在数据头后的第一个字节
                                log_i("CMD = [%d]",temp_data_array[0]);
                                //2.解析协议中的数据，数据在Len字节后面的数据
                                //2.2.构造队列发送的数据包
                                app_msg_t temp_msg ={ .data_cnt = receive_data_cnt++};
                                for( uint32_t i = 0; i < temp_data_array[1]; i++)
                                { 
                                    temp_msg.data[i] = temp_data_array[1+i];
                                }
                                
                                //3 转发对应的APP
                                for(uint32_t i = 0; i < PROTO_MAX_SUBSCRIBERS; i++)
                                {
                                    for(uint32_t j = 0; j < PROTO_MAX_CMD_SUBSCRIBERS; j++)
                                    {
                                        if(app_subscribers[i].cmd[j] == temp_msg.data[0])
                                        {
                                            xQueueSend(app_subscribers[i].rx_handle, &temp_msg, 0);
                                        }

                                    }
                                }
                            }
                            
                            for(int i = 0; i < (data_counter-1); i++)
                            {
                                temp_data_array[data_counter]=0x00;
                            }
                            data_counter = 0;
                            
                           
                       } else {
                            log_i("FRAME_HEAD data = [%d]",temp_data );
                            
                           
                           //暂存
                           temp_data_array[data_counter] = temp_data;
                           
                           data_counter++;
                           
                           log_i("data_counter = [%d]",data_counter);
                       }
                break;
                
                //case  :
                //break;
                
                //default:
            
            }
            
        }

    
      
      osDelay(1);
  }
  /* USER CODE END uart_rec_A_func */
}


