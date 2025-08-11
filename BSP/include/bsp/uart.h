#ifndef BSP_INCLUDE_BSP_UART_H_
#define BSP_INCLUDE_BSP_UART_H_

#include <stdint.h>

#include <usart.h>


#define UART_BUFFER_SIZE 1024

typedef enum {
  E_UART_3,
  E_UART_7,
  E_UART_10,
  E_UART_END
} bsp_uart_e;

#define UART_SIZE ((size_t)(E_UART_END))

typedef void (*bsp_uart_cb)(bsp_uart_e uart, uint8_t *data, uint16_t len);


/**
 * @brief 将对应的串口绑定到 `bsp`.
 *
 * 绑定后的标识符按原理图编号.
 *
 * @verbatim
 * 原理图标识  代码配置   GPIO(R/T)
 * USART3     usart1    A10/A09
 * UART7      uart7     E07/E08
 * UART10     usart10   E02/E03
 * @endverbatim
 */
void bsp_uart_bind();

/**
 * @brief 获取当前串口状态
 * @retval  0 串口已绑定
 * @retval -1 串口未绑定, 不可使用
 */
uint8_t bsp_uart_status();

void bsp_uart_send(bsp_uart_e uart, uint8_t *data, uint16_t len);

void bsp_uart_send_block(bsp_uart_e uart, uint8_t *data, uint16_t len,
                         uint32_t timeout);

void bsp_uart_printf(bsp_uart_e uart, const char *fmt, ...);

void bsp_uart_printf_block(bsp_uart_e uart, uint32_t timeout, const char *fmt,
                           ...);

void bsp_uart_set_callback(bsp_uart_e uart, bsp_uart_cb cb);

#endif // BSP_INCLUDE_BSP_UART_H_
