#ifndef BSP_INCLUDE_BSP_ERR_H_
#define BSP_INCLUDE_BSP_ERR_H_

#include <stdint.h>


/**
 * @brief 进入 panic 状态
 *
 * - 设置红灯常亮
 * - 进入 Error_Handler
 */
void bsp_panic(const char *file, uint32_t line);


#define BSP_ASSERT(arg)                                                        \
  do {                                                                         \
    if (!(arg))                                                                \
      bsp_panic(__FILE__, __LINE__);                                           \
  } while (0)


#endif // BSP_INCLUDE_BSP_ERR_H_
