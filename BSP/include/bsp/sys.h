#ifndef BSP_INCLUDE_BSP_SYS_H_
#define BSP_INCLUDE_BSP_SYS_H_

#include <stdint.h>
#include <stddef.h>


typedef enum {
  E_SYS_CHECK,
  E_SYS_INIT,
  E_SYS_READY,
  E_SYS_END
} bsp_sys_status_e;

#define SYS_STATUS_SIZE ((size_t)(E_SYS_END))


/**
 * @brief 设置当前任务处于 `status` 状态.
 * @warning 当任务退出状态时, 务必使用 bsp_sys_status_end
 */
void bsp_sys_status_begin(bsp_sys_status_e status);

/**
 * @brief 设置任务退出 `status` 状态.
 */
void bsp_sys_status_end(bsp_sys_status_e status);

/**
 * @brief 当 `status` 之前的任务都完成后, 返回 1;
 * 否则返回 0.
 *
 * @example 若 `status=E_SYS_READY`,
 * 则只有在所有处于 `E_SYS_CHECK`, `E_SYS_INIT` 状态的任务都退出后,
 * 才会返回 1.
 *
 * @retval 1 前置任务都已完成
 * @retval 0 前置任务未完成, 仍在运行
 */
uint8_t bsp_sys_status_ready(bsp_sys_status_e status);

#endif // BSP_INCLUDE_BSP_SYS_H_
