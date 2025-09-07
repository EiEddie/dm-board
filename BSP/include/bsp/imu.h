#ifndef BSP_INCLUDE_BSP_IMU_H_
#define BSP_INCLUDE_BSP_IMU_H_


void bsp_imu_init();


/**
 * @param acc  返回以 g 值为单位的加速度
 * @param gyro 返回以 s^-1 为单位, 即弧度制的角速度
 * @param temp 返回以摄氏度为单位的温度
 */
void bsp_imu_read(float acc[3], float gyro[3], float *temp);

#endif // BSP_INCLUDE_BSP_IMU_H_
