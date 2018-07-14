/* 
 * File:   task_state.h
 * Author: peter.nunn
 *
 * Created on 8 November 2017, 9:55 AM
 */

#ifndef TASK_STATE_H
#define	TASK_STATE_H

#if defined (__LANGUAGE_C__) || defined (__LANGUAGE_C_PLUS_PLUS)

#ifdef	__cplusplus
extern "C"
  {
#endif
  

typedef uint32_t stack_type_t;

typedef struct _pic32_state_t{
  // this holds the task context
  // these are the pic32 registers
  // note regs[0] is r0 which is always 0 but this makes code look neater
  uint32_t regs[32];
  // epc register in cp0
  uint32_t epc;
  // status register in cp0
  uint32_t status;
  // DSP Control
  uint32_t dsp_control; 
  uint32_t reserved;
  // AC0, AC1, AC2, AC3
  uint64_t ac[4];
#ifdef __PIC32_HAS_FPU64
  // FPU registers stored here
  uint64_t fp[32];
  uint32_t fp_status;
#endif
  size_t stack_len;
#ifdef _DEBUG_STACK
  stack_type_t stack_guard[16];   // filled with gaurd data
#endif
  volatile stack_type_t stack[];
  } pic32_state_t;



#ifdef	__cplusplus
  }
#endif
#endif

#define OFFSET_R1  4
#define OFFSET_R2  8
#define OFFSET_R3  12
#define OFFSET_R4  16
#define OFFSET_R5  20
#define OFFSET_R6  24
#define OFFSET_R7  28
#define OFFSET_R8  32
#define OFFSET_R9  36
#define OFFSET_R10 40
#define OFFSET_R11 44
#define OFFSET_R12 48
#define OFFSET_R13 52
#define OFFSET_R14 56
#define OFFSET_R15 60
#define OFFSET_R16 64
#define OFFSET_R17 68
#define OFFSET_R18 72
#define OFFSET_R19 76
#define OFFSET_R20 80
#define OFFSET_R21 84
#define OFFSET_R22 88
#define OFFSET_R23 92
#define OFFSET_R24 96
#define OFFSET_R25 100
#define OFFSET_R26 104
#define OFFSET_R27 108
#define OFFSET_R28 112
#define OFFSET_R29 116
#define OFFSET_R30 120
#define OFFSET_R31 124
#define OFFSET_EPC 128
#define OFFSET_STATUS 132
#define OFFSET_DSP_CONTROL 136
#define OFFSET_RESERVED 140
#define OFFSET_AC0 144
#define OFFSET_AC1 152
#define OFFSET_AC2 160
#define OFFSET_AC3 168
#ifdef __PIC32_HAS_FPU64
#define OFFSET_FP0  176
#define OFFSET_FP1  184
#define OFFSET_FP2  192
#define OFFSET_FP3  200
#define OFFSET_FP4  208
#define OFFSET_FP5  216
#define OFFSET_FP6  224
#define OFFSET_FP7  232
#define OFFSET_FP8  240
#define OFFSET_FP9  248
#define OFFSET_FP10 256
#define OFFSET_FP11 264
#define OFFSET_FP12 272
#define OFFSET_FP13 280
#define OFFSET_FP14 288
#define OFFSET_FP15 296
#define OFFSET_FP16 304
#define OFFSET_FP17 312
#define OFFSET_FP18 320
#define OFFSET_FP19 328
#define OFFSET_FP20 336
#define OFFSET_FP21 344
#define OFFSET_FP22 352
#define OFFSET_FP23 360
#define OFFSET_FP24 368
#define OFFSET_FP25 376
#define OFFSET_FP26 384
#define OFFSET_FP27 392
#define OFFSET_FP28 400
#define OFFSET_FP29 408
#define OFFSET_FP30 416
#define OFFSET_FP31 424
#define OFFSET_FPSTATUS 432

#define SIZEOF_PIC32_STATE 436
#else
#define SIZEOF_PIC32_STATE 176
#endif
#endif	/* TASK_STATE_H */

