#include <bsp/uart.h>

#include <stdarg.h>
#include <stdio.h>
#include <memory.h>

#include "stm32h7xx_hal_dma.h"


static UART_HandleTypeDef *_handles[UART_SIZE] = {NULL};

static uint8_t _uart_tx_buf[UART_BUFFER_SIZE];
static uint8_t _uart_rx_buf[UART_SIZE][UART_BUFFER_SIZE];

static bsp_uart_cb _callback[UART_SIZE];


void bsp_uart_bind()
{
  _handles[E_UART_3]  = &huart1;
  _handles[E_UART_7]  = &huart7;
  _handles[E_UART_10] = &huart10;
  _bsp_uart_ready     = 1;
}


uint8_t bsp_uart_status()
{
  return _bsp_uart_ready;
}


void bsp_uart_send(bsp_uart_e uart, uint8_t *data, uint16_t len)
{
  volatile HAL_StatusTypeDef err;
  if (_handles[uart]->hdmatx) {
    HAL_UART_Transmit_DMA(_handles[uart], data, len);
  } else {
    HAL_UART_Transmit_IT(_handles[uart], data, len);
  }
}


void bsp_uart_send_block(bsp_uart_e uart, uint8_t *data, uint16_t len,
                         uint32_t timeout)
{
  if (_handles[uart]->hdmatx) {
    HAL_UART_Transmit_DMA(_handles[uart], data, len);
  } else {
    HAL_UART_Transmit(_handles[uart], data, len, timeout);
  }
}


void bsp_uart_printf(bsp_uart_e uart, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  uint16_t len = vsnprintf(_uart_tx_buf, UART_BUFFER_SIZE, fmt, ap);
  va_end(ap);
  bsp_uart_send(uart, _uart_tx_buf, len);
}


void bsp_uart_printf_block(bsp_uart_e uart, uint32_t timeout, const char *fmt,
                           ...)
{
  va_list ap;
  va_start(ap, fmt);
  uint16_t len = vsnprintf(_uart_tx_buf, UART_BUFFER_SIZE, fmt, ap);
  va_end(ap);
  bsp_uart_send_block(uart, _uart_tx_buf, len, timeout);
}


void bsp_uart_set_callback(bsp_uart_e uart, bsp_uart_cb cb)
{
  _callback[uart] = cb;
  HAL_UARTEx_ReceiveToIdle_DMA(_handles[uart], _uart_rx_buf[uart],
                               UART_BUFFER_SIZE);
  __HAL_DMA_DISABLE_IT(_handles[uart]->hdmarx, DMA_IT_HT);
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *h, uint16_t l)
{
  for (size_t i = 0; i < UART_SIZE; i++) {
    if (h == _handles[i]) {
      if (_callback[i] != NULL) _callback[i](i, _uart_rx_buf[i], l);
      memset(_uart_rx_buf[i], 0, sizeof(uint8_t) * l);
      HAL_UARTEx_ReceiveToIdle_DMA(h, _uart_rx_buf[i], UART_BUFFER_SIZE);
      __HAL_DMA_DISABLE_IT(h->hdmarx, DMA_IT_HT);
    }
  }
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *h)
{
  for (int i = 0; i < UART_SIZE; i++) {
    if (_handles[i] == NULL) continue;
    if (h == _handles[i]) {
      HAL_UARTEx_ReceiveToIdle_DMA(h, _uart_rx_buf[i], UART_BUFFER_SIZE);
      __HAL_DMA_DISABLE_IT(h->hdmarx, DMA_IT_HT);
      break;
    }
  }
}
