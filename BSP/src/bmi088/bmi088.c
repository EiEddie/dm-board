#include "bmi088.h"

#include <stm32h7xx.h>
#include <gpio.h>
#include <spi.h>

#include <bsp/time.h>

#include "bmi088_reg.h"


#define BMI088_PORT                hspi2

#define BMI088_COM_WAIT_US         150 ///< 与传感器通信等待时间
#define BMI088_LONG_DELAY_MS       80  ///< 复位后长延时

#define BMI088_WRITE_ACCEL_REG_NUM 6 ///< 初始化时读写加速度计寄存器数量
#define BMI088_WRITE_GYRO_REG_NUM  6 ///< 初始化时读写陀螺仪寄存器数量

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
 * @brief SPI 读写一个字节
 * @note 必须在拉低片选的前提下
 */
inline static uint8_t spi_rw_byte(uint8_t tx)
{
  uint8_t rx;
  HAL_SPI_TransmitReceive(&BMI088_PORT, &tx, &rx, 1, UINT32_MAX);
  return rx;
}


/**
 * @brief SPI 读多个寄存器
 * @note 必须在拉低片选的前提下
 */
inline static void spi_read_multi_reg(uint8_t reg, uint8_t *buf, uint32_t len)
{
  spi_rw_byte(reg | 0x80);
  for (; len != 0; len--) {
    *buf++ = spi_rw_byte(0x55);
  }
}


/**
 * @brief SPI 写寄存器
 * @note 必须在拉低片选的前提下
 */
inline static void spi_write_reg(uint8_t reg, uint8_t data)
{
  spi_rw_byte(reg);
  spi_rw_byte(data);
}


/// @brief 读加速度计寄存器
inline static uint8_t acc_read_reg(uint8_t reg)
{
  uint8_t val;
  ACCEL_CS_LOW();
  spi_rw_byte(reg | 0x80);
  spi_rw_byte(0x55);
  val = spi_rw_byte(0x55);
  ACCEL_CS_HIGH();
  return val;
}


/// @brief 从加速度计寄存器读多个数据
inline static void acc_read_multi_reg(uint8_t reg, uint8_t *buf, uint32_t len)
{
  ACCEL_CS_LOW();
  // spi_rw_byte(reg | 0x80);
  spi_read_multi_reg(reg, buf, len);
  ACCEL_CS_HIGH();
}


/// @brief 写加速度计寄存器
inline static void acc_write_reg(uint8_t reg, uint8_t data)
{
  ACCEL_CS_LOW();
  spi_write_reg(reg, data);
  ACCEL_CS_HIGH();
}


/// @brief 读陀螺仪寄存器
inline static uint8_t gyro_read_reg(uint8_t reg)
{
  uint8_t val;
  GYRO_CS_LOW();
  spi_rw_byte(reg | 0x80);
  val = spi_rw_byte(0x55);
  GYRO_CS_HIGH();
  return val;
}


/// @brief 从陀螺仪寄存器读多个数据
inline static void gyro_read_multi_reg(uint8_t reg, uint8_t *buf, uint32_t len)
{
  GYRO_CS_LOW();
  spi_read_multi_reg(reg, buf, len);
  GYRO_CS_HIGH();
}


/// @brief 写陀螺仪寄存器
inline static void gyro_write_reg(uint8_t reg, uint8_t data)
{
  GYRO_CS_LOW();
  spi_write_reg(reg, data);
  GYRO_CS_HIGH();
}


inline static void dwt_init()
{
  if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  }
}


void delay_us_dwt(uint32_t us)
{
  uint64_t ticks = (uint64_t)us * (SystemCoreClock / 1000000ULL);
  uint32_t start = DWT->CYCCNT;
  while ((uint32_t)(DWT->CYCCNT - start) < (uint32_t)ticks) {
    __NOP();
  }
}


typedef struct {
  uint8_t reg;
  uint8_t val;
  bmi088_err_t errno;
} bmi088_reg_cfg_t;


bmi088_err_t bmi088_accel_init()
{
  uint8_t id;

  // 通信检查
  id = acc_read_reg(BMI088_ACC_CHIP_ID);
  UNUSED(id);
  delay_us_dwt(BMI088_COM_WAIT_US);

  // 软件复位
  acc_write_reg(BMI088_ACC_SOFTRESET, BMI088_ACC_SOFTRESET_VALUE);
  bsp_time_delay_block(BMI088_LONG_DELAY_MS);

  // 再次读取 Chip ID
  id = acc_read_reg(BMI088_ACC_CHIP_ID);
  if (id != BMI088_ACC_CHIP_ID_VALUE) {
    return BMI088_NO_SENSOR;
  }

  // 配置寄存器列表
  // clang-format off
  static const bmi088_reg_cfg_t accel_cfgs[BMI088_WRITE_ACCEL_REG_NUM] = {
      {BMI088_ACC_PWR_CTRL, BMI088_ACC_ENABLE_ACC_ON,
       BMI088_ACC_PWR_CTRL_ERROR},
      {BMI088_ACC_PWR_CONF, BMI088_ACC_PWR_ACTIVE_MODE,
       BMI088_ACC_PWR_CONF_ERROR},
      {BMI088_ACC_CONF    , BMI088_ACC_NORMAL | BMI088_ACC_800_HZ | BMI088_ACC_CONF_MUST_Set,
       BMI088_ACC_CONF_ERROR},
      {BMI088_ACC_RANGE   , BMI088_ACC_RANGE_3G,
       BMI088_ACC_RANGE_ERROR},
      {BMI088_INT1_IO_CTRL, BMI088_ACC_INT1_IO_ENABLE | BMI088_ACC_INT1_GPIO_PP | BMI088_ACC_INT1_GPIO_LOW,
       BMI088_INT1_IO_CTRL_ERROR},
      {BMI088_INT_MAP_DATA, BMI088_ACC_INT1_DRDY_INTERRUPT,
       BMI088_INT_MAP_DATA_ERROR}};
  // clang-format on

  // 写入配置并验证
  for (int i = 0; i < BMI088_WRITE_ACCEL_REG_NUM; i++) {
    acc_write_reg(accel_cfgs[i].reg, accel_cfgs[i].val);
    delay_us_dwt(BMI088_COM_WAIT_US);
    uint8_t verify = acc_read_reg(accel_cfgs[i].reg);
    if (verify != accel_cfgs[i].val)
      return accel_cfgs[i].errno;
  }

  return BMI088_NO_ERROR;
}


bmi088_err_t bmi088_gyro_init()
{
  uint8_t id;

  // 通信检查
  id = gyro_read_reg(BMI088_GYRO_CHIP_ID);
  UNUSED(id);
  delay_us_dwt(BMI088_COM_WAIT_US);

  // 软件复位
  gyro_write_reg(BMI088_GYRO_SOFTRESET, BMI088_GYRO_SOFTRESET_VALUE);
  bsp_time_delay_block(BMI088_LONG_DELAY_MS);

  // 再次读取 Chip ID
  id = gyro_read_reg(BMI088_GYRO_CHIP_ID);
  if (id != BMI088_GYRO_CHIP_ID_VALUE) {
    return BMI088_NO_SENSOR;
  }

  // 配置寄存器表
  // clang-format off
  static const bmi088_reg_cfg_t gyro_cfgs[BMI088_WRITE_GYRO_REG_NUM] = {
      {BMI088_GYRO_RANGE            , BMI088_GYRO_2000,
       BMI088_GYRO_RANGE_ERROR},
      {BMI088_GYRO_BANDWIDTH        , BMI088_GYRO_1000_116_HZ | BMI088_GYRO_BANDWIDTH_MUST_Set,
       BMI088_GYRO_BANDWIDTH_ERROR},
      {BMI088_GYRO_LPM1             , BMI088_GYRO_NORMAL_MODE,
       BMI088_GYRO_LPM1_ERROR},
      {BMI088_GYRO_CTRL             , BMI088_DRDY_ON,
       BMI088_GYRO_CTRL_ERROR},
      {BMI088_GYRO_INT3_INT4_IO_CONF, BMI088_GYRO_INT3_GPIO_PP | BMI088_GYRO_INT3_GPIO_LOW,
       BMI088_GYRO_INT3_INT4_IO_CONF_ERROR},
      {BMI088_GYRO_INT3_INT4_IO_MAP , BMI088_GYRO_DRDY_IO_INT3,
       BMI088_GYRO_INT3_INT4_IO_MAP_ERROR}};
  // clang-format on

  // 写入配置并验证
  for (int i = 0; i < BMI088_WRITE_GYRO_REG_NUM; i++) {
    gyro_write_reg(gyro_cfgs[i].reg, gyro_cfgs[i].val);
    delay_us_dwt(BMI088_COM_WAIT_US);
    uint8_t verify = gyro_read_reg(gyro_cfgs[i].reg);
    if (verify != gyro_cfgs[i].val)
      return gyro_cfgs[i].errno;
  }

  return BMI088_NO_ERROR;
}


bmi088_err_t bmi088_init(void)
{
  dwt_init();

  bmi088_err_t err;
  if ((err = bmi088_accel_init()) != BMI088_NO_ERROR) {
    return err;
  }
  if ((err = bmi088_gyro_init()) != BMI088_NO_ERROR)
    return err;

  return BMI088_NO_ERROR;
}


void bmi088_read(float acc[3], float gyro[3], float *temp)
{
  uint8_t buf[8] = {0};

  acc_read_multi_reg(BMI088_ACCEL_XOUT_L, buf, 6);
  for (int i = 0; i < 3; i++) {
    int16_t raw = (int16_t)(buf[2 * i + 1] << 8 | buf[2 * i]);
    acc[i]      = (float)raw * BMI088_ACCEL_3G_SEN;
    // 矫正为右手坐标系
    // 与丝印方向一致
    acc[i] *= -1;
  }

  gyro_read_multi_reg(BMI088_GYRO_CHIP_ID, buf, 8);
  if (buf[0] == BMI088_GYRO_CHIP_ID_VALUE) {
    for (int i = 0; i < 3; i++) {
      int16_t raw = (int16_t)(buf[2 * i + 1] << 8 | buf[2 * i]);
      gyro[i]     = (float)raw * BMI088_GYRO_2000_SEN;
      // 从角度值转为弧度制
      gyro[i] *= M_PI / 180.f;
    }
  }

  acc_read_multi_reg(BMI088_TEMP_M, buf, 2);
  {
    int16_t raw = (int16_t)((buf[0] << 3) | (buf[1] >> 5));
    if (raw > 1023)
      raw -= 2048;
    *temp = (float)raw * BMI088_TEMP_FACTOR + BMI088_TEMP_OFFSET;
  }
}
