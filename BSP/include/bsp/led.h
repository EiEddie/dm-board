#ifndef BSP_INCLUDE_BSP_LED_H_
#define BSP_INCLUDE_BSP_LED_H_

#include <stdint.h>


void bsp_led_set(uint8_t r, uint8_t g, uint8_t b);

void bsp_led_turnoff();


typedef struct {
  uint8_t r, g, b;
} rgb_t;

void bsp_led_set_rgb(rgb_t rgb);


/**
 * @param h [0, 360)
 * @param s [0, 1]
 * @param v [0, 1]
 */
typedef struct {
  float h, s, v;
} hsv_t;

void bsp_led_set_hsv(hsv_t hsv);


rgb_t bsp_led_from_hsv(hsv_t hsv);

#define LED_HSV_RED_LIGHT     ((hsv_t){0., 1., 1.})
#define LED_HSV_RED           ((hsv_t){0., 1., 0.5})
#define LED_HSV_RED_DARK      ((hsv_t){0., 1., 0.1})
#define LED_HSV_GREEN_LIGHT   ((hsv_t){120., 1., 1.})
#define LED_HSV_GREEN         ((hsv_t){120., 1., 0.5})
#define LED_HSV_GREEN_DARK    ((hsv_t){120., 1., 0.1})
#define LED_HSV_BLUE_LIGHT    ((hsv_t){240., 1., 1.})
#define LED_HSV_BLUE          ((hsv_t){240., 1., 0.5})
#define LED_HSV_BLUE_DARK     ((hsv_t){240., 1., 0.1})
#define LED_HSV_YELLOW_LIGHT  ((hsv_t){60., 1., 1.})
#define LED_HSV_YELLOW        ((hsv_t){60., 1., 0.5})
#define LED_HSV_YELLOW_DARK   ((hsv_t){60., 1., 0.1})
#define LED_HSV_CYAN_LIGHT    ((hsv_t){180., 1., 1.})
#define LED_HSV_CYAN          ((hsv_t){180., 1., 0.5})
#define LED_HSV_CYAN_DARK     ((hsv_t){180., 1., 0.1})
#define LED_HSV_FUCHSIA_LIGHT ((hsv_t){300., 1., 1.})
#define LED_HSV_FUCHSIA       ((hsv_t){300., 1., 0.5})
#define LED_HSV_FUCHSIA_DARK  ((hsv_t){300., 1., 0.1})
#define LED_HSV_GRAY          ((hsv_t){0., 0., 0.1})

#endif // BSP_INCLUDE_BSP_LED_H_
