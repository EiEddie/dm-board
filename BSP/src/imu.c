#include <bsp/imu.h>

#include "bmi088/bmi088.h"


void bsp_imu_init()
{
  bmi088_init();
}


void bsp_imu_read(float acc[3], float gyro[3], float *temp)
{
  bmi088_read(acc, gyro, temp);
}
