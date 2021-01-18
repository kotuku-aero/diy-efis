#include "../neutron/neutron.h"
#include "nano/HAL/Include/nanoHAL_Time.h"

static semaphore_p wakeup;
static task_p wakeup_task;
static uint32_t next_wakeup;
static expireCallback callback;

static void wakeup_timer(void *arg)
  {
  while (1)
    {
    while (next_wakeup < ticks())
      (*callback)(arg);

    next_wakeup = INDEFINITE_WAIT;
    semaphore_wait(wakeup, next_wakeup - ticks());
    }
  }

void HAL_Time_SetCompare(uint64_t compareValueTicks, expireCallback cb, void *arg)
  {
  enter_critical();
  if (wakeup == 0)
    {
    if (wakeup == 0)
      {
      semaphore_create(&wakeup);

      next_wakeup = (uint32_t)(compareValueTicks / 10000);
      cb = callback;

      // create the timer thread
      task_create("CLR", DEFAULT_STACK_SIZE, wakeup_timer, 0, BELOW_NORMAL, &wakeup_task);
      }
    }
  else
    {
    uint32_t wakeup_at = (uint32_t)(compareValueTicks / 10000);
    // only force a wakeup if required
    if (next_wakeup > wakeup_at)
      {
      next_wakeup = wakeup_at;
      // release the task.
      semaphore_signal(wakeup);
      }
    }
  exit_critical();
  }
