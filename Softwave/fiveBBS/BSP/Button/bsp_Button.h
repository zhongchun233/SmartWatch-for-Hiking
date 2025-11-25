#ifndef __bsp_Button_H
#define	__bsp_Button_H
//******************************** Includes *********************************//


#include <stdint.h>               //  the compiling lib.
#include <stdio.h>

#include "main.h"                 //  Core / OS layer
#include "cmsis_os.h"


#include "queue.h"                //  specific file for operations
#include "stm32f1xx_hal.h"        
#include "stm32f1xx_hal_gpio.h"


#define SHORT_PRESS_TIME    (500)              // Specify the short press time

//******************************** Includes *********************************//
#define KEY_CALLBACK        void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
#define KEY_Pin GPIO_PIN_0
#define KEY_GPIO_Port GPIOA
//*********************Thread_Func **********************//
extern osThreadId_t key_TaskHandle;
extern const osThreadAttr_t key_Task_attributes;
//*********************Thread_Func **********************//

//*********************Queue_Handler ********************//
extern QueueHandle_t              key_queue;
extern QueueHandle_t        inter_key_queue;
//*********************Queue_Handler ********************//

typedef enum
{
  KEY_OK                = 0,           /* Operation completed successfully.  */
  KEY_ERROR             = 1,           /* Run-time error without case matched*/
  KEY_ERRORTIMEOUT      = 2,           /* Operation failed with timeout      */
  KEY_ERRORRESOURCE     = 3,           /* Resource not available.            */
  KEY_ERRORPARAMETER    = 4,           /* Parameter error.                   */
  KEY_ERRORNOMEMORY     = 5,           /* Out of memory.                     */
  KEY_ERRORISR          = 6,           /* Not allowed in ISR context         */
  KEY_RESERVED          = 0x7FFFFFFF   /* Reserved                           */
} key_status_t;

typedef enum
{
  KEY_PRESSED           = 0,           /* Operation completed successfully.  */
  KEY_NOT_PRESSED       = 1,           /* Run-time error without case matched*/
  KEY_SHORT_PRESSED     = 2,           /* Operation                          */
  KEY_LONG_PRESSED      = 3,           /* Operation                          */
} key_press_status_t;

typedef enum
{
  RASING                = 0,           /* Operation completed successfully.  */ 
  FAILING               = 1,           /* Operation failed                   */
} key_trigger_edge_t;


typedef struct
{
  key_trigger_edge_t    edge_type;     /* Operation completed successfully.  */
  uint32_t           trigger_tick;     /* Operation completed successfully.  */
} key_press_event_t;
void key_task_func(void *argument);

key_status_t key_scan      (key_press_status_t * key_value);

key_status_t key_scan_short_long_press(key_press_status_t *  key_value, 
                                       uint32_t      short_press_time);
#endif /* __USART_H */