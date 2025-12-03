#include "circular_buffer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "elog.h"

/**
 * @brief 创建一个空的循环缓冲区
 * @details 为循环缓冲区结构体分配内存并初始化为0，返回缓冲区指针
 * @return circular_buffer_t*: 成功返回缓冲区结构体指针，失败返回NULL（内存分配失败）
 */
circular_buffer_t * create_empty_circular_buffer (void)
{
    circular_buffer_t * p_buffer_temp = NULL;
    // 1. 为循环缓冲区结构体分配内存空间
    p_buffer_temp = (circular_buffer_t *) malloc(sizeof(circular_buffer_t));
    if ( NULL == p_buffer_temp ) 
    {
        log_e("error: create_empty_circular_buffer ");
        return NULL;
    }
    
    // 2. 将缓冲区结构体所有成员初始化为0（包括head、tail指针和数据数组）
    memset ( p_buffer_temp, 0, sizeof(circular_buffer_t));
    
    return p_buffer_temp;
}

/**
 * @brief 判断循环缓冲区是否为空
 * 
 * @param[in] p_buffer : 循环缓冲区结构体指针（目标缓冲区句柄）
 * 
 * @return uint8_t : 
 *                  0xff: 错误，输入的缓冲区指针为NULL；
 *                  0x00: 缓冲区为空（head指针等于tail指针）；
 *                  0x01: 缓冲区不为空（head指针不等于tail指针）
 * 
 * @note 空缓冲区的判定条件：写指针(head)与读指针(tail)指向同一位置
 */
uint8_t buffer_is_empty (circular_buffer_t * p_buffer)
{
    // 输入参数合法性检查：缓冲区指针不能为NULL
    if ( NULL == p_buffer )
    {
        log_e("buffer*_is_NULL ");
        return 0xFF;
    }        
    // 核心判定逻辑：head == tail 表示缓冲区无数据
    if ( p_buffer->head == p_buffer->tail )
    {
        return 0x00;
    }
    else
    {
        return 0x01;
    }        
}

/**
 * @brief 判断循环缓冲区是否为满
 * 
 * @param[in] p_buffer : 循环缓冲区结构体指针（目标缓冲区句柄）
 * 
 * @return uint8_t : 
 *                  0xff: 错误，输入的缓冲区指针为NULL；
 *                  0x00: 缓冲区为满（head指针下一个位置等于tail指针）；
 *                  0x01: 缓冲区未满（head指针下一个位置不等于tail指针）
 * 
 * @note 满缓冲区的判定条件：(head % 缓冲区大小) + 1 == (tail % 缓冲区大小)
 *       预留一个位置区分"满"和"空"，避免与空缓冲区条件（head==tail）冲突
 */
uint8_t buffer_is_full (circular_buffer_t * p_buffer)
{
    // 输入参数合法性检查：缓冲区指针不能为NULL
    if ( NULL == p_buffer )
    {
        log_e("buffer*_is_NULL ");
        return 0xFF;
    }  
    // 核心判定逻辑：head的下一个位置（模缓冲区大小）等于tail位置，表示缓冲区已满
    if (
        (( (p_buffer->head)%CIRCULAR_BUFFER_SIZE) + 1) == 
        ( (p_buffer->tail)%CIRCULAR_BUFFER_SIZE)
        )
    {
        return 0x00;
    }
    else
    {
        return 0x01;
    }      
}

/**
 * @brief 向循环缓冲区插入数据
 * 
 * @param[in] p_buffer : 循环缓冲区结构体指针（目标缓冲区句柄）
 * @param[in] data     : 待插入的具体数据（数据类型为data_type_t）
 * 
 * @return uint8_t : 
 *                  0xff: 错误，输入的缓冲区指针为NULL；
 *                  0xfe: 错误，缓冲区已满，无法插入数据；
 *                  0x00: 成功，数据已插入缓冲区；
 *                  0x01: 失败（预留返回值，当前逻辑未触发）
 * 
 * @note 插入流程：1. 检查缓冲区指针合法性；2. 检查缓冲区是否满；3. 写入数据到head指向位置；4. head指针自增
 */
uint8_t insert_data (circular_buffer_t * p_buffer, data_type_t data)
{
    // 输入参数合法性检查：缓冲区指针不能为NULL
    if ( NULL == p_buffer )
    {
        log_e("insert_data");
        return 0xFF;
    }
    // 检查缓冲区是否已满，满则返回错误
    if( 0x00 == buffer_is_full(p_buffer) )
    {
        log_w("buffer_is_full ");
        return 0xFE;
    }
    
    // 将数据写入head指针当前指向的位置（通过模运算确保索引在缓冲区范围内）
    p_buffer->data[(p_buffer->head)%CIRCULAR_BUFFER_SIZE] = data;
    
    // 写指针(head)自增，准备下一次写入
    p_buffer->head++;
    
    return 0x00;
}

/**
 * @brief 从循环缓冲区读取数据
 * 
 * @param[in]  p_buffer : 循环缓冲区结构体指针（目标缓冲区句柄）
 * @param[out] data     : 读取到的数据存储地址（通过指针输出，数据类型为data_type_t）
 * 
 * @return uint8_t : 
 *                  0xff: 错误，输入的缓冲区指针为NULL；
 *                  0xfe: 错误，缓冲区为空，无数据可读取；
 *                  0x00: 成功，已从缓冲区读取数据并写入data指针；
 *                  0x01: 失败（预留返回值，当前逻辑未触发）
 * 
 * @note 读取流程：1. 检查缓冲区指针合法性；2. 检查缓冲区是否空；3. 从tail指向位置读取数据；4. tail指针自增
 * @warning 输出参数data不能为NULL，否则会导致空指针解引用
 */
uint8_t get_data (circular_buffer_t * p_buffer, data_type_t * data)
{
    // 输入参数合法性检查：缓冲区指针不能为NULL
    if ( NULL == p_buffer )
    {
        log_e("buffer* IS NULL");
        return 0xFF;
    }
    // 检查缓冲区是否为空，空则返回错误
    if( 0x00 == buffer_is_empty(p_buffer) )
    {
        log_w("get_data IS empty");
        return 0xFE;
    }
    // 从tail指针当前指向的位置读取数据（通过模运算确保索引在缓冲区范围内）
    *data = p_buffer->data[(p_buffer->tail)%CIRCULAR_BUFFER_SIZE];
    // 读指针(tail)自增，准备下一次读取
    p_buffer->tail++;
    
    return 0x00;
}


/**
 * @brief get_the_head_pos.
 * 
 *  
 * @param[in] circular_buffer_t : Pointer to the target of handler.
 * @param[in] head : Pointer to the head storage varibale.
 * 
 * @return      uint8_t : 
                        0xff:error, the buffer pointer is NULL;
                        0xfe:error, the buffer is empty;
                        0x00:success
                        0x01:failed
 * 
 * */
uint8_t get_head_pos   (circular_buffer_t * p_buffer,uint32_t * head)
{
    if ( NULL == p_buffer )
    {
        return 0xFF;
    }
    *head = p_buffer->head;
    
    return 0x00;
}
/**
 * @brief head pos. increment.
 * 
 *  
 * @param[in] circular_buffer_t : Pointer to the target of handler.
 * @param[in] increament_num : increment number.
* @return      uint8_t : 
                        0xff:error, the buffer pointer is NULL;
                        0xfe:error, the buffer is empty;
                        0x00:success
                        0x01:failed
 * 
 * */
uint8_t  head_pos_increment         (circular_buffer_t * p_buffer,\
                                            uint32_t increament_num)
{
    if ( NULL == p_buffer )
    {
        return 0xFF;
    }
    p_buffer->head += increament_num;
    
    return 0x00;
}

