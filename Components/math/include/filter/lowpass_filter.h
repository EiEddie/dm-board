#ifndef COMP_MATH_INCLUDE_FILTER_LOWPASS_H_
#define COMP_MATH_INCLUDE_FILTER_LOWPASS_H_

#include "../alg/quat.h"


struct lowpass_filter_t {
  vec3f_t val;
  float delta;

  float RC;
};
// TODO: 考虑改为更通用的向量形式


void lowpass_filter_init(struct lowpass_filter_t *f, float freq);


void lowpass_filter_set(struct lowpass_filter_t *f, float freq);


void lowpass_filter_update(struct lowpass_filter_t *f, vec3f_t in, float dt);


/**
 * @param val   返回滤波后的最近一次输入值
 * @param delta 返回最近一次输入相比上次的变化率:
 *              delta = |v_i-v_{i-1}| / (|v_i| + |v_{i-1}|)
 */
void lowpass_filter_get(struct lowpass_filter_t *f, vec3f_t *val, float *delta);

#endif
