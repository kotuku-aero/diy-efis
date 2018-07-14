#include "microkernel.h"
#include <stdlib.h>
#include <string.h>

extern bool scheduler_enabled;

result_t has_wait_tasks(semaphore_p semp)
  {
  return semp->tasks.head != 0 ? s_ok : s_false;
  }

result_t semaphore_signal(semaphore_p hndl)
  {
  enter_critical();
  signal_from_isr(hndl);
  
  exit_critical();

  if(scheduler_enabled)
    yield();
  
  return s_ok;
  }

result_t semaphore_create(semaphore_p *hndl)
  {
  semaphore_t *semp = (semaphore_t *)neutron_malloc(sizeof(semaphore_t));
  memset(semp, 0, sizeof(semaphore_t));
  
  *hndl = semp;
  return s_ok;
  }

result_t semaphore_close(semaphore_p hndl)
  {
  // todo: fix this...
  /*
  if(semp->tasks.tail != 0)
    {
    volatile tcb_t *task = semp->tasks.tail;
    semp->count--;
  
    unblock_tcb(task);
   }
   */
  
  neutron_free(hndl);

  return s_ok;
  }

result_t signal_from_isr(semaphore_p semaphore)
  {
  if(semaphore == 0)
    return e_bad_parameter;
  
  semaphore->count++;
  
  if(semaphore->tasks.tail != 0)
  {
    volatile tcb_t *task = semaphore->tasks.tail;
    semaphore->count--;

    // free the oldest task.
    unblock_tcb(task);
   }

  return s_ok;
  }

result_t semaphore_wait(semaphore_p semaphore, uint32_t delay)
  {
  if(semaphore == 0)
    return e_bad_parameter;
  
  enter_critical();
  // special case for un-blocked semaphore
  if(semaphore->tasks.tail == 0 && semaphore->count > 0)
  {
    semaphore->count--;
    exit_critical();
    return s_ok;
  }

  current_task->state = ts_blocked;
  current_task->delay = delay;
  current_task->waiting_on = &semaphore->tasks;

  if(semaphore->tasks.head != 0)
  {
    semaphore->tasks.head->prev = (tcb_t *)current_task;
    current_task->next = semaphore->tasks.head;
    }
  
  semaphore->tasks.head = (tcb_t *)current_task;
  
  if(semaphore->tasks.tail == 0)
    semaphore->tasks.tail = (tcb_t *)current_task;
  
  // never call from an interrupt...
  exit_critical();
  
  if(scheduler_enabled)
    yield();
  
  return s_ok;
  }
