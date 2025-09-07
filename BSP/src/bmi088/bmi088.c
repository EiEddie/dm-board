#include "bmi088.h"

#include <memory.h>
#include <gpio.h>
#include <spi.h>

#include "bmi088_reg.h"


#define BMI088_PORT hspi2

/**
 * @brief 将原始值转为以 g 值表示的加速度
 * @{
 */
#define BMI088_ACCEL_3G_SEN  (3.f / 32767)
#define BMI088_ACCEL_6G_SEN  (6.f / 32767)
#define BMI088_ACCEL_12G_SEN (12.f / 32767)
#define BMI088_ACCEL_24G_SEN (24.f / 32767)
/** @} */

/**
 * @brief 将原始值转为 deg/s 为单位的角速度
 * @{
 */
#define BMI088_GYRO_2000_SEN (2000.f / 32767)
#define BMI088_GYRO_1000_SEN (1000.f / 32767)
#define BMI088_GYRO_500_SEN  (500.f / 32767)
#define BMI088_GYRO_250_SEN  (250.f / 32767)
#define BMI088_GYRO_125_SEN  (125.f / 32767)
/** @} */

#define BMI088_TEMP_FACTOR 0.125f
#define BMI088_TEMP_OFFSET 23.0f


/**
 * @brief 加速度计片选操作
 * @{
 */
#define ACCEL_CS_LOW()                                                         \
  HAL_GPIO_WritePin(ACC_CS_GPIO_Port, ACC_CS_Pin, GPIO_PIN_RESET)
#define ACCEL_CS_HIGH()                                                        \
  HAL_GPIO_WritePin(ACC_CS_GPIO_Port, ACC_CS_Pin, GPIO_PIN_SET)
/** @} */


/**
 * @brief 陀螺仪片选操作
 * @{
 */
#define GYRO_CS_LOW()                                                          \
  HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET)
#define GYRO_CS_HIGH()                                                         \
  HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET)
/** @} */


/**
 * @brief SPI 写一个字节
 * @note 必须在拉低片选的前提下.
 * @note 写入后最好延迟一段时间 (>= 2us).
 */
inline static void _write_reg(uint8_t reg, uint8_t val)
{
  uint8_t tx[2];
  tx[0] = reg & 0x7f;
  tx[1] = val;
  HAL_SPI_Transmit(&BMI088_PORT, tx, 2, HAL_MAX_DELAY);
}


/// @brief 向加速度计写入数据
inline static void write_acc_reg(uint8_t reg, uint8_t val)
{
  ACCEL_CS_LOW();
  _write_reg(reg, val);
  ACCEL_CS_HIGH();
}


/// @brief 从加速度计寄存器读多个数据
inline static void read_acc_reg(uint8_t reg, uint8_t *buf, uint32_t len)
{
  uint8_t addr = reg | 0x80;
  uint8_t rx[len + 1];
  memset(rx, 0, len + 1);

  ACCEL_CS_LOW();
  HAL_SPI_Transmit(&BMI088_PORT, &addr, 1, HAL_MAX_DELAY);
  HAL_SPI_Receive(&BMI088_PORT, rx, len + 1, HAL_MAX_DELAY);
  ACCEL_CS_HIGH();

  // 丢弃第一位脏数据
  memcpy(buf, rx + 1, len);
}


/// @brief 向陀螺仪写入数据
inline static void write_gyro_reg(uint8_t reg, uint8_t val)
{
  GYRO_CS_LOW();
  _write_reg(reg, val);
  GYRO_CS_HIGH();
}


/// @brief 从陀螺仪寄存器读多个数据
inline static void read_gyro_reg(uint8_t reg, uint8_t *buf, uint32_t len)
{
  uint8_t addr = reg | 0x80;

  GYRO_CS_LOW();
  //  HAL_SPI_TransmitReceive(&BMI088_PORT, &tx, &tmp, 1, HAL_MAX_DELAY);
  //  HAL_SPI_Receive(&BMI088_PORT, buf, len, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&BMI088_PORT, &addr, 1, HAL_MAX_DELAY);
  HAL_SPI_Receive(&BMI088_PORT, buf, len, HAL_MAX_DELAY);
  GYRO_CS_HIGH();
}


void bmi088_init(void)
{
  uint8_t tmp;

  // 加速度计初始化
  // 重置加速度计参数
  write_acc_reg(BMI088_ACC_SOFTRESET, BMI088_ACC_SOFTRESET_VALUE);
  HAL_Delay(50);

  // 将加速度计转为 SPI 模式
  // 加速度计上电后默认处于 I2C 模式,
  // 通过一次 SPI 操作可以将其转为 SPI 模式
  read_acc_reg(BMI088_ACC_CHIP_ID, &tmp, 1);
  HAL_Delay(2);

  // 将加速度计置于 NORMAL 模式
  write_acc_reg(BMI088_ACC_PWR_CONF, 0x00);
  HAL_Delay(2);
  write_acc_reg(BMI088_ACC_PWR_CTRL, 0x04);
  HAL_Delay(50);

  // 显式指定加速度计量程为 +-3g
  write_acc_reg(BMI088_ACC_RANGE, BMI088_ACC_RANGE_3G);
  HAL_Delay(5);

  // 陀螺仪初始化
  write_gyro_reg(BMI088_GYRO_SOFTRESET, BMI088_GYRO_SOFTRESET_VALUE);
  HAL_Delay(30);
  // 陀螺仪上电后自动处于 SPI 模式
  // 显式指定陀螺仪量程为 +-2000deg
  write_gyro_reg(BMI088_GYRO_RANGE, BMI088_GYRO_2000);
  HAL_Delay(5);
}


void bmi088_read(float acc[3], float gyro[3], float *temp)
{
  uint8_t buf[6] = {0};

  read_acc_reg(BMI088_ACCEL_XOUT_L, buf, 6);
  for (int i = 0; i < 3; i++) {
    int16_t raw = (int16_t)(buf[2 * i + 1] << 8 | buf[2 * i]);
    acc[i]      = (float)raw * BMI088_ACCEL_3G_SEN;
    // 矫正为右手坐标系
    // 与丝印方向一致
    acc[i] *= -1;
  }

  read_gyro_reg(BMI088_GYRO_X_L, buf, 6);
  for (int i = 0; i < 3; i++) {
    int16_t raw = (int16_t)(buf[2 * i + 1] << 8 | buf[2 * i]);
    gyro[i]     = (float)raw * BMI088_GYRO_2000_SEN;
    // 从角度值转为弧度制
    gyro[i] *= M_PI / 180.f;
  }

  read_acc_reg(BMI088_TEMP_M, buf, 2);
  {
    int16_t raw = (int16_t)((buf[0] << 3) | (buf[1] >> 5));
    if (raw > 1023)
      raw -= 2048;
    *temp = (float)raw * BMI088_TEMP_FACTOR + BMI088_TEMP_OFFSET;
  }
}
