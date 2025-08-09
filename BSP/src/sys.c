#include <bsp/sys.h>

#include <bsp/err.h>


static int16_t _status_cnt[SYS_STATUS_SIZE] = {0};


void bsp_sys_status_begin(bsp_sys_status_e status)
{
  _status_cnt[status]++;
}


void bsp_sys_status_end(bsp_sys_status_e status)
{
  _status_cnt[status]--;
  BSP_ASSERT(_status_cnt[status] >= 0);
}


uint8_t bsp_sys_status_ready(bsp_sys_status_e status)
{
  for (size_t i = 0; i < status; i++) {
    if (_status_cnt[i])
      return 0;
  }
  return 1;
}
