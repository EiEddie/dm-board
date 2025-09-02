#ifndef COMP_MATH_INCLUDE_FILTER_COMP_FILTER_H_
#define COMP_MATH_INCLUDE_FILTER_COMP_FILTER_H_

#include <stddef.h>
#include "../alg/quat.h"


struct comp_filter_t {
  quat_t q_hat;
  vec3f_t b_hat;

  float k_P;
  float k_I;
};


void comp_filter_init(struct comp_filter_t *f, float k_P, float k_I);


void comp_filter_set(struct comp_filter_t *f, float k_P, float k_I);


/**
 * @param omega 角速度
 * @param dir   归一化参考方向, 定义在全局坐标系中
 * @param mes   归一化测量方向, 对应于参考方向, 定义于局部坐标系内
 * @param k     测量方向的权重, 对应于特定测量方向.
 *              在内部计算中, k 将不被除以 2: 因为这是完全没有必要的.
 *              这一点与公式不同. 公式中只是为了保证数学上的合理
 * @param n     参考/测量方向数目
 *
 * @note 输入的方向: dir 与 mes 必须归一化, 也就是模长为 1.
 */
void comp_filter_update(struct comp_filter_t *f, vec3f_t omega,
                        const vec3f_t *dir, const vec3f_t *mes, const float *k,
                        size_t n, float dt);


/**
 * @brief 获取**轴角**表示的姿态 (弧度制)
 */
void comp_filter_get_aa(struct comp_filter_t *f, vec3f_t *pose);


/**
 * @brief 获取**欧拉角**表示的姿态 (弧度制)
 *
 * 返回的为 (roll, pitch, yaw)
 */
void comp_filter_get_el(struct comp_filter_t *f, vec3f_t *pose);

#endif // COMP_MATH_INCLUDE_FILTER_COMP_FILTER_H_
