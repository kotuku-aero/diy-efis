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
#ifndef __microkernel_h__
#define __microkernel_h__

/*
 * this is microkernel that can run on PIC micrprocessors with little or no memory
 *
 * the model uses callbacks instead of context switching.
 *
 * the system maintains up to 16 tasks that can be called.
 */

#include <stdint.h>
#include <stdbool.h>

struct _task_control_block_t;

typedef struct _linked_list_t
  {
  struct _task_control_block_t *head;
  struct _task_control_block_t *tail;
  } linked_list_t;

/** A semaphore allows one or more tasks to wait on a counter.
 * Each task is released when the semaphore is signalled.
 * If no tasks are waiting a count will be added
 */
typedef struct _semaphore_t
  {
  linked_list_t tasks;
  uint16_t count;
  } semaphore_t;
  
  
/** An event is similar to a semaphore however it can be
 * either set, or reset.
 * If the flags are set to autoreset, when at least one task
 * is removed from the task list, the event will be cleared
 */
#define EVENT_SET 0x0001
#define EVENT_AUTORESET 0x8000

#define NO_WAIT 0
#define INDEFINITE_WAIT 0xFFFFFFFF

  typedef struct _event_t
  {
  linked_list_t tasks;
  uint16_t flags;
  } event_t;
  
typedef enum _task_state
  {
  ts_suspended,
  ts_blocked,
  ts_running,
  ts_terminated,
  ts_aborted
  } task_state;

typedef struct _task_control_block_t {
  uint16_t *stack_top;
  uint16_t *stack_limit;
  uint16_t *stack;
  uint8_t priority;
  // the task can wait on this
  linked_list_t *waiting_on;
  // next task waiting on semaphore
  struct _task_control_block_t *next;  
  // previous task waiting on semaphore
  struct _task_control_block_t *prev;
  // how long to wait on a semaphore before we stop
  uint32_t delay;
  task_state state;
  char task_name[16];
  } tcb_t;

  
typedef void (*task_callback)(void *parg);
/**
   * Initialize the scheduler with application storage
   * @param tcbs        Task control blocks
   * @param max_tasks   Maximum supported tasks
   * @param idle_task   Task procedure, if 0 no idle task is created
   * @param stack       Idle task stack
   * @param stack_size  Length of idle task stack
   * @return Idle task id, -1 if no idle task created
   */
extern int8_t scheduler_init(tcb_t *tcbs,
                             int8_t max_tasks,
                             task_callback idle_task,
                             uint16_t *stack,
                             uint16_t stack_size);
extern volatile tcb_t *current_task;
/**
 * run the microkernel and set a custom despatcher idle proc
 * @param idle_task     task to resume that is the idle task.  Must be provided
 */
extern void run(int8_t idle_task);
/**
 * signal that an event is set.  Should be called from an interrupt service
 * routine.
 * @param event_mask
 */
extern void signal(semaphore_t *semaphore);
/**
 * Signal within an isr
 * @param semaphore
 */
extern void signal_from_isr(semaphore_t *semaphore);
/**
 * Set an event.  Will cause a schedule swap if any tasks are released
 * @param event Event to set
 */
extern void set_event(event_t *event);
/**
 * Reset an event
 * @param event Event to clear
 */
extern void reset_event(event_t *event);
/**
 * Set an event from an interrupt does not cause a schedule
 * @param event
 * @returns true if at least 1 task was scheduled to run
 */
extern bool set_event_from_isr(event_t *event);
/**
 * Reset and event from an interrupt
 * @param event
 */
extern void reset_event_from_isr(event_t *event);
/**
 * Wait for an event to be set.  May cause the task to block
 * @param event
 */
extern void wait_for_event(event_t *event, uint32_t delay);

typedef struct _deque
  {
  semaphore_t readers;
  semaphore_t writers;
  void *buffer;
  void *back;
  void *front;
  void *end;
  uint16_t length;      // number of elements that can be queued
  uint16_t size;        // size of elements in the queue
  uint16_t count;
  } deque_t;

/**
 * Initialize a deque structure
 * @param deque         allocated queue
 * @param element_size  size of elements in the queue
 * @param buffer        buffer of elements
 * @param length        length of buffer as number of elements
 */
extern void init_deque(deque_t *deque, uint16_t element_size, void *buffer, uint16_t length);
/**
 * return true if there is an item on the queue
 * @param   queue to pop
 * @return  true if an item available
 */
extern bool can_pop(deque_t *deque);
/**
 * Number of items in the deque
 * @param deque  deque to query
 * @return  number of items in the deque
 */
extern uint16_t count(deque_t *deque);
/**
 * Push an item onto the deque.  will block if no space
 * @param deque   queue to push onto
 * @param item    item to copy onto queue
 */
extern void push_back(deque_t *deque, const void *item, uint32_t max_wait);
/**
 * Push an item onto the deque.  will block if no space
 * @param deque   queue to push onto
 * @param item    item to copy onto queue
 * @returns true if the item was pushed onto the queue
 */
extern bool push_back_from_isr(deque_t *deque, const void *item);
/**
 * Remove an item from a queue
 * @param deque deque to pop from
 * @param item  if non 0 copy of item
 */
extern void pop_front(deque_t *deque, void *item, uint32_t max_wait);
/**
 * Remove an item from a queue
 * @param deque deque to pop from
 * @param item  if non 0 copy of item
 */
extern bool pop_front_from_isr(deque_t *deque, void *item);
/**
 * Return the number of items that can be in a deque
 * @param 
 * @return 
 */
extern uint16_t capacity(deque_t *deque);
/**
 * Suspend the calling task, waiting for an event
 * @param event_mask  event to wait on.
 */
extern void wait(semaphore_t *semaphore, uint32_t ticks);
/**
 * Release a task irrespective of wait
 * @param task
 */
extern void unblock(int8_t task);
/**
 * Create a new scheduled task
 * @param name          name of the task
 * @param stack_size    size of stack to allocate (words)
 * @param callback      task main routine
 * @param parg          argument to pass to task
 * @param priority      initial priority
 * @return task ordinal, if no tasks remaining will return MAX_TASKS
 */
extern int8_t create_task(const char *name,
                           uint16_t *stack,
                           uint16_t stack_size,
                           task_callback callback,
                           void *parg,
                           uint8_t priority);

#define IDLE_PRIORITY 0
#define NORMAL_PRIORITY 128
#define HIGH_PRIORITY 192
#define MAX_PRIORITY 255

#define IDLE_STACK_SIZE 128
#define DEFAULT_STACK_SIZE 256

/**
 * Change the priority of a scheduled task
 * @param task        task to change
 * @param priority    priority to set
 */
extern void set_task_priority(int8_t task, uint8_t priority);
/**
 * Return the priority for a task
 * @param task        task to query
 * @return            assigned priority
 */
extern uint8_t get_task_priority(int8_t task);
/**
 * Suspend a task
 * @param task
 */
extern void suspend(int8_t task);
/**
 * Resume a suspended task
 * @param task  task to resumt
 */
extern void resume(int8_t task);
/**
 * return the 1khz tick counter
 * @return 
 */
extern uint32_t ticks();
/*
 * Perform a task switch.
 */
extern void yield();
/**
 * Atomic increment
 * @param value
 * @return 
 */
extern uint16_t interlocked_increment(uint16_t *value);
/**
 * Atomic decrement
 * @param value
 * @return 
 */
extern uint16_t interlocked_decrement(uint16_t *value);
/**
 * Kernel panic.
 * will reset the system.
 */
extern void panic();

/*  Never call from inside an interrupt!
 */
#define enter_critical() __builtin_disi(0x3FFF)
#define exit_critical() __builtin_disi(0x0000)

#endif
