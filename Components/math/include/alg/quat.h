#ifndef COMP_MATH_INCLUDE_ALG_QUAT_H_
#define COMP_MATH_INCLUDE_ALG_QUAT_H_

#include <math.h>
#include "vec.h"


typedef struct {
  float re;
  vec3f_t im;
} quat_t;


/**
 * @brief ans = q1 + q2
 */
static inline void quat_add(quat_t *ans, const quat_t q1, const quat_t q2)
{
  ans->re = q1.re + q2.re;
  vec_add(&ans->im, q1.im, q2.im);
}


/**
 * @brief q <- k q
 */
static inline void quat_scale(quat_t *q, float k)
{
  q->re *= k;
  vec_scale(&q->im, k);
}


/**
 * @brief ans = q1 q2
 */
static inline void quat_mul(quat_t *ans, const quat_t q1, const quat_t q2)
{
  ans->re   = q1.re * q2.re - vec_dot(q1.im, q2.im);
  vec3f_t z = {0.f, 0.f, 0.f};
  ans->im   = z;

  vec3f_t v;
  v = q1.im;
  vec_scale(&v, q2.re);
  vec_add(&ans->im, ans->im, v);

  v = q2.im;
  vec_scale(&v, q1.re);
  vec_add(&ans->im, ans->im, v);

  vec_cross(&v, q1.im, q2.im);
  vec_add(&ans->im, ans->im, v);
}

/**
 * @brief q <- q* = [u, -v]
 */
static inline void quat_conj(quat_t *q)
{
  vec_scale(&q->im, -1.f);
}


/**
 * @brief q <- q / |q|
 */
static inline void quat_normalize(quat_t *q)
{
  float n = q->re * q->re + vec_dot(q->im, q->im);
  n       = 1. / sqrtf(n);
  quat_scale(q, n);
}


/**
 * @brief 从转角与转轴生成表示旋转的四元数
 *
 * ans = [cos(theta/2), sin(theta/2)*n]
 *
 * @warning 转轴必须经过归一化.
 */
static inline void quat_rotate(quat_t *ans, float theta, vec3f_t n)
{
  ans->re = cosf(theta / 2.f);
  ans->im = n;
  vec_scale(&ans->im, sinf(theta / 2.f));
}


/**
 * @brief 四元数旋转群作用
 *
 * 将四元数表示的旋转作用于矢量上:
 * ans = v' = q v q*
 *
 * @warning 表示旋转的四元数必须是单位四元数.
 */
static inline void quat_act(vec3f_t *ans, const quat_t q, const vec3f_t v)
{
  vec3f_t z = {0.f, 0.f, 0.f};
  *ans      = z;
  float t;
  vec3f_t u;

  u = v;
  t = q.re * q.re - vec_dot(q.im, q.im);
  vec_scale(&u, t);
  vec_add(ans, *ans, u);

  u = q.im;
  vec_scale(&u, 2.f * vec_dot(q.im, v));
  vec_add(ans, *ans, u);

  vec_cross(&u, q.im, v);
  vec_scale(&u, 2.f * q.re);
  vec_add(ans, *ans, u);
}

#endif // COMP_MATH_INCLUDE_ALG_QUAT_H_
