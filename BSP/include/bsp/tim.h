#ifndef BSP_INCLUDE_BSP_TIM_H_
#define BSP_INCLUDE_BSP_TIM_H_

#include <tim.h>


/**
 * @brief 配置定时器, 并用给定频率初始化.
 */
void bsp_tim_conf(TIM_HandleTypeDef *tim, float freq);

/**
 * @brief 配置指定 PWM 输出通道的占空比.
 * @note 必须在配置定时器后调用.
 */
void bsp_tim_set_duty(TIM_HandleTypeDef *tim, uint32_t chn, float duty);

void bsp_tim_start(TIM_HandleTypeDef *tim);

void bsp_tim_stop(TIM_HandleTypeDef *tim);


typedef enum {
  E_TIM1_CH1,
  E_TIM1_CH3,
  E_TIM2_CH1,
  E_TIM2_CH3,
  E_TIM_PWM_END
} bsp_pwm_e;

#define PWM_SIZE ((size_t)(E_TIM_PWM_END))


/**
 * @brief 使用给定频率初始化 PWM 输出.
 */
void bsp_pwm_conf(bsp_pwm_e pwm, float freq);

/**
 * @brief 配置占空比.
 * @note 必须在初始化后调用.
 */
void bsp_pwm_set_duty(bsp_pwm_e pwm, float duty);

void bsp_pwm_start(bsp_pwm_e pwm);

void bsp_pwm_stop(bsp_pwm_e pwm);


#endif // BSP_INCLUDE_BSP_TIM_H_
