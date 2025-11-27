#include "bsp_adaper_port_button.h"
static Button btn1;

osThreadId_t key_TaskHandle;  // LED任务句柄（用于操作任务，如暂停、删除）
// LED任务属性配置结构体（FreeRTOS任务创建必需）
const osThreadAttr_t key_Task_attributes = {
  .name = "led_Task",          // 任务名称（调试用，便于识别）
  .stack_size = 128 * 4,       // 任务栈大小：128字 × 4字节/字 = 512字节（足够LED控制逻辑）
  .priority = (osPriority_t) osPriorityHigh,  // 任务优先级：高优先级（确保LED响应及时）
};
uint8_t read_button_gpio(uint8_t button_id)
{
    switch (button_id) {
        case 1:
            return HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin);
				case 2:
            return HAL_GPIO_ReadPin(BUTTON2_GPIO_Port, BUTTON2_Pin);
				case 3:
            return HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin);
				case 4:
            return HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin);
				
        default:
            return 0;
    }
}
void btn1_single_click_handler(Button* btn_handle)
{
    log_d("Button 1: Single Click\n");
}

void btn1_DOUBLE_click_handler(Button* btn_handle)
{
    log_d("Button 1: DOUBLE Click\n");
}
void key_task_func(void *argument)
{
	for(;;)
	{
		button_ticks();
		vTaskDelay(pdMS_TO_TICKS(5));
	
	}

}





void drv_adapter_button_register(void)
{
// 初始化按键 (active_level: 0=低电平有效, 1=高电平有效)
		button_init(&btn1, read_button_gpio, 1, 1);
		button_attach(&btn1, BTN_SINGLE_CLICK, btn1_single_click_handler);
		button_attach(&btn1, BTN_DOUBLE_CLICK, btn1_DOUBLE_click_handler);
		button_start(&btn1);
	
	
	
	
		key_TaskHandle = osThreadNew(key_task_func, NULL, &key_Task_attributes);



}

