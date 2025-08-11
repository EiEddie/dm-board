#include <bsp/err.h>

#include <main.h>
#include <bsp/led.h>


void bsp_panic(const char *file, uint32_t line)
{
  UNUSED(file);
  UNUSED(line);
  bsp_led_set_hsv(LED_HSV_RED_LIGHT);
  Error_Handler();
}
