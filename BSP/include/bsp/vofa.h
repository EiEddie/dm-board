#ifndef BSP_INCLUDE_BSP_VOFA_H_
#define BSP_INCLUDE_BSP_VOFA_H_

#include <stdint.h>
#include <memory.h>

#include <bsp/err.h>
#include <bsp/uart.h>


/**
 * @brief 使用 vofa+ 的 `JustFloat` 协议发送数据.
 */
inline static void bsp_vofa_send_f32(bsp_uart_e uart, const float *data,
                                     uint16_t len)
{
#define CH_MAX 128
  BSP_ASSERT(len <= CH_MAX);

  // clang-format off
  __attribute__((section(".ram_d1"))) \
  static uint8_t buf[(CH_MAX + 1) * sizeof(float)];
  // clang-format on
  uint32_t bytes = len * sizeof(float);
  memcpy(buf, data, bytes);

  uint8_t tail[] = {0x00, 0x00, 0x80, 0x7f};
  memcpy(buf + bytes, tail, 4);

  bsp_uart_send(uart, (uint8_t *)buf, bytes + 4);
}

#endif // BSP_INCLUDE_BSP_VOFA_H_
