#ifndef BSP_INCLUDE_BSP_KEY_H_
#define BSP_INCLUDE_BSP_KEY_H_

#include <stdint.h>
#include <tim.h>


typedef enum {
  E_KEY_CLICK,
  E_KEY_ON_PRESS,
  E_KEY_ON_RELEASE,
} bsp_key_event_e;

typedef void (*bsp_key_cb)(bsp_key_event_e);

/**
 * @brief 消抖使用的定时器
 */
#define KEY_DEBOUNCE_TIM htim6

/**
 * @brief 消抖时间
 */
#define KEY_DEBOUNCE_TIME (float)0.01/*s*/

/**
 * @brief 长按时间
 *
 * 按下按钮时间超过此值时被认为是长按
 */
#define KEY_LONG_PRESS_TIME (float)0.5/*s*/


/**
 * @brief 初始化按键 PA15.
 */
void bsp_key_init();

void bsp_key_set_callback(bsp_key_cb cb);

/**
 * @note 在外部中断中使用, 用于处理按键事件.
 * @warning 不要手动调用此函数.
 */
void _bsp_key_exti_callback(uint16_t);

/**
 * @note 在定时器中断中使用, 用于处理按键事件.
 * @warning 不要手动调用此函数.
 */
void _bsp_key_tim_callback(TIM_HandleTypeDef *);

#endif // BSP_INCLUDE_BSP_KEY_H_
