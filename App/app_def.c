#include "app_def.h"


void latch_init(struct latch_t *l)
{
  if (l->inited == 1)
    return;
  taskENTER_CRITICAL();
  l->cnt   = 0;
  l->ev    = xEventGroupCreate();
  l->mutex = xSemaphoreCreateMutex();
  xEventGroupSetBits(l->ev, LATCH_BIT_DONE);
  l->inited = 1;
  taskEXIT_CRITICAL();
}


void latch_take(struct latch_t *l)
{
  xSemaphoreTake(l->mutex, portMAX_DELAY);
  l->cnt++;
  if (l->cnt == 1) {
    // 从 0 -> 1
    // 清除 done bit, 表示有未完成任务
    xEventGroupClearBits(l->ev, LATCH_BIT_DONE);
  }
  xSemaphoreGive(l->mutex);
}


void latch_give(struct latch_t *l)
{
  xSemaphoreTake(l->mutex, portMAX_DELAY);
  l->cnt--;
  if (l->cnt == 0) {
    // 所有任务完成
    // 设置 done bit, 唤醒所有等待的任务
    xEventGroupSetBits(l->ev, LATCH_BIT_DONE);
  }
  xSemaphoreGive(l->mutex);
}


void latch_wait(struct latch_t *l)
{
  xEventGroupWaitBits(l->ev, LATCH_BIT_DONE, pdFALSE, pdTRUE, portMAX_DELAY);
}
