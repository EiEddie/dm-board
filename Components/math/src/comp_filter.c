#include <filter/comp_filter.h>


void comp_filter_init(struct comp_filter_t *f, float k_P, float k_I)
{
  f->k_P = k_P;
  f->k_I = k_I;

  f->q_hat.re = 1.f;
  vec3f_t i   = {0.f, 0.f, 0.f};
  f->q_hat.im = i;

  vec3f_t b = {0.f, 0.f, 0.f};
  f->b_hat  = b;
}


void comp_filter_set(struct comp_filter_t *f, float k_P, float k_I)
{
  f->k_P = k_P;
  f->k_I = k_I;
}


void comp_filter_update(struct comp_filter_t *f, vec3f_t omega,
                        const vec3f_t *dir, const vec3f_t *mes, const float *k,
                        size_t n, float dt)
{
  // \omega = \sum k v \times \hat{v}
  // \hat{v} = q* v_0 q = (q*) v_0 (q*)*
  vec3f_t o = {0.f, 0.f, 0.f};
  for (size_t i = 0; i < n; i++) {
    vec3f_t a1, b1;
    quat_t q = f->q_hat;
    quat_conj(&q);
    quat_act(&a1, q, dir[i]);
    vec_cross(&b1, mes[i], a1);
    vec_scale(&b1, k[i]);
    vec_add(&o, o, b1);
  }

  // d \hat{b} = -k_I \omega * dt
  vec3f_t db = o;
  vec_scale(&db, -f->k_I * dt);
  vec_add(&f->b_hat, f->b_hat, db);

  // a2 = (\Omega - \hat{b} + k_P \omega)/2 * dt
  vec3f_t a2 = f->b_hat, b2 = o;
  vec_scale(&a2, -1.f);
  vec_scale(&b2, f->k_P);
  vec_add(&a2, a2, b2);
  vec_add(&a2, a2, omega);
  vec_scale(&a2, .5f * dt);

  // d \hat{q} = \hat{q} a2
  quat_t a3 = {0.f, a2}, dq;
  quat_mul(&dq, f->q_hat, a3);
  quat_add(&f->q_hat, f->q_hat, dq);

  // 优化旋转四元数
  // q = q / |q|
  quat_normalize(&f->q_hat);
}


void comp_filter_get_aa(struct comp_filter_t *f, vec3f_t *pose)
{
  const float ERR = 1e-5f;
  float theta     = 2.f * acosf(f->q_hat.re);

  if (fabsf(theta) > ERR && fabsf(theta - 2.f * (float)M_PI) > ERR) {
    *pose = f->q_hat.im;
    vec_normalize(pose);
    vec_scale(pose, theta);
  } else {
    vec3f_t n = {0.f, 0.f, 0.f};
    *pose     = n;
  }
}


void comp_filter_get_el(struct comp_filter_t *f, vec3f_t *pose)
{
  float qw = f->q_hat.re;
  float qx = f->q_hat.im.x, qy = f->q_hat.im.y, qz = f->q_hat.im.z;

  // roll
  float a1 = 2.f * (qw * qx + qy * qz);
  float b1 = 1.f - 2.f * (qx * qx + qy * qy);
  pose->x  = atan2f(a1, b1);

  // pitch
  float a2 = 2.f * (qw * qy - qz * qx);
  if (a2 > 1.f)
    a2 = 1.f;
  if (a2 < -1.f)
    a2 = -1.f;
  pose->y = asinf(a2);

  // yaw
  float a3 = 2.f * (qw * qz + qx * qy);
  float b3 = 1.f - 2.f * (qy * qy + qz * qz);
  pose->z  = atan2f(a3, b3);
}
