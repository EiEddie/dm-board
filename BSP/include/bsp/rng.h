#ifndef BSP_INCLUDE_BSP_RNG_H_
#define BSP_INCLUDE_BSP_RNG_H_

#include <stdint.h>

#include <rng.h>


inline static uint32_t bsp_rng_rand()
{
  uint32_t rn;
  HAL_RNG_GenerateRandomNumber(&hrng, &rn);
  return rn;
}

#endif // BSP_INCLUDE_BSP_RNG_H_
