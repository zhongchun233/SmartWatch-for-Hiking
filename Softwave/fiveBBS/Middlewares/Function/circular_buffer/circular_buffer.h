#ifndef __MID_CIRCULAR_BUFFER__
#define __MID_CIRCULAR_BUFFER__

#include <stdint.h>

#define CIRCULAR_BUFFER_SIZE 100

typedef uint8_t data_type_t;

typedef struct {
    data_type_t  data[CIRCULAR_BUFFER_SIZE];
    uint32_t                           head;
    uint32_t                           tail;
}circular_buffer_t;

circular_buffer_t * create_empty_circular_buffer             (void);

/**
 * @brief 判断循环缓冲区是否为空
 * @details 基于缓冲区的读指针（tail）和写指针（head）位置关系，判定缓冲区当前是否无数据
 * 
 * @param[in] p_buffer 循环缓冲区句柄指针（指向目标循环缓冲区结构体）
 * 
 * @return uint8_t 返回状态码：
 *         - 0xFF: 错误，输入的缓冲区句柄指针为 NULL；
 *         - 0x00: 成功，缓冲区为空（head 指针与 tail 指针指向同一位置）；
 *         - 0x01: 成功，缓冲区不为空（head 指针与 tail 指针位置不同）
 * 
 * @note 空缓冲区判定条件：写指针（head） == 读指针（tail）
 */
uint8_t buffer_is_empty(circular_buffer_t *p_buffer);

/**
 * @brief 判断循环缓冲区是否为满
 * @details 基于缓冲区的读指针（tail）和写指针（head）位置关系（预留一个空位置区分空/满），
 *          判定缓冲区当前是否无剩余空间
 * 
 * @param[in] p_buffer 循环缓冲区句柄指针（指向目标循环缓冲区结构体）
 * 
 * @return uint8_t 返回状态码：
 *         - 0xFF: 错误，输入的缓冲区句柄指针为 NULL；
 *         - 0x00: 成功，缓冲区为满（head 指针下一个位置等于 tail 指针）；
 *         - 0x01: 成功，缓冲区未满（head 指针下一个位置不等于 tail 指针）
 * 
 * @note 满缓冲区判定条件：(head % CIRCULAR_BUFFER_SIZE) + 1 == (tail % CIRCULAR_BUFFER_SIZE)，
 *       预留一个位置避免与空缓冲区条件（head == tail）冲突
 */
uint8_t buffer_is_full(circular_buffer_t *p_buffer);

/**
 * @brief 向循环缓冲区插入数据
 * @details 将指定数据写入缓冲区的写指针（head）当前指向位置，写入成功后更新 head 指针
 * 
 * @param[in] p_buffer 循环缓冲区句柄指针（指向目标循环缓冲区结构体）
 * @param[in] data     待插入的具体数据（数据类型为 data_type_t，由用户提前定义）
 * 
 * @return uint8_t 返回状态码：
 *         - 0xFF: 错误，缓冲区句柄指针为 NULL 或缓冲区已满；
 *         - 0x00: 成功，数据已插入缓冲区；
 *         - 0x01: 失败，数据插入未完成（预留返回值）
 * 
 * @note 插入流程：1. 检查缓冲区句柄合法性；2. 检查缓冲区是否满；3. 写入数据；4. 更新 head 指针
 */
uint8_t insert_data(circular_buffer_t *p_buffer, data_type_t data);

/**
 * @brief 从循环缓冲区读取数据
 * @details 从缓冲区的读指针（tail）当前指向位置读取数据，读取成功后更新 tail 指针，
 *          数据通过输出参数返回
 * 
 * @param[in]  p_buffer 循环缓冲区句柄指针（指向目标循环缓冲区结构体）
 * @param[out] data     读取数据的存储地址（需提前分配有效内存，不可为 NULL）
 * 
 * @return uint8_t 返回状态码：
 *         - 0xFF: 错误，缓冲区句柄指针为 NULL；
 *         - 0xFE: 错误，缓冲区为空，无数据可读取；
 *         - 0x00: 成功，已从缓冲区读取数据并写入 data 指针；
 *         - 0x01: 失败，数据读取未完成（预留返回值）
 * 
 * @warning 输出参数 data 不可为 NULL，否则会导致空指针解引用错误
 * @note 读取流程：1. 检查缓冲区句柄合法性；2. 检查缓冲区是否空；3. 读取数据；4. 更新 tail 指针
 */
uint8_t get_data(circular_buffer_t *p_buffer, data_type_t *data);
#endif // end __MID_CIRCULAR_BUFFER__