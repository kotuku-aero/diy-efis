/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include <p33Exxxx.h>
#include "microkernel.h"
#include <stdlib.h>
#include <string.h>

static int8_t next_task = 0;
static tcb_t *scheduler_tasks;
static int8_t last_task = 0;
static int8_t max_tasks;
// disable scheduler till requested to run by system.
static bool scheduler_enabled = false;

int8_t scheduler_init(tcb_t *tcbs,
                      int8_t _max_tasks,
                      task_callback idle_proc,
                      uint16_t *stack,
                      uint16_t stack_size)
  {
  // disable nested interrupts
  INTCON1bits.NSTDIS = 1;
  
  scheduler_tasks = tcbs;
  max_tasks = _max_tasks;
  memset(tcbs, 0, sizeof(tcb_t) * _max_tasks);
  
  if(idle_proc != 0)
    return create_task("IDLE", stack, stack_size, idle_proc, 0, IDLE_PRIORITY);
  
  return -1;
  }
  
static tcb_t *schedule()
  {
  uint16_t count;
  uint8_t id = last_task + 1; // always let next task run
  uint8_t next_task = 0;
  // find the highest priority ready to run task.
  tcb_t *task = 0;
  for(count = max_tasks; count > 0; count--)
    {
    if(id >= max_tasks)
      id = 0;         // loop around
    
    if(scheduler_tasks[id].state == ts_running)
      {
      // found a ready task.
      if(task == 0 || scheduler_tasks[id].priority > task->priority)
        {
        task = scheduler_tasks + id;    // first ready task so use it.
        next_task = id;
        }
      }
    
    id++;
    }
  
  last_task = next_task;
  
  return task;
  }

volatile tcb_t *current_task = 0;

  /*
task_callback tasks[max_tasks];
static void *args[max_tasks];
static event_mask_t event_masks[max_tasks];
int running_task = 0;
*/

void set_task_priority(int8_t task, uint8_t priority)
  {
  if(task >= max_tasks || task < 0)
    return;
  
  scheduler_tasks[task].priority = priority;
  }

uint8_t get_task_priority(int8_t task)
  {
  if(task >= max_tasks || task < 0)
    return 0;
  
  return scheduler_tasks[task].priority;
  }

// NOTE: timer_tick is never 0, starts at 1
static uint32_t timer_tick = 1;
static const uint16_t initial_stack[] =
  {
  0x1111, /* W1 */
  0x2222, /* W2 */
  0x3333, /* W3 */
  0x4444, /* W4 */
  0x5555, /* W5 */
  0x6666, /* W6 */
  0x7777, /* W7 */
  0x8888, /* W8 */
  0x9999, /* W9 */
  0xaaaa, /* W10 */
  0xbbbb, /* W11 */
  0xcccc, /* W12 */
  0xdddd, /* W13 */
  0xcdce, /* RCOUNT */
  0xabac, /* TBLPAG */
  0x0202, /* ACCAL */
  0x0303, /* ACCAH */
  0x0404, /* ACCAU */
  0x0505, /* ACCBL */
  0x0606, /* ACCBH */
  0x0707, /* ACCBU */
  };

static void task_abort();

int8_t create_task(const char *name,
                    uint16_t *stack,
                    uint16_t stack_size,
                    task_callback callback,
                    void *parg,
                    uint8_t priority)
  {
  uint16_t *top_of_stack;
  uint16_t value;
  int i;
  
  int8_t task_num = next_task;
  if(task_num >= max_tasks)
    return -1;
  
  strncpy(scheduler_tasks[task_num].task_name, name, 16);
  // allocate a stack
  scheduler_tasks[task_num].stack = stack;
  top_of_stack = scheduler_tasks[task_num].stack;
  scheduler_tasks[task_num].stack_limit = stack + stack_size;
  
  // we provide a call to task_abort in case the thread exits
  value = (uint16_t) task_abort;
  *top_of_stack++ = value;
  *top_of_stack++ = 0;
  // fp
  *top_of_stack++ = (uint16_t) scheduler_tasks[task_num].stack;
  
  // Save the low bytes of the program counter.
  value = (uint16_t) callback;
  *top_of_stack++ = value;

  // Save the high byte of the program counter.  This will always be zero
  // here as it is passed in a 16bit pointer.  If the address is greater than
  // 16 bits then the pointer will point to a jump table.
  *top_of_stack++ = 0;
 
  // Set old FP on stack
  *top_of_stack++ = (uint16_t)(top_of_stack - 2);
 
  // Status register with interrupts enabled.
  *top_of_stack++ = 0;
  
  // w14 on stack which is not in the initial args
  *top_of_stack++ = (uint16_t)(top_of_stack -1);

  /* Parameters are passed in W0. */
  *top_of_stack++ = (uint16_t) parg;

  for (i = 0; i < (sizeof ( initial_stack) / sizeof ( uint16_t)); i++)
    *top_of_stack++ = initial_stack[ i ];

  *top_of_stack++ = CORCON;
  *top_of_stack++ = DSRPAG;
  *top_of_stack++ = DSWPAG;

  scheduler_tasks[task_num].stack_top = top_of_stack;
  scheduler_tasks[task_num].priority = priority;
  scheduler_tasks[task_num].state = ts_suspended;

  next_task++;

  return task_num;
  }

static void unlink_task(tcb_t *task)
  {
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

static void unblock_tcb(tcb_t *task)
  {
  if(task->state == ts_blocked)
    task->state = ts_running;   // task can run

  unlink_task(task);          // no longer blocked
  }

void unblock(int8_t task)
  {
  if(task < 0 || task >= max_tasks)
    return;
  
  unblock_tcb(scheduler_tasks + task);
  }

void resume(int8_t task)
  {
  if(task < 0 || task >= max_tasks)
    return;
  
  if(scheduler_tasks[task].state == ts_suspended)
    scheduler_tasks[task].state = ts_running;
  }

// called from the interrupt routine when the publish tick is made
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
  {
  int8_t i;
  IFS0bits.T1IF = 0;

  timer_tick++;
  
  // look for any tasks that are waiting on an event that can time-out
  for(i = 0; i < max_tasks; i++)
    {
    tcb_t *task = scheduler_tasks + i;
    if(task->state == ts_blocked && task->delay < INDEFINITE_WAIT)
      {
      // check to see if the task was un-lock at the last tick.
      if(task->delay == 0)
        {
        if(task->waiting_on != 0)
          {
          
          }
          
        unblock_tcb(task);
        }
      else
        task->delay--;
      }
    }
  }

uint32_t ticks()
  {
  return timer_tick;
  }

static void dispatch_task()
  {
  // disable any context swap...
  __builtin_disi(0x3FFF);
  // find the first ready to run task (could be the idle task)
  current_task = schedule();
  
  /* Restore the context of the first task to run. */
  asm volatile("MOV #0x4FFE, W0");
  asm volatile("MOV W0, SPLIM");
  asm volatile("MOV	_current_task, W0");
  asm volatile("MOV	[W0], W15");
  asm volatile("MOV [W0+2], W0");
  asm volatile("MOV W0, SPLIM");
  asm volatile("POP	DSWPAG");
  asm volatile("POP	DSRPAG");
  asm volatile("POP	CORCON");
  asm volatile("POP	ACCBU");
  asm volatile("POP	ACCBH");
  asm volatile("POP	ACCBL");
  asm volatile("POP	ACCAU");
  asm volatile("POP	ACCAH");
  asm volatile("POP	ACCAL");
  asm volatile("POP	TBLPAG");
  asm volatile("POP	RCOUNT");
  asm volatile("POP.D	W12");
  asm volatile("POP.D	W10");
  asm volatile("POP.D	W8");
  asm volatile("POP.D	W6");
  asm volatile("POP.D	W4");
  asm volatile("POP.D	W2");
  asm volatile("POP.D	W0");
  asm volatile("POP	W14");
  asm volatile("POP	SR");
  }

void run(int8_t idle_task)
  {
  // create the idle task, and start it running
  resume(idle_task);
  
  // set up TMR1 as our kernel timer.
  // start the timer running  We need a 90 msec interval
  // Fp = 70mhz, 
  PR1 = 8750;                         // 1 msec timer.
  TMR1 = 0;
  T1CONbits.TCKPS =1;                 // clk / 8 or 0.4 usec clock to counter
  T1CONbits.TCS = 0;                  // select clock source to free-run
  T1CONbits.TGATE = 0;
  T1CONbits.TON = 1;

  IFS0bits.T1IF = 0;
  IEC0bits.T1IE = 1;
  
  scheduler_enabled = true;
  

  dispatch_task();
  }

void signal(semaphore_t *semaphore)
  {
  enter_critical();
  signal_from_isr(semaphore);
  exit_critical();
  
  yield();
  }

void signal_from_isr(semaphore_t *semaphore)
  {
  semaphore->count++;
  
  if(semaphore->tasks.tail != 0)
    {
    tcb_t *task = semaphore->tasks.tail;
    semaphore->count--;

    unblock_tcb(task);
   }
  }

void wait(semaphore_t *semaphore, uint32_t delay)
  {
  enter_critical();
  // special case for un-blocked semaphore
  if(semaphore->tasks.tail == 0 && semaphore->count > 0)
    {
    semaphore->count--;
    exit_critical();
    return;
    }
  
  current_task->state = ts_blocked;
  current_task->delay = delay;
  current_task->waiting_on = &semaphore->tasks;
  
  if(semaphore->tasks.head != 0)
    {
    semaphore->tasks.head->prev = current_task;
    current_task->next = semaphore->tasks.head;
    }
  
  semaphore->tasks.head = current_task;
  
  if(semaphore->tasks.tail == 0)
    semaphore->tasks.tail = current_task;
  
  exit_critical();
  
  // never call from an interrupt...
  yield();
  }

static void task_abort()
  {
  // seruous problem here
  enter_critical();
  current_task->state = ts_aborted;
  dispatch_task();
  
  while(1);
  }

void yield()
  {
  if(!scheduler_enabled)
    return;

	asm volatile("PUSH	SR");	// Save the SR used by the task....
  enter_critical();   // ....then disable interrupts.
	asm volatile("PUSH	W14");
  asm volatile("PUSH.D	W0"); // save all registers
  asm volatile("PUSH.D	W2");
	asm volatile("PUSH.D	W4");
	asm volatile("PUSH.D	W6");
	asm volatile("PUSH.D 	W8");
	asm volatile("PUSH.D 	W10");
	asm volatile("PUSH.D	W12");
	asm volatile("PUSH	RCOUNT");
	asm volatile("PUSH	TBLPAG");
	asm volatile("PUSH	ACCAL");
	asm volatile("PUSH	ACCAH");
	asm volatile("PUSH	ACCAU");
	asm volatile("PUSH	ACCBL");
	asm volatile("PUSH	ACCBH");
	asm volatile("PUSH	ACCBU");
	asm volatile("PUSH	CORCON");
	asm volatile("PUSH	DSRPAG");
	asm volatile("PUSH	DSWPAG");
	asm volatile("MOV		_current_task, W0");
	asm volatile("MOV		W15, [W0]");

  current_task = schedule();
  
  if(current_task == 0)
    panic();

  asm volatile("MOV #0x4FFE, W0");
  asm volatile("MOV W0, SPLIM");
  asm volatile("MOV	_current_task, W0");
  asm volatile("MOV	[W0], W15");
  asm volatile("MOV [W0+2], W0");
  asm volatile("MOV W0, SPLIM");
	asm volatile("POP		DSWPAG");
	asm volatile("POP		DSRPAG");
	asm volatile("POP		CORCON");
	asm volatile("POP		ACCBU");
	asm volatile("POP		ACCBH");
	asm volatile("POP		ACCBL");
	asm volatile("POP		ACCAU");
	asm volatile("POP		ACCAH");
	asm volatile("POP		ACCAL");
	asm volatile("POP		TBLPAG");
	asm volatile("POP		RCOUNT");
	asm volatile("POP.D	W12");
	asm volatile("POP.D	W10");
	asm volatile("POP.D	W8");
	asm volatile("POP.D	W6");
	asm volatile("POP.D	W4");
	asm volatile("POP.D	W2");
	asm volatile("POP.D	W0");
	asm volatile("POP		W14");
	asm volatile("POP		SR");
  
  exit_critical();
  }

/*********************************************************************
 * Function Name     : interlocked_increment                          *
 * Description       : This function performs an atomic increment(ie. *
 *                     performs the increment operation on a variable *
 *                     without the operation being interrupted until  *
 *                     it has been fully completed).                  *
 * Parameters        : unsigned long *value, the initial variable to  *
 *                     be incremented.                                *
 * Return Value      : unsigned long result, the incremented result.  *
 *********************************************************************/
unsigned int interlocked_increment(uint16_t *value)
{

    // Used to store the incremented result locally
    unsigned int result;

    enter_critical(); /* disable interrupts */

    // Increment value by 1 and store the result
    *value = *value + 1;
    result = *value;

    exit_critical(); /* enable interrupts */

    // Return the incremented value
    return result;
}


/*********************************************************************
 * Function Name     : interlocked_decrement                          *
 * Description       : This function performs an atomic decrement(ie. *
 *                     performs the decrement operation on a variable *
 *                     without the operation being interrupted until  *
 *                     it has been fully completed).                  *
 * Parameters        : unsigned long *value, the initial variable to  *
 *                     be decremented.                                *
 * Return Value      : unsigned long result, the decremented result.  *
 *********************************************************************/
unsigned int interlocked_decrement(uint16_t *value)
  {
  unsigned int result;
  enter_critical(); /* disable interrupts */

  *value = *value - 1;
  result = *value;

  exit_critical(); /* enable interrupts */

  return result;
  }

void init_deque(deque_t *deque, uint16_t element_size, void *buffer, uint16_t length)
  {
  deque->buffer = buffer;
  deque->length = length;
	deque->back = buffer;
	deque->front = buffer;
	deque->end = ((uint8_t *)buffer) + (length * element_size);
  deque->count = 0;
  deque->size = element_size;
  }

bool can_pop(deque_t *deque)
  {
  return deque->count > 0;
  }

uint16_t count(deque_t *deque)
  {
  return deque->count;
  }

void push_back(deque_t *deque, const void *item, uint32_t max_wait)
  {
  enter_critical();
  while(deque->count >= deque->length)
    wait(&deque->writers, max_wait);         // suspend task
  
  // copy the item into the queue
  memcpy(deque->back, item, deque->size);

  deque->count++;
  deque->back = ((uint8_t *)deque->back) + deque->size;

  if(deque->back >= deque->end)
    deque->back = deque->buffer;
  
  if(deque->readers.tasks.tail != 0)
    signal(&deque->readers);
  
  exit_critical();
  yield();
  }

bool push_back_from_isr(deque_t *deque, const void *item)
  {
  if(deque->count >= deque->length)
    return false;             // failed
  
  // copy the item into the queue
  memcpy(deque->back, item, deque->size);

  deque->count++;
  deque->back = ((uint8_t *)deque->back) + deque->size;

  if(deque->back >= deque->end)
    deque->back = deque->buffer;
  
  if(deque->readers.tasks.tail != 0)
    signal_from_isr(&deque->readers);

  return true;
  }

void pop_front(deque_t *deque, void *item, uint32_t max_wait)
  {
  enter_critical();
  if(deque->count == 0)
    wait(&deque->readers, max_wait);     // wait on an item

  if(item != 0)
    memcpy(item, deque->front, deque->size);
  
  deque->front = ((uint8_t *)deque->front) + deque->size;
  deque->count--;
  
  if(deque->front == deque->end)
    deque->front = deque->buffer;
  
  if(deque->writers.tasks.tail != 0)
    signal(&deque->writers);
  
  exit_critical();
  }

bool pop_front_from_isr(deque_t *deque, void *item)
  {
  if(deque->count == 0)
    return false;

  if(item != 0)
    memcpy(item, deque->front, deque->size);
  
  
  deque->front = ((uint8_t *)deque->front) + deque->size;
  deque->count--;

  if(deque->front == deque->end)
    deque->front = deque->buffer;
  
  if(deque->writers.tasks.tail != 0)
    signal_from_isr(&deque->writers);
  
  return true;
  }

uint16_t capacity(deque_t *deque)
  {
  return deque->length - deque->count;
  }

void set_event(event_t *event)
  {
  bool should_yield = false;
  enter_critical();
  
  should_yield = set_event_from_isr(event);
  
  exit_critical();
  if(should_yield)
    yield();
  }

void reset_event(event_t *event)
  {
  enter_critical();
  reset_event_from_isr(event);
  exit_critical();
  }

bool set_event_from_isr(event_t *event)
  {
  bool should_yield = false;

  event->flags |= EVENT_SET;
  while(event->tasks.tail != 0)
    {
    should_yield = true;
    tcb_t *task = event->tasks.tail;

    unblock_tcb(task);
    }
  
  if(should_yield && ((event->flags & EVENT_AUTORESET) != 0 ))
    event->flags &= ~EVENT_SET;
  
  return should_yield;
  }

void reset_event_from_isr(event_t *event)
  {
  event->flags &= ~EVENT_SET;
  }

extern void wait_for_event(event_t *event, uint32_t delay)
  {
  enter_critical();
  // special case for un-blocked semaphore
  if(event->tasks.tail == 0 && ((event->flags & EVENT_SET) != 0))
    {
    if((event->flags & EVENT_AUTORESET) != 0)
      event->flags &= ~EVENT_SET;
    
    exit_critical();
    return;
    }
  
  current_task->state = ts_blocked;
  current_task->delay = delay;
  current_task->waiting_on = &event->tasks;
  
  if(event->tasks.head != 0)
    {
    event->tasks.head->prev = current_task;
    current_task->next = event->tasks.head;
    }
  
  event->tasks.head = current_task;
  
  if(event->tasks.tail == 0)
    event->tasks.tail = current_task;
  
  exit_critical();
  
  // never call from an interrupt...
  yield();
  }

