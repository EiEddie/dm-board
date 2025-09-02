#include <filter/lowpass_filter.h>


void lowpass_filter_init(struct lowpass_filter_t *f, float freq)
{
  vec3f_t v = {0.f, 0.f, 0.f};
  f->val    = v;
  f->delta  = 0.f;
  f->RC     = 1.f / (float)(2.f * M_PI * freq);
}


void lowpass_filter_set(struct lowpass_filter_t *f, float freq)
{
  f->RC = 1.f / (float)(2.f * M_PI * freq);
}


void lowpass_filter_update(struct lowpass_filter_t *f, vec3f_t in, float dt)
{
  // \delta = |in-val| / (|in| + |val|)
  //        = |val-in| / (|in| + |val|)
  float n = vec_norm(in);
  vec_scale(&in, -1.f);
  vec_add(&in, in, f->val);
  f->delta = vec_norm(in) / (vec_norm(f->val) + n);

  // \alpha = dt / (RC + dt)
  // val = \alpha*in + (1-\alpha)*val
  //     = -\alpha*(val-in) + val
  float alpha = dt / (f->RC + dt);
  vec_scale(&in, -alpha);
  vec_add(&f->val, f->val, in);
}


void lowpass_filter_get(struct lowpass_filter_t *f, vec3f_t *val, float *delta)
{
  *val   = f->val;
  *delta = f->delta;
}
