#include <bsp/led.h>

#include <spi.h>


#define WS2812_HIGH 0xf0
#define WS2812_LOW  0xc0
#define WS2812_PORT &hspi6


static uint8_t buf[24];


void bsp_led_set(uint8_t r, uint8_t g, uint8_t b)
{
  for (int i = 0; i < 8; i++) {
    buf[7 - i]  = (((g >> i) & 1) ? WS2812_HIGH : WS2812_LOW) >> 1;
    buf[15 - i] = (((r >> i) & 1) ? WS2812_HIGH : WS2812_LOW) >> 1;
    buf[23 - i] = (((b >> i) & 1) ? WS2812_HIGH : WS2812_LOW) >> 1;
  }
  HAL_SPI_Transmit(WS2812_PORT, buf, 24, HAL_MAX_DELAY);
}

void bsp_led_turnoff()
{
  bsp_led_set(0, 0, 0);
}

void bsp_led_set_rgb(rgb_t rgb)
{
  bsp_led_set(rgb.r, rgb.g, rgb.b);
}

void bsp_led_set_hsv(hsv_t hsv)
{
  rgb_t rgb = bsp_led_from_hsv(hsv);
  bsp_led_set_rgb(rgb);
}

rgb_t bsp_led_from_hsv(hsv_t hsv)
{
  float c   = hsv.v * hsv.s;
  float x   = (1.f - fabsf(fmodf(hsv.h / 60.f, 2.f) - 1.f)) * c;
  float m   = hsv.v - c;
  uint8_t M = (uint8_t)(m * 255.f);

  uint8_t rgb[3] = {M, M, M};
#define MOD(x, y) (((x) % (y) + (y)) % (y))
  rgb[MOD(1 - (int)(hsv.h / 60), 3)] += (uint8_t)(x * 255.f);
  rgb[MOD((int)((hsv.h + 60) / 120), 3)] += (uint8_t)(c * 255.f);
  return *(rgb_t *)rgb;
}
