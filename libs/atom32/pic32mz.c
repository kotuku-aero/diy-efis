#include "../atom/microkernel.h"
#include "../atom/memory.h"
#include <stdlib.h>
#include <string.h>
#include <sys/attribs.h>

static bool is_init = false;
// PBCLK3 is set to 10mhz for the internal periperals so
// for 1msec = 10000
#define TIMER 10000

static void idle_task(void *parg)
  {
  while(true)
    {
    exit_critical();
    // reset the watchdog timer
    // WDTCONSET = 0x01;
    
    asm volatile("wait");
    }
  }

typedef enum {
  EXCEP_IRQ = 0,          // interrupt
  EXCEP_MOD = 1,          // TLB Modified
  EXCEP_TLBL = 2,         // TLB Exception (Load or instr fetch)
  EXCEP_TLBS = 3,         // TLB Exception (store)
  EXCEP_AdEL = 4,         // address error exception (load or ifetch)
  EXCEP_AdES = 5,         // address error exception (store)
  EXCEP_IBE = 6,          // bus error (ifetch)
  EXCEP_DBE = 7,          // bus error (load/store)
  EXCEP_Sys = 8,          // syscall
  EXCEP_Bp = 9,           // breakpoint
  EXCEP_RI = 10,          // reserved instruction
  EXCEP_CpU = 11,         // coprocessor unusable
  EXCEP_Overflow = 12,    // arithmetic overflow
  EXCEP_Trap = 13,        // trap (possible divide by zero)
  EXCEP_FPE = 15,         // floating point exception
  EXCEP_TLBRI = 19,       // TLB Read Inhibit
  EXCEP_TLBEI = 20,       // TLB Execution Inhibit
  EXCEP_WATCH = 23,       // Reference to Watch Hi/Lo address
  EXCEP_MCHECK = 24,      // Machine check
  EXCEP_DSPDIS = 26,      // DSP Disabled exception
  } excep_code_t;

excep_code_t _excep_code;

uint32_t _excep_addr;
// this function overrides the normal _weak_ generic handler
void _general_exception_handler(void)
  {
  _excep_addr = __builtin_mfc0(14, 2);
  _excep_code = (excep_code_t) ((__builtin_mfc0(13, 0) >> 2) & 0x1f);

    panic();
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_TIMER_1_VECTOR))) _t1_interrupt( void );
void t1_interrupt(void)
  {
  IFS0bits.T1IF = 0;
  timer_interrupt();
  }

void task_abort()
  {
  // seruous problem here
  enter_critical();
  if(current_task != 0)
    current_task->state = ts_aborted;
  dispatch_task();
  
  while(1);
  }

static void task_exit()
  {
  // should just do a half despatch
  close_task((handle_t) current_task);
  dispatch_task();
  }

#ifdef _DEBUG
#define _DEBUG_STACK
#endif

#include "task_state.h"

// This is the actual TCB
typedef struct _pic32_tcb_t {
  tcb_t tcb;
  pic32_state_t state;
  
  uint64_t align[];
  } pic32_tcb_t;
  
extern void *_gp;

#ifdef _DEBUG_STACK
static const stack_type_t guard_words[16] =
  {
  0xbaadf00d,
  0x55aa55aa,
  0xaa55aa55,
  0x5555aaaa,
  0xbaadf00d,
  0x55aa55aa,
  0xaa55aa55,
  0x5555aaaa,
  0xbaadf00d,
  0x55aa55aa,
  0xaa55aa55,
  0x5555aaaa,
  0xbaadf00d,
  0x55aa55aa,
  0xaa55aa55,
  0x5555aaaa,
  };
#endif

volatile pic32_tcb_t *pic_task;

void set_next_task(volatile tcb_t *next_task)
  {
  pic_task = (volatile pic32_tcb_t *) next_task;
  }

volatile tcb_t *create_tcb(size_t len, task_callback callback, void *parg)
  {
  if((len & 0x01)!= 0)
    len++;
  
  size_t task_len = sizeof(pic32_tcb_t);
  size_t stack_len = (len * sizeof(stack_type_t));

  task_len += stack_len;
  task_len += sizeof(stack_type_t);         // allows us to align to an 8 byte boundary
  
  volatile pic32_tcb_t *task = (volatile pic32_tcb_t *)neutron_malloc(task_len);
  
  if(task == 0)
    return 0;
  
  memset((void *)task, 0, sizeof(pic32_tcb_t));
  
  // used to find the offset of the pic32_tcb_t
  task->tcb.version = sizeof(tcb_t);
  
  volatile stack_type_t *stack = &task->state.stack[0];

  stack += len;     // point stack to top of stack area
  
  task->state.stack_len = len;
  
  stack--;          // always point to next stack
  stack--;
  *stack-- = 0;     // 
  *stack-- = 0;     // 
  *stack-- = 0;     // 
  *stack-- = 0;     // 
  task->state.regs[31] = (stack_type_t)task_abort;
  task->state.regs[30] = (stack_type_t)stack;     // fp
  task->state.regs[29] = (stack_type_t)stack;     // sp
  task->state.regs[28] = (stack_type_t)&_gp;      // linker provided
  task->state.regs[5] = (stack_type_t)stack;      // R05 = sp
  task->state.regs[4] = (stack_type_t) parg;  // R05 = a0
  
#ifdef __PIC32_HAS_FPU64
  task->state.fp_status = 0x010C0000;             // initial status 
#endif
  
  task->state.epc = (uint32_t) callback;
  task->state.status = 0x25000003;  // EXL and IE are both set, MZ, FR and CU1
  
#ifdef _DEBUG_STACK
  // init the stack guard
  memcpy(((pic32_tcb_t *)task)->state.stack_guard, guard_words, sizeof(guard_words));
#endif

  // and return the address of the task.
  return &task->tcb;
  }

void release_tcb(volatile tcb_t *tcb)
  {
  // release all memory
  neutron_free((void *)tcb);
  }

#ifdef _DEBUG_STACK
  // called during a yield() will check the tcb
void stkchk()
  {
  pic32_tcb_t *task = (pic32_tcb_t *)current_task;
  
  if(task != 0 &&
    (task->state.regs[29] < (uint32_t)&task->align[0] ||
     memcmp(task->state.stack_guard, guard_words, sizeof(guard_words))!= 0))
    {
    // the stack pointer is too low or the stack has gone below
    // the bottom of the stack
    panic();
    }
   }
#endif

static uint8_t *aligned_heap;

extern void neutron_run(void *the_heap,
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

  // _mtc0(_CP0_SRSMAP, _CP0_SRSMAP_SELECT, 0x76543210);
  // set up TMR1 as our kernel timer.
  PR1 = TIMER;                         // 1 msec timer.
  TMR1 = 0;
  T1CONbits.TCKPS = 0;                // clk is 1:1
  T1CONbits.TCS = 0;                  // select clock source to free-run
  T1CONbits.TGATE = 0;

  // set up timer 1
  IPC1bits.T1IP = 2;
  IFS0bits.T1IF = 0;
  IEC0bits.T1IE = 1;

  // set to low priority for yield
  IPC0bits.CS0IP = 1;
  IPC0bits.CS0IS = 0;

  IEC0bits.CS0IE = 1;

  T1CONbits.TON = 1;

  // set the MX, and CU1 bit
  /*
  uint32_t sr = _mfc0(_CP0_STATUS, _CP0_STATUS_SELECT);
  sr |= 0x21000000;
  _mtc0(_CP0_STATUS, _CP0_STATUS_SELECT, sr);
   * */

  // this will allocate a new task 
  task_create(name, stack_size, callback, parg, priority, task);

  // make sure no IRQ till the new stack is swapped in
  enter_critical();

  dispatch_task();
  }