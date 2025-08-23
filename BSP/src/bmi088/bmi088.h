#ifndef BSP_SRC_BMI088_H_
#define BSP_SRC_BMI088_H_

#include <stdint.h>


typedef enum {
  BMI088_NO_ERROR                     = 0x00,
  BMI088_ACC_PWR_CTRL_ERROR           = 0x01,
  BMI088_ACC_PWR_CONF_ERROR           = 0x02,
  BMI088_ACC_CONF_ERROR               = 0x03,
  BMI088_ACC_SELF_TEST_ERROR          = 0x04,
  BMI088_ACC_RANGE_ERROR              = 0x05,
  BMI088_INT1_IO_CTRL_ERROR           = 0x06,
  BMI088_INT_MAP_DATA_ERROR           = 0x07,
  BMI088_GYRO_RANGE_ERROR             = 0x08,
  BMI088_GYRO_BANDWIDTH_ERROR         = 0x09,
  BMI088_GYRO_LPM1_ERROR              = 0x0A,
  BMI088_GYRO_CTRL_ERROR              = 0x0B,
  BMI088_GYRO_INT3_INT4_IO_CONF_ERROR = 0x0C,
  BMI088_GYRO_INT3_INT4_IO_MAP_ERROR  = 0x0D,
  BMI088_SELF_TEST_ACCEL_ERROR        = 0x80,
  BMI088_SELF_TEST_GYRO_ERROR         = 0x40,
  BMI088_NO_SENSOR                    = 0xFF,
} bmi088_err_t;

bmi088_err_t bmi088_init();

/**
 * @param acc  返回以 g 值为单位的加速度
 * @param gyro 返回以 s^-1 为单位, 即弧度制的角速度
 * @param temp 返回以摄氏度为单位的温度
 */
void bmi088_read(float acc[3], float gyro[3], float *temp);

#endif // BSP_SRC_BMI088_H_
