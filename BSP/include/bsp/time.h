#ifndef BSP_INCLUDE_BSP_TIME_H_
#define BSP_INCLUDE_BSP_TIME_H_

#include <main.h>
#include <FreeRTOS.h>
#include <task.h>


inline static uint32_t bsp_time_get_ms()
{
  return xTaskGetTickCount();
}


inline static uint64_t bsp_time_get_us()
{
  uint32_t ms_old       = xTaskGetTickCount();
  uint32_t tick_val_old = SysTick->VAL;
  uint32_t ms_now       = xTaskGetTickCount();
  uint32_t tick_val_now = SysTick->VAL;
  return ms_old == ms_now
             ? ms_now * 1000 + 1000 - tick_val_old * 1000 / (SysTick->LOAD + 1)
             : ms_now * 1000 + 1000 - tick_val_now * 1000 / (SysTick->LOAD + 1);
}


/**
 * @brief 在 FreeRTOS 调度的任务中延迟指定毫秒
 * @noting 只能在 FreeRTOS 调度的任务中使用
 */
inline static void bsp_time_delay(uint32_t ms)
{
  vTaskDelay(ms / portTICK_PERIOD_MS);
}


/**
 * @brief 阻塞时延迟指定毫秒
 */
inline static void bsp_time_delay_block(uint32_t ms)
{
  HAL_Delay(ms);
}

#endif // BSP_INCLUDE_BSP_TIME_H_
