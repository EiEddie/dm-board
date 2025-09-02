#ifndef COMP_AHRS_INCLUDE_AHRS_H_
#define COMP_AHRS_INCLUDE_AHRS_H_

#include <math.h>

#include <comp/math/alg/vec.h>
#include <comp/math/filter/lowpass_filter.h>
#include <comp/math/filter/comp_filter.h>


/**
 * @brief 根据参考方向的变化率 |\\Delta a| / |a| 计算此参考方向的置信度
 *
 * 一般来说, 变化越大, 越不可信, 置信度越低, 因此函数应该单调递减.
 * 使用逻辑斯蒂 (Logistic) 函数实现.
 *
 * 可以微调参数 _k.
 */
#define CHANGE_RATE_BASED_CONFIDENCE(rate)                                     \
  ({                                                                           \
    const float _k = 15.f;                                                     \
    1.f / (1.f + expf(_k * ((rate) - .5f)));                                   \
  })


struct ahrs_t {
  struct lowpass_filter_t lpf;
  struct comp_filter_t cf;
};


/**
 * @brief 初始化一个航姿参考系统
 *
 * 用于估计系统的三维姿态.
 * 使用 Mahony 互补滤波算法.
 *
 * @param freq 滤除高频加速度分量的截止频率. 高于此值的会被剔除.
 * @param k_P  Mahony 互补滤波算法所需的反馈参数 k_P
 * @param k_I  同上
 */
void ahrs_init(struct ahrs_t *ahrs, float freq, float k_P, float k_I);


/**
 * @brief 使用角速度与一个参考方向更新系统
 * @param omega 弧度制角速度 [s^-1]
 * @param a     重力方向, 不需归一化, 单位不限
 * @param dt    自上次更新以来经过的时间
 * @note 必须确保重力方向在角速度参考系中指向下方.
 */
void ahrs_update(struct ahrs_t *ahrs, vec3f_t omega, vec3f_t a, float dt);


/**
 * @brief 获取**轴角**表示的姿态 (弧度制)
 */
void ahrs_get_aa(struct ahrs_t *ahrs, vec3f_t *pose);


/**
 * @brief 获取**欧拉角**表示的姿态 (弧度制)
 *
 * 返回的为 (roll, pitch, yaw)
 */
void ahrs_get_el(struct ahrs_t *ahrs, vec3f_t *pose);

#endif // COMP_AHRS_INCLUDE_AHRS_H_
