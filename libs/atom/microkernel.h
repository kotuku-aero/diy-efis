#ifndef __microkernel_h__
#define __microkernel_h__

/*
 * this is microkernel that can run on PIC microprocessors with little or no memory
 *
 * the model uses callbacks instead of context switching.
 *
 * the system maintains up to 16 tasks that can be called.
 */

#include <stdint.h>
#include <stdbool.h>

#include "../../libs/neutron/neutron.h"
#if defined(__dsPIC33EP512GP504__) | defined(__dsPIC33EP512GP506__) | defined(__dsPIC33EP256GP502__) 
#include <p33Exxxx.h>
#endif
#if defined(__PIC32MZ__)
#include "../../libs/ion/ion.h"
#include <xc.h>
#endif

/**
 * Signal within an isr
 * @param semaphore
 */
extern result_t signal_from_isr(semaphore_p semaphore);
/**
 * Push an item onto the deque.  will block if no space
 * @param deque   queue to push onto
 * @param item    item to copy onto queue
 * @returns s_ok if the item was pushed onto the queue
 */
extern result_t push_back_from_isr(deque_p deque, const void *item);
/**
 * Remove an item from a queue
 * @param deque deque to pop from
 * @param item  if non 0 copy of item
 * @return s_ok if item available in queue
 */
extern result_t pop_front_from_isr(deque_p deque, void *item);
/**
 * Release a task irrespective of wait
 * @param task
 */
extern void unblock(task_p task);
/**
 * Kernel panic.
 * will reset the system.
 */
extern void panic();


typedef struct _linked_list_t
  {
  volatile struct _task_control_block_t *head;
  volatile struct _task_control_block_t *tail;
  } linked_list_t;

/** A semaphore allows one or more tasks to wait on a counter.
 * Each task is released when the semaphore is signalled.
 * If no tasks are waiting a count will be added
 */
typedef struct _semaphore_t
  {
  linked_list_t tasks;
  int32_t count;
  } semaphore_t;
  
  
typedef enum _task_state
  {
  ts_suspended,
  ts_blocked,
  ts_running,
  ts_terminated,
  ts_aborted
  } task_state;

  // this must be mod8
typedef struct _task_control_block_t {
  uint16_t version;     // this must be sizeof(tcb_t) 
  uint8_t priority;
  uint8_t reserved;
  // the task can wait on this
  linked_list_t *waiting_on;
  // next task waiting on semaphore
  volatile struct _task_control_block_t *next;  
  // previous task waiting on semaphore
  volatile struct _task_control_block_t *prev;
  // how long to wait on a semaphore before we stop
  uint32_t delay;
  task_state state;
  char task_name[16];
  // all tasks at same priority
  volatile struct _task_control_block_t *next_task;
  volatile struct _task_control_block_t *prev_task;
  uint32_t reserved2;
  uint32_t reserved3;
  } tcb_t;

// This is the task that is running. exposed so we can add a watch onto it.
extern volatile tcb_t *current_task;
/**
 * @function create_tcb(size_t stack_size, task_callback callback, void *parg)
 * Create a new task and initialize it.
 * @param stack_size
 * @param callback
 * @param parg
 * @return newly created task
 */
extern volatile tcb_t *create_tcb(size_t stack_size, task_callback callback, void *parg);
/**
 * @function release_tcb(volatile tcb_t *tcb)
 * Release all resources associated witha  tcb
 * @param tcb tcb to release
 */
extern void release_tcb(volatile tcb_t *tcb);
extern void task_abort();
extern volatile tcb_t *schedule();
extern void timer_interrupt();
extern void dispatch_task();
extern void unblock_tcb(volatile tcb_t *task);

// the hook function is called WITHIN an interrupt so only
// IRQ safe operations can occur.  ALso enter_critical and exit_critical cannot
// be used.
typedef void (*timer_fn)(uint32_t tcnt);

typedef struct _timer_hook_t {
  struct _timer_hook_t *next;
  struct _timer_hook_t *prev;
  timer_fn callback;
  } timer_hook_t;

result_t timer_hook(timer_hook_t *hook);
result_t timer_unhook(timer_hook_t *unhook);

extern bool scheduler_enabled;
extern result_t has_wait_tasks(semaphore_p hndl);


#if defined(__PIC32MZ__)
// register the atom specific ION interpreter functions
extern result_t register_atom_functions(duk_context *ctx, handle_t co);
#endif

#endif
