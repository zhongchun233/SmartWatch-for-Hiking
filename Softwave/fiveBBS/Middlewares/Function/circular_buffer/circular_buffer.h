#ifndef __MID_CIRCULAR_BUFFER__
#define __MID_CIRCULAR_BUFFER__

#include <stdint.h>

#define CIRCULAR_BUFFER_SIZE 100

typedef uint8_t data_type_t;

typedef struct {
    uint32_t                           head;
    uint32_t                           tail;
		uint32_t                           side;
	  data_type_t  data[];//ÈáÐÔÊý×é
}circular_buffer_t;

circular_buffer_t * create_empty_circular_buffer(uint32_t side);

uint8_t buffer_is_empty(circular_buffer_t *p_buffer);

uint8_t buffer_is_full(circular_buffer_t *p_buffer);

uint8_t insert_data(circular_buffer_t *p_buffer, data_type_t data);

uint8_t get_data(circular_buffer_t *p_buffer, data_type_t *data);

uint8_t get_head_pos   (circular_buffer_t * p_buffer,uint32_t * head);

uint8_t  head_pos_increment         (circular_buffer_t * p_buffer,uint32_t increament_num);
#endif // end __MID_CIRCULAR_BUFFER__