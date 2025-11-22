#ifndef __bsp_ili9341_LCD_drive_H
#define	__bsp_ili9341_LCD_drive_H
#include <stdio.h>
#include <stdint.h>


typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} font_def_t;

typedef struct spi_gpio_driver_interface_obj
{
    /*-----need to be realized by user based on the hardware platform-----*/
    uint8_t (*pf_spi_transmit)(const uint8_t *pData, uint32_t dataLength);
    uint8_t (*pf_spi_transmit_dma)(const uint8_t *pData, uint32_t dataLength);
    uint8_t (*pf_write_reset_pin)(uint8_t pinState);
    uint8_t (*pf_write_cs_pin)(uint8_t pinState); 
    uint8_t (*pf_write_dc_pin)(uint8_t pinState);  
    /*-----need to be realized by user based on the hardware platform-----*/
}basic_oper_driver_interface_t;

typedef struct ili9341_LCD_driver_interface_obj
{
    uint8_t (*pf_ili9341_LCD_write_data) (void * const driver_instance, uint8_t *pData, uint32_t dataLength);
    uint8_t (*pf_ili9341_LCD_write_simple_data) (void * const driver_instance,  const uint8_t dataSend);
    uint8_t (*pf_ili9341_LCD_write_command) (void * const driver_instance, const uint8_t cmd, uint8_t dataLength); 
}ili9341_LCD_spi_driver_interface_t;

typedef struct ili9341_LCD_timebase_interface_obj
{
    uint32_t    (*pf_get_tick_count)       (void);
    void    (*pf_delay_no_os) (uint32_t delay);
    
}ili9341_LCD_timebase_interface_t;

typedef struct ili9341_LCD_yield_interface_obj
{
    void (*pf_rtos_yield) (const uint32_t yield_cnt);    
}ili9341_LCD_yield_interface_t;



typedef struct bsp_ili9341_LCD_driver_obj
{
    basic_oper_driver_interface_t * p_ili9341_LCD_basic_operation;
    ili9341_LCD_spi_driver_interface_t * p_ili9341_LCD_spi_instance;
    ili9341_LCD_timebase_interface_t * p_timebase_instance;
    ili9341_LCD_yield_interface_t * p_yield_instance;

    /* Basic functions. */
    uint8_t (*pf_ili9341_LCD_init) (void * const ili9341_LCD_instance);
    uint8_t (*pf_set_direction) (void * const ili9341_LCD_instance, uint8_t direction);
    uint8_t (*pf_fill_color) (void * const ili9341_LCD_instance, uint16_t color);
    uint8_t (*pf_draw_pixel) (void * const ili9341_LCD_instance, uint16_t x, uint16_t y, uint16_t color);
    uint8_t (*pf_fill) (void * const ili9341_LCD_instance, uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color);
    uint8_t (*pf_draw_pixel_4px) (void * const ili9341_LCD_instance, uint16_t x, uint16_t y, uint16_t color);

    uint8_t (*pf_set_addr_window) (void * const ili9341_LCD_instance, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

    /* Graphical functions. */
    uint8_t (*pf_draw_line) (void * const ili9341_LCD_instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
    uint8_t (*pf_draw_rectangle) (void * const ili9341_LCD_instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
    uint8_t (*pf_draw_circle) (void * const ili9341_LCD_instance, uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
    uint8_t (*pf_draw_image) (void * const ili9341_LCD_instance, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);
    uint8_t (*pf_invert_colors) (void * const ili9341_LCD_instance, uint16_t invert);

    /* Text functions. */
    uint8_t (*pf_write_char) (void * const ili9341_LCD_instance, uint16_t x, uint16_t y, uint16_t ch, 
                              font_def_t font, uint16_t color, uint16_t bgcolor);
    uint8_t (*pf_write_string) (void * const ili9341_LCD_instance, uint16_t x, uint16_t y, const char *str, 
                                font_def_t font, uint16_t color, uint16_t bgcolor);
    /* Extented Graphical functions. */
    uint8_t (*pf_draw_filled_rectangle) (void * const ili9341_LCD_instance, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    uint8_t (*pf_draw_triangle) (void * const ili9341_LCD_instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, 
                                 uint16_t x3, uint16_t y3, uint16_t color);
    uint8_t (*pf_draw_filled_triangle) (void * const ili9341_LCD_instance, uint16_t x1, uint16_t y1, uint16_t x2, 
                                        uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
    uint8_t (*pf_draw_filled_circle) (void * const ili9341_LCD_instance, int16_t x0, int16_t y0, int16_t r, uint16_t color);                                                                 

    /* Command functions */
    uint8_t (*pf_tear_effect) (void * const ili9341_LCD_instance, uint8_t tear);

    /*test function*/
    uint8_t (*pf_test_your_device) (void * const ili9341_LCD_instance);
}bsp_ili9341_LCD_driver_t;



uint8_t ili9341_LCD_instance_driver_link  (bsp_ili9341_LCD_driver_t * const ili9341_LCD_driver_instance,
                                       basic_oper_driver_interface_t * const ili9341_LCD_basic_operation_instance,
                                       ili9341_LCD_spi_driver_interface_t * const ili9341_LCD_spi_gpio_instance,
                                       ili9341_LCD_timebase_interface_t   * const timebase_instance,
                                       ili9341_LCD_yield_interface_t      * const yield_instance);



#endif /* __USART_H */
																			 