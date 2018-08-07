#include "../atom/microkernel.h"
#include "../atom/memory.h"
#include <stdlib.h>
#include <string.h>

// start the timer running  We need a 1 msec interval
// Fp = 20mhz, TCKPS = /8
// clock = 8.75mhz
// interval = 1khz = 8750
#define TIMER (1250-1)

static void idle_task(void *parg)
  {
  while(true)
    {
    exit_critical();

    asm volatile("PWRSAV #1"); 
    yield();
    }
  }

static uint8_t *aligned_heap;

void neutron_run(void *the_heap,
                         size_t heap_size,
                        const char *name,
                        uint16_t stack_size,
                        task_callback callback,
                        void *parg,
                        uint8_t priority,
                        task_p *task)
  {
  scheduler_enabled = true;
  
  // init the memory allocation routines
  // get our heap
  block_link_t *first_free_block;
  size_t address;
  size_t total_heap_size = heap_size;

  /* Ensure the heap starts on a correctly aligned boundary. */
  address = (size_t) the_heap;

  if ((address & 1) != 0)
    {
    address += (2 - 1);
    address &= ~((size_t) 1);
    total_heap_size -= address - (size_t) the_heap;
    }

  aligned_heap = (uint8_t *) address;

  /* xStart is used to hold a pointer to the first item in the list of free
  blocks.  The void cast is used to prevent compiler warnings. */
  start_of_heap.next_free_block = (void *) aligned_heap;
  start_of_heap.block_size = (size_t) 0;

  /* pxEnd is used to mark the end of the list of free blocks and is inserted
  at the end of the heap space. */
  address = ((size_t) aligned_heap) + total_heap_size;
  address -= sizeof (block_link_t);
  address &= ~((size_t) 1);
  end_of_heap = (void *) address;
  end_of_heap->block_size = 0;
  end_of_heap->next_free_block = 0;

  /* To start with there is a single free block that is sized to take up the
  entire heap space, minus the space taken by pxEnd. */
  first_free_block = (void *) aligned_heap;
  first_free_block->block_size = address - (size_t) first_free_block;
  first_free_block->next_free_block = end_of_heap;

  /* Only one block exists - and it covers the entire usable heap space. */
  minimum_free_bytes_remaining = first_free_block->block_size;
  free_bytes_remaining = first_free_block->block_size;

  /* Work out the position of the top bit in a size_t variable. */
  block_allocated_bit = ((size_t) 1) << ((sizeof ( size_t) << 3) - 1);
  
    // create the idle task.
  task_create("IDLE", IDLE_STACK_SIZE, idle_task, 0, IDLE_PRIORITY, 0);

  // this will allocate a new task pasth 
  task_create(name, stack_size, callback, parg, priority, task);
  
  // make sure no IRQ till the new stack is swapped in
  __builtin_disi(1024);
  
  // set up TMR1 as our kernel timer.
  PR1 = TIMER;                         // 1 msec timer.
  TMR1 = 0;
  T1CONbits.TCKPS = 1;                 // clk / 8 or 0.4 usec clock to counter
  T1CONbits.TCS = 0;                   // select clock source to free-run
  T1CONbits.TGATE = 0;
  T1CONbits.TON = 1;

  // allow timer interrupt after the first task dispatched.
  IFS0bits.T1IF = 0;
  IEC0bits.T1IE = 1;
  IPC0bits.T1IP = 4;

  dispatch_task();
  
  asm volatile("return");
  }

void __attribute__((interrupt, auto_psv)) _T1Interrupt(void)
  {
  IFS0bits.T1IF = 0;
  timer_interrupt();
  
  asm volatile("CLRWDT");
  yield();
  }

extern void dispatch_task(void);

void task_abort()
  {
  // seruous problem here
  enter_critical();
  if(current_task != 0)
    current_task->state = ts_aborted;
  dispatch_task();
  
  while(1);
  }

typedef uint16_t stack_type_t;

typedef struct _tcb16_t {
  volatile tcb_t tcb;       // 40 bytes
  uint16_t splim;
  uint16_t sp;
  uint16_t sr;
  uint16_t w14;
  uint16_t w12;
  uint16_t w13;
  uint16_t w10;
  uint16_t w11;
  uint16_t w8;
  uint16_t w9;
  uint16_t w6;
  uint16_t w7;
  uint16_t w4;
  uint16_t w5;
  uint16_t w2;
  uint16_t w3;
  uint16_t w0;
  uint16_t w1;
  uint16_t rcount;
  uint16_t tblpag;
  uint16_t accal;
  uint16_t accah;
  uint16_t accau;
  uint16_t accbl;
  uint16_t accbh;
  uint16_t accbu;
  uint16_t dsrpag;
  uint16_t dswpag;
  uint16_t corcon;
  uint16_t guard[8];     // used when a non maskable IRQ occurs
                         // ensures the stack cannot overwrite the user stack
  stack_type_t stack[];
  } tcb16_t;

volatile tcb_t *create_tcb(size_t len, task_callback callback, void *parg)
  {
  stack_type_t value;
  
  uint16_t tcb_len = sizeof(tcb16_t) + (sizeof(stack_type_t) * len);
  
  tcb16_t *tcb = (tcb16_t *)neutron_malloc(tcb_len);
  memset(tcb, 0, tcb_len);
  
  tcb->tcb.version = sizeof(tcb_t);
  
  tcb->splim = ((uint16_t)tcb) + tcb_len;
  
  stack_type_t *stack = &tcb->stack[0];
  
  
#ifdef _DEBUG
  tcb->w1 = 0x1111;
  tcb->w2 = 0x2222;
  tcb->w3 = 0x3333;
  tcb->w4 = 0x4444;
  tcb->w5 = 0x5555;
  tcb->w6 = 0x6666;
  tcb->w7 = 0x7777;
  tcb->w8 = 0x8888;
  tcb->w9 = 0x9999;
  tcb->w10 = 0xaaaa;
  tcb->w11 = 0xbbbb;
  tcb->w12 = 0xcccc;
  tcb->w13 = 0xdddd;
  tcb->rcount = 0xcdce;
  tcb->accal = 0x0202;
  tcb->accah = 0x0303;
  tcb->accau = 0x0404;
  tcb->accbl = 0x0505;
  tcb->accbh = 0x0606;
  tcb->accbu = 0x0707;
#endif
  // and save the stack pointer (note stack upside down so R15 == 0)
  tcb->sr = SR;
  tcb->corcon = CORCON;
  tcb->tblpag = TBLPAG;
  tcb->dsrpag = DSRPAG;
  tcb->dswpag = DSWPAG;
  // we provide a call to task_abort in case the thread exits
  value = (stack_type_t) task_abort;
  *stack++ = value;
  *stack++ = 0;
  tcb->w14 = (uint16_t)stack;       // create our frame pointer
  
  // Save the low bytes of the program counter.
  value = (stack_type_t) callback;
  
  *stack++ = value;

  // Save the high byte of the program counter.
  *stack++ = 0;

  // the yield routine does not have a frame pointer.

  /* Parameters are passed in W0. */
  tcb->w0 = (stack_type_t) parg;
  
  tcb->sp = (uint16_t)stack;
  
  return &tcb->tcb;
  }

volatile tcb16_t *pic_task;

void set_next_task(volatile tcb_t *next_task)
  {
  pic_task = (volatile tcb16_t *) next_task;
  }

void release_tcb(volatile tcb_t *tcb)
  {
  // release all memory
  neutron_free((void *)tcb);
  }


#ifdef _DEBUG
void stkchk()
  {
  
  }
#endif
 
typedef enum _error_type
  {
  e_oscillator_fail,
  e_address_error,
  e_stack_error,
  e_math_error,
  e_dmac_error
  } error_type;

void *err_addr;
error_type error;


void __attribute__((__interrupt__)) _OscillatorFail(void);
void __attribute__((__interrupt__)) _AddressError(void);
void __attribute__((__interrupt__)) _StackError(void);
void __attribute__((__interrupt__)) _MathError(void);
void __attribute__((__interrupt__)) _DMACError(void);

/*
Primary Exception Vector handlers:
These routines are used if INTCON2bits.ALTIVT = 0.
All trap service routines in this file simply ensure that device
continuously executes code within the trap service routine. Users
may modify the basic framework provided here to suit to the needs
of their application.
 */
void __attribute__((interrupt, auto_psv)) _OscillatorFail(void)
  {
  INTCON1bits.OSCFAIL = 0; //Clear the trap flag
  error = e_oscillator_fail;
  panic();
  }

extern void *get_error_location();

void __attribute__((interrupt, auto_psv)) _AddressError(void)
  {
  err_addr = get_error_location();
  INTCON1bits.ADDRERR = 0; //Clear the trap flag
  error = e_address_error;
  panic();
  }

void *error_sp;
uint16_t error_stack[32];

void __attribute__((interrupt, auto_psv)) _StackError(void)
  {
  // get a new stack as the stack has failed
  asm volatile("MOV W15, _error_sp");
  asm volatile("MOV _error_stack, W15");
  asm volatile("MOV _error_stack, W0");
  asm volatile("ADD #64, W0");
  asm volatile("MOV W0, SPLIM");
  
  INTCON1bits.STKERR = 0; //Clear the trap flag
  error = e_stack_error;
  panic();
  }

void __attribute__((interrupt, auto_psv)) _MathError(void)
  {
  INTCON1bits.MATHERR = 0; //Clear the trap flag
  error = e_math_error;
  panic();
  }

void __attribute__((interrupt, auto_psv)) _DMACError(void)
  {
  INTCON1bits.DMACERR = 0; //Clear the trap flag
  error = e_dmac_error;
  panic();
  }
