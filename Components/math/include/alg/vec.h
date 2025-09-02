#ifndef COMP_MATH_INCLUDE_ALG_VEC_H_
#define COMP_MATH_INCLUDE_ALG_VEC_H_

#include <string.h>
#include <math.h>


typedef struct {
  float x, y, z;
} vec3f_t;

#define AS_ARR(v) ((float *)(v))


static inline void vec_cpy(vec3f_t *dst, const vec3f_t *src)
{
  memcpy(dst, src, sizeof(vec3f_t));
}


/**
 * @brief ans = v1 + v2
 */
static inline void vec_add(vec3f_t *ans, const vec3f_t v1, const vec3f_t v2)
{
  ans->x = v1.x + v2.x;
  ans->y = v1.y + v2.y;
  ans->z = v1.z + v2.z;
}


/**
 * @brief v <- k v
 */
static inline void vec_scale(vec3f_t *v, float k)
{
  v->x *= k;
  v->y *= k;
  v->z *= k;
}


/**
 * @return v1 \cdot v2
 */
static inline float vec_dot(const vec3f_t v1, const vec3f_t v2)
{
  float ans = 0.f;
  ans += v1.x * v2.x;
  ans += v1.y * v2.y;
  ans += v1.z * v2.z;
  return ans;
}


/**
 * @brief ans = v1 \times v2
 */
static inline void vec_cross(vec3f_t *ans, const vec3f_t v1, const vec3f_t v2)
{
  ans->x = v1.y * v2.z - v1.z * v2.y;
  ans->y = v1.z * v2.x - v1.x * v2.z;
  ans->z = v1.x * v2.y - v1.y * v2.x;
}


/**
 * @return |v|
 */
static inline float vec_norm(const vec3f_t v)
{
  float ans = vec_dot(v, v);
  return sqrtf(ans);
}


/**
 * @brief v <- v / |v|
 */
static inline void vec_normalize(vec3f_t *v)
{
  float n = vec_dot(*v, *v);
  n       = 1.f / sqrtf(n);
  vec_scale(v, n);
}

#endif // COMP_MATH_INCLUDE_ALG_VEC_H_
