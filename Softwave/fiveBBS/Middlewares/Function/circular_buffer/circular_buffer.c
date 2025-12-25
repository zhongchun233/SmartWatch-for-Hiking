#include "circular_buffer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "osal_heap.h"
#include "elog.h"

//typedef struct {
//    uint32_t                           head;
//    uint32_t                           tail;
//	  data_type_t  data[];//ÈáÐÔÊý×é
//}circular_buffer_t;
//circular_buffer_t URAT_circular_buffer;

circular_buffer_t * create_empty_circular_buffer(uint32_t side)
{
	circular_buffer_t * p_buffer =
	      (circular_buffer_t *)osal_heap_malloc(sizeof(circular_buffer_t)+side * sizeof(data_type_t));
	if(p_buffer == NULL)
	{
		return NULL;
	}
	p_buffer->side = side;
	p_buffer->head = 0;  
	p_buffer->tail = 0;
	return p_buffer;
}

uint8_t buffer_is_empty(circular_buffer_t *p_buffer)
{
	if(p_buffer == NULL) return 0xff;
  return (p_buffer->head == p_buffer->tail) ? 0xff : 0;
}

uint8_t buffer_is_full(circular_buffer_t *p_buffer)
{
    if(p_buffer == NULL) return 0xff;
    return ((p_buffer->head + 1) % p_buffer->side == p_buffer->tail) ? 0xff : 0;
}

uint8_t insert_data(circular_buffer_t *p_buffer, data_type_t data)
{
 if(p_buffer == NULL || buffer_is_full(p_buffer))
 {
	return 0;
 }
	p_buffer->data[p_buffer->head] = data;
 p_buffer->head = (p_buffer->head + 1) % p_buffer->side;
 return 0xff;
 
}

uint8_t get_data(circular_buffer_t *p_buffer, data_type_t *data)
{
 if(p_buffer == NULL || buffer_is_empty(p_buffer))
 {
	return 0;
 }
	*data = p_buffer->data[p_buffer->tail];
	p_buffer->tail = (p_buffer->tail + 1) % p_buffer->side;
 return 0xff;

}

uint8_t get_head_pos   (circular_buffer_t * p_buffer,uint32_t * head)
{
 if(p_buffer == NULL || head == NULL)
 {
	return 0;
 }
return 0xff;

}

uint8_t  head_pos_increment         (circular_buffer_t * p_buffer,uint32_t increament_num)
{
	if(p_buffer == NULL || increament_num == 0)
	{
		return 0;
	}

	return 0xff;
}