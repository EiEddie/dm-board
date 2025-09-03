#ifndef APP_APP_SYS_H_
#define APP_APP_SYS_H_

#include <FreeRTOS.h>
#include <semphr.h>
#include <event_groups.h>


/**
 * @brief 用于同步各个线程的计数器
 *
 * 当执行某任务时, 给计数器加 1; 任务完成时, 计时器减 1.
 * 当计数器不为 0 时, 阻塞所有已经完成任务的线程, 直到所有任务都完成.
 */
struct latch_t {
  uint8_t inited;

  size_t cnt;
  EventGroupHandle_t ev;
  SemaphoreHandle_t mutex;
};

#define LATCH_BIT_DONE (1U << 0)


/**
 * @note 多次调用此函数是允许的.
 *
 * 只有第一次调用才会正常初始化, 其余直接返回.
 */
void latch_init(struct latch_t *l);


/**
 * @brief 标记任务开始
 *
 * 即获取一个信号量.
 * 此时调用 `latch_wait` 的线程将会阻塞.
 *
 * @note 当任务完成后, 必须调用 `latch_give` 交还.
 */
void latch_take(struct latch_t *l);


/**
 * @brief 标记任务结束
 *
 * 即交还信号量.
 * 此时正在调用 `latch_give` 的线程将从阻塞状态释放,
 */
void latch_give(struct latch_t *l);


/**
 * @brief 等待所有任务完成
 *
 * 若所有任务都完成, 即所有获取了信号量的都将其交还, 则释放线程;
 * 否则将本线程阻塞.
 */
void latch_wait(struct latch_t *l);


#define CONCAT2(a, b) a##b
#define CONCAT(a, b)  CONCAT2(a, b)

/**
 * @brief 线程初始化区
 *
 * 在此区域内进行初始化操作, 并在结束后阻塞.
 * 在所有线程初始化完毕后, 释放线程.
 *
 * @note 必须有一个全局共享的 `latch_t`.
 *
 * @example
 * @code{.c}
 * extern latch_t latch;
 *
 * TASK_INIT(&latch) {
 *   // initialize objects in this task
 *   // do something...
 * }
 *
 * // use objects...
 * @endcode
 */
#define TASK_INIT(latch)                                                       \
  for (struct latch_t * CONCAT(_l, __LINE__) = ({                              \
         latch_init(&latch);                                                   \
         latch_take(&latch);                                                   \
         &latch;                                                               \
       });                                                                     \
       CONCAT(_l, __LINE__);                                                   \
       (latch_give(&latch), CONCAT(_l, __LINE__) = NULL, latch_wait(&latch)))

#endif // APP_APP_SYS_H_
