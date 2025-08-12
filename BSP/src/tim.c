#include <bsp/tim.h>

#include <bsp/err.h>


#define RCC_APB1_CLOCK_FREQ_MHZ 240
#define RCC_APB2_CLOCK_FREQ_MHZ 240


void bsp_tim_conf(TIM_HandleTypeDef *tim, float freq)
{
  double t = 1e6 / freq, period_limit = 0;
  if (tim->Instance == TIM1 || tim->Instance == TIM8 ||
      tim->Instance == TIM15 || tim->Instance == TIM16 ||
      tim->Instance == TIM17) {
    t *= RCC_APB2_CLOCK_FREQ_MHZ;
  } else {
    t *= RCC_APB1_CLOCK_FREQ_MHZ;
  }
  if (tim->Instance == TIM2 || tim->Instance == TIM5 ||
      tim->Instance == TIM23 || tim->Instance == TIM24) {
    period_limit = (1ll << 32);
  } else {
    period_limit = (1ll << 16);
  }
  double k = ceil(t / period_limit);

  HAL_TIM_Base_Stop(tim);
  HAL_TIM_Base_DeInit(tim);
  tim->Init.Period    = (uint32_t)(t / k) - 1;
  tim->Init.Prescaler = (uint32_t)k - 1;
  HAL_TIM_Base_Init(tim);
}


void bsp_tim_set_duty(TIM_HandleTypeDef *tim, uint32_t chn, float duty)
{
  BSP_ASSERT(0 <= duty && duty <= 1);
  __HAL_TIM_SetCompare(tim, chn, (uint32_t)((tim->Init.Period + 1) * duty));
}


void bsp_tim_start(TIM_HandleTypeDef *tim)
{
  HAL_TIM_Base_Start(tim);
}


void bsp_tim_stop(TIM_HandleTypeDef *tim)
{
  HAL_TIM_Base_Stop(tim);
}


TIM_HandleTypeDef *const PWM_TIM[] = {&htim1, &htim1, &htim2, &htim2};
const uint32_t PWM_CH[] = {TIM_CHANNEL_1, TIM_CHANNEL_3, TIM_CHANNEL_1,
                           TIM_CHANNEL_3};


void bsp_pwm_conf(bsp_pwm_e pwm, float freq)
{
  bsp_tim_conf(PWM_TIM[pwm], freq);
}


void bsp_pwm_set_duty(bsp_pwm_e pwm, float duty)
{
  bsp_tim_set_duty(PWM_TIM[pwm], PWM_CH[pwm], duty);
}


void bsp_pwm_start(bsp_pwm_e pwm)
{
  HAL_TIM_PWM_Start(PWM_TIM[pwm], PWM_CH[pwm]);
}


void bsp_pwm_stop(bsp_pwm_e pwm)
{
  HAL_TIM_PWM_Stop(PWM_TIM[pwm], PWM_CH[pwm]);
}
