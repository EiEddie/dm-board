#include <bsp/key.h>

#include <bsp/tim.h>


static bsp_key_cb _cb = NULL;


void bsp_key_init()
{
  static uint8_t inited = 0;
  if (!inited) {
    bsp_tim_conf(&KEY_DEBOUNCE_TIM, 1.f / KEY_DEBOUNCE_TIME);
    inited = 1;
  }
}


void bsp_key_set_callback(bsp_key_cb cb)
{
  _cb = cb;
}


static GPIO_PinState _key_stat = 0;
static uint32_t _passed_times  = 0;
static uint8_t _is_on_press    = 0;


void _bsp_key_exti_callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin != USER_KEY_Pin)
    return;
  if (_passed_times != 0)
    return;
  _key_stat = HAL_GPIO_ReadPin(USER_KEY_GPIO_Port, USER_KEY_Pin);
  HAL_TIM_Base_Start_IT(&KEY_DEBOUNCE_TIM);
}


void _bsp_key_tim_callback(TIM_HandleTypeDef *htim)
{
  if (htim != &KEY_DEBOUNCE_TIM)
    return;
  _passed_times += 1;
  GPIO_PinState status = HAL_GPIO_ReadPin(USER_KEY_GPIO_Port, USER_KEY_Pin);
  // 按下时间低于消抖时间 KEY_DEBOUNCE_TIME -> 无效事件
  // 按下时间超过消抖时间, 但低于长按时间 KEY_LONG_PRESS_TIMR -> 单击事件
  // 按下时间超过消抖时间和长按时间 -> 仍在按下 -> 按下事件
  //                           \-> 已经松开 -> 释放事件
  if (_passed_times == 1 && status != _key_stat) {
    // 无效事件
    goto end;
  } else if (status != _key_stat &&
             KEY_DEBOUNCE_TIME * (float)_passed_times <= KEY_LONG_PRESS_TIME) {
    // 单击事件
    _cb(E_KEY_CLICK);
    goto end;
  } else if (KEY_DEBOUNCE_TIME * (float)_passed_times > KEY_LONG_PRESS_TIME) {
    if (status == _key_stat) {
      // 按下事件
      if (!_is_on_press) {
        _is_on_press = 1;
        _cb(E_KEY_ON_PRESS);
      }
      goto next;
    } else {
      // 释放事件
      _is_on_press = 0;
      _cb(E_KEY_ON_RELEASE);
      goto end;
    }
  } else {
    // 正在按下
    goto next;
  }
end:
  _passed_times = 0;
  HAL_TIM_Base_Stop_IT(&KEY_DEBOUNCE_TIM);
  return;
next:
  HAL_TIM_Base_Start_IT(&KEY_DEBOUNCE_TIM);
  return;
}

// TODO: 使用其他方案避免中断中调用过于耗时的 _cb 函数
// 考虑使用 FreeRTOS 的 xTaskNotifyFromISR 通知机制,
// 创建一个任务来单独处理按键事件.
