#include "microkernel.h"
#include "memory.h"
#include <stdlib.h>
#include <string.h>

// This is the task that is running. exposed so we can add a watch onto it.
volatile tcb_t *current_task = 0;
// this is the list of all tasks.  should always at least have the
// idle task
volatile tcb_t *task_list = 0;

// disable scheduler till requested to run by system.
bool scheduler_enabled = false;

/**
 * Notify a new task is scheduled
 * @param next_task
 */
extern void set_next_task(volatile tcb_t *next_task);

task_p get_current_task()
  {
  return (task_p) current_task;
  }

#ifdef _DEBUG
extern void stkchk();
#endif

volatile tcb_t *schedule()
  {
#ifdef _DEBUG
  stkchk();
#endif
  
  volatile tcb_t *next_task = 0;         // next task candidate
  // find the highest priority ready to run task.
  volatile tcb_t *task = current_task;
  do
    {
    // this walks the task list to the end.
    if(task != 0)
      task = task->next_task;
    
    // case when the end of the task list is found
    if(task == 0)
      task = task_list;         // start at beginning again

    if(task->state == ts_running)
      {
      // found a ready task.
      if(next_task == 0 || task->priority > next_task->priority)
        {
        next_task = task;    // first ready task so use it.
        // if a higher priority task is found it will be used
        if(current_task == 0)
          current_task = next_task;
        }
      }
    } while(task != current_task);

    // if we have not found a ready to run task so we need to
    // wait for an interrupt
    if(next_task == 0)
      panic();

    current_task = next_task;
    
    set_next_task(next_task);
    return next_task;
  }

  /*
task_callback tasks[max_tasks];
static void *args[max_tasks];
static event_mask_t event_masks[max_tasks];
int running_task = 0;
*/

result_t set_task_priority(task_p hndl, uint8_t priority)
  {
  tcb_t *task = (tcb_t *)hndl;

  task->priority = priority;

  return s_ok;
  }

result_t get_task_priority(task_p hndl, uint8_t *p)
  {
  if(p == 0 || hndl == 0)
    return e_bad_parameter;

  tcb_t *task = (tcb_t *)hndl;
  *p = task->priority;

  return s_ok;
  }

////////////////////////////////////////////////////////////////////////////////
//
// Machine specific stuff...
//
// NOTE: timer_tick is never 0, starts at 1
static uint32_t timer_tick = 1;

result_t task_create(const char *name,
                    uint16_t stack_size,
                    task_callback callback,
                    void *parg,
                    uint8_t priority,
                     task_p *hndl)
  {
  if(callback == 0)
    return e_bad_parameter;
  
  enter_critical();
  if(stack_size == 0)
    stack_size = DEFAULT_STACK_SIZE;
  
  volatile tcb_t *task = create_tcb(stack_size, callback, parg);
  
  strncpy((char *)task->task_name, name, 16);
  task->priority = priority;
  task->state = ts_suspended;

  //add_task(task_list, task);
  if(task_list != 0)
    {
    task->next_task = task_list;
    task_list->prev_task = task;
  }

  task_list = task;
  exit_critical();
  
  if(hndl != 0)
    *hndl = (void *)task;

  return task_resume((void *)task);
  }

void unblock_tcb(volatile tcb_t *task)
    {
  if(task->state == ts_blocked)
    task->state = ts_running;   // task can run

  if(task->waiting_on == 0)
    return;
  
  if(task->waiting_on->head == task)
    task->waiting_on->head = task->next;

  if(task->waiting_on->tail == task)
    task->waiting_on->tail = task->prev;

  if(task->prev != 0)
    task->prev->next = task->next;

  if(task->next != 0)
    task->next->prev = task->prev;
  
  task->next = 0;
  task->prev = 0;
  task->waiting_on = 0;
  }

result_t close_task(task_p hndl)
  {
  if(hndl == 0)
    return e_bad_parameter;
  
  volatile tcb_t *task = (volatile tcb_t *)hndl;
  
  enter_critical();
  
  if(task->next_task != 0)
    task->next_task->prev_task = task->prev_task;

  if(task->prev_task != 0)
    task->prev_task->next_task = task->next_task;
  
  task->next_task = 0;
  task->prev_task = 0;
  
  unblock_tcb(task);
  
  release_tcb(task);
  
  exit_critical();
  
  return s_ok;
  }

void unblock(task_p hndl)
  {
  unblock_tcb((volatile tcb_t *)hndl);
  }

result_t task_resume(task_p hndl)
  {
  if(hndl == 0)
    return e_bad_parameter;
  
  volatile tcb_t *task = (volatile tcb_t *)hndl;
  
  
  if(task->state == ts_suspended)
    task->state = ts_running;
  
  return s_ok;
  }

static void timer_cb(uint32_t timer_tick)
  {
  volatile tcb_t *task = task_list;
  // look for any tasks that are waiting on an event that can time-out
  while(task != 0)
    {
    if(task->state == ts_blocked && task->delay < INDEFINITE_WAIT)
      {
      // check to see if the task was un-lock at the last tick.
      if(task->delay == 0)
        unblock_tcb(task);
      else
        task->delay--;
      }
    
    task = task->next_task;
    }
  }

static timer_hook_t scheduler_hook = { 0, 0, timer_cb };
static timer_hook_t *hook_chain = &scheduler_hook;

result_t timer_hook(timer_hook_t *hook)
  {
  if(hook == 0 || hook->callback == 0)
    return e_bad_parameter;
  
  enter_critical();
  
  hook->prev = hook_chain;
  
  if(hook->prev != 0)
    hook->prev->next = hook;
  
  hook->next = 0;
  
  hook_chain = hook;
  
  exit_critical();
  return s_ok;
  }

result_t timer_unhook(timer_hook_t *hook)
  {
  if(hook == 0)
    return e_bad_parameter;
  
  enter_critical();
  
  if(hook_chain == hook)
    hook_chain = hook->prev;
  
  if(hook->next != 0)
    hook->next->prev = hook->prev;
  
  if(hook->prev != 0)
    hook->prev->next = hook->next;
  
  hook->prev = 0;
  hook->next = 0;
  
  exit_critical();
  return s_ok;
  }

void timer_interrupt()
  {
  timer_tick++;
  
  timer_hook_t *hook = hook_chain;
  
  while(hook != 0)
    {
    (*hook->callback)(hook, timer_tick);
    hook = hook->prev;
    }
  }

uint32_t ticks()
  {
  return timer_tick;
  }

