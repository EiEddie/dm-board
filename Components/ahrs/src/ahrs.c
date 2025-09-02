#include <ahrs.h>


void ahrs_init(struct ahrs_t *ahrs, float freq, float k_P, float k_I)
{
  lowpass_filter_init(&ahrs->lpf, freq);
  comp_filter_init(&ahrs->cf, k_P, k_I);
}


void ahrs_update(struct ahrs_t *ahrs, vec3f_t omega, vec3f_t a, float dt)
{
  float delta;
  vec3f_t g_n;
  lowpass_filter_update(&ahrs->lpf, a, dt);
  lowpass_filter_get(&ahrs->lpf, &g_n, &delta);

  float k   = CHANGE_RATE_BASED_CONFIDENCE(delta);
  vec3f_t g = {0.f, 0.f, -1.f};
  vec_normalize(&g_n);
  comp_filter_update(&ahrs->cf, omega, &g, &g_n, &k, 1, dt);
}


void ahrs_get_aa(struct ahrs_t *ahrs, vec3f_t *pose)
{
  comp_filter_get_aa(&ahrs->cf, pose);
}


void ahrs_get_el(struct ahrs_t *ahrs, vec3f_t *pose)
{
  comp_filter_get_el(&ahrs->cf, pose);
}
