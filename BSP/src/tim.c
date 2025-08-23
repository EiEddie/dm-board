#include <bsp/tim.h>

#include <bsp/err.h>


#define RCC_APB1_CLOCK_FREQ_MHZ 240
#define RCC_APB2_CLOCK_FREQ_MHZ 240

#define CEIL(x, y)              (1 + ((x) - 1) / (y))


void bsp_tim_conf(TIM_HandleTypeDef *tim, float freq)
{
  // 计算定时器输入时钟 (Hz)
  uint64_t timer_clk_hz;
  if (tim->Instance == TIM1 || tim->Instance == TIM8 ||
      tim->Instance == TIM15 || tim->Instance == TIM16 ||
      tim->Instance == TIM17) {
    timer_clk_hz = (uint64_t)RCC_APB2_CLOCK_FREQ_MHZ * 1000000ULL;
  } else {
    timer_clk_hz = (uint64_t)RCC_APB1_CLOCK_FREQ_MHZ * 1000000ULL;
  }

  // 计算所需时钟周期数 ticks = timer_clk_hz / freq
  long double ticks_ld = (long double)timer_clk_hz / (long double)freq;
  if (ticks_ld < 1.0L)
    ticks_ld = 1.0L;
  uint64_t ticks = (uint64_t)(ticks_ld + 0.5L); /* 四舍五入到最近整数 */

  // 计算 period limit (ARR 最大+1):
  // 对于 32-bit 定时器, period_limit=2^32; 否则 =2^16
  uint8_t is_32bit_timer = (tim->Instance == TIM2 || tim->Instance == TIM5 ||
                            tim->Instance == TIM23 || tim->Instance == TIM24);
  uint64_t period_limit_counts = is_32bit_timer ? (1ULL << 32) : (1ULL << 16);

  uint64_t k = CEIL(ticks, period_limit_counts);
  if (k == 0)
    k = 1;

  // 填写 PSC 和 ARR
  // PSC 寄存器通常 16-bit, ARR 视定时器而定
  const uint64_t PSC_MAX = 0xFFFFULL;
  uint64_t ARR_MAX       = is_32bit_timer ? 0xFFFFFFFFULL : 0xFFFFULL;

  // 首选方案:
  // 直接用 prescaler = k-1, arr = floor(ticks / k)
  uint64_t desired_presc = k - 1;
  uint64_t presc         = desired_presc;

  uint64_t arr_counts;
  if (presc <= PSC_MAX) {
    arr_counts = ticks / (presc + 1ULL);
    if (arr_counts == 0)
      arr_counts = 1;
    // 若 arr_counts 超过 ARR_MAX, 则需要调整 presc
    if (arr_counts > ARR_MAX) {
      // 重新计算 presc 使 arr_counts <= ARR_MAX
      uint64_t presc_new = CEIL(ticks, ARR_MAX + 1ULL);
      if (presc_new == 0)
        presc_new = 1;
      presc_new = presc_new - 1ULL;
      if (presc_new > PSC_MAX) {
        // 无法用单个定时器表示: 将 presc 设为最大, 后面再尝试
        presc = PSC_MAX;
      } else {
        presc = presc_new;
      }
      arr_counts = CEIL(ticks, presc + 1ULL);
    }
  } else {
    // desired presc 已超出 PSC_MAX
    // 退化: 将 presc 设为 PSC_MAX 并计算 arr_counts
    presc      = PSC_MAX;
    arr_counts = CEIL(ticks, presc + 1ULL);
  }

  // 最后再检查 arr_counts 是否仍超出 ARR_MAX, 若超出说明无法表示
  if (arr_counts == 0)
    arr_counts = 1;
  if (arr_counts > ARR_MAX) {
    // 极端：无法在该定时器上直接表示
    // 选择一个保守降级: 把 arr_counts 设为 ARR_MAX, presc 保持 PSC_MAX
    // 但频率会偏离目标
    arr_counts = ARR_MAX;
    presc      = PSC_MAX;
    /* BSP_ASSERT(0); */
  }

  uint32_t final_prescaler = (uint32_t)presc;
  uint32_t final_period    = (uint32_t)(arr_counts - 1ULL);

  HAL_TIM_Base_Stop(tim);
  HAL_TIM_Base_DeInit(tim);

  tim->Init.Prescaler         = final_prescaler;
  tim->Init.CounterMode       = TIM_COUNTERMODE_UP;
  tim->Init.Period            = final_period;
  tim->Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  tim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

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
