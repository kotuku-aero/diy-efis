#include <xc.h>
#include <sys/asm.h>
    
#include "task_state.h"
#define ISR_CONTEXT_SIZE 16

    .section .kernel, code, keep
    .set at
    
    .global enter_critical
    .global exit_critical
    .global yield
    .global dispatch_task
    
    .ent enter_critical
enter_critical:
    di
    ehb
    jr ra
    nop
    .end enter_critical
    
    .ent exit_critical
exit_critical:
    ei
    ehb
    jr ra
    nop
    .end exit_critical
    
    /* Make room for the context. First save the current status so it can be
    manipulated, and the cause and EPC registers so their original values are
    captured. */
    .ent yield
yield:
    di
/*
    We push:
     $k0
     $k1
     $status
     $epc
*/
    addiu		sp, sp, -ISR_CONTEXT_SIZE
    sw			k0, 12(sp)
    sw			k1, 8(sp)
    mfc0		k1, _CP0_STATUS
    ori			k1, 0x00000003		// set the EXL and EI bits
    sw			k1, 4(sp)
    // this saves the EPC_REGISTER which in our case is the return from the yield
    sw			ra, 0(sp)
    
    la      k1, current_task
    lw	  k0, 0(k1)	    // first word is version which is sizeof(tcb_t)
    lh	  k1, 0(k0)	    // should be 64 bytes as of writing
    add	  k0, k1	    // offset to the state
    
    mflo   k1, $ac0
    sw	    k1, OFFSET_AC0(k0)
    mfhi    k1, $ac0
    sw	    k1, OFFSET_AC0+4(k0)
   
    mflo   k1, $ac1
    sw	    k1, OFFSET_AC1(k0)
    mfhi    k1, $ac1
    sw	    k1, OFFSET_AC1+4(k0)
   
    mflo   k1, $ac2
    sw	    k1, OFFSET_AC2(k0)
    mfhi    k1, $ac2
    sw	    k1, OFFSET_AC2+4(k0)
   
    mflo   k1, $ac3
    sw	    k1, OFFSET_AC3(k0)
    mfhi    k1, $ac3
    sw	    k1, OFFSET_AC3+4(k0)

#ifdef __PIC32_HAS_FPU64
   
    cfc1    k1, $f31
    sw	    k1, OFFSET_FPSTATUS(k0)
    
    sdc1		$f0, OFFSET_FP0(k0)
    sdc1		$f1, OFFSET_FP1(k0)
    sdc1		$f2, OFFSET_FP2(k0)
    sdc1		$f3, OFFSET_FP3(k0)
    sdc1		$f4, OFFSET_FP4(k0)
    sdc1		$f5, OFFSET_FP5(k0)
    sdc1		$f6, OFFSET_FP6(k0)
    sdc1		$f7, OFFSET_FP7(k0)
    sdc1		$f8, OFFSET_FP8(k0)
    sdc1		$f9, OFFSET_FP9(k0)
    sdc1		$f10, OFFSET_FP10(k0)
    sdc1		$f11, OFFSET_FP11(k0)
    sdc1		$f12, OFFSET_FP12(k0)
    sdc1		$f13, OFFSET_FP13(k0)
    sdc1		$f14, OFFSET_FP14(k0)
    sdc1		$f15, OFFSET_FP15(k0)
    sdc1		$f16, OFFSET_FP16(k0)
    sdc1		$f17, OFFSET_FP17(k0)
    sdc1		$f18, OFFSET_FP18(k0)
    sdc1		$f19, OFFSET_FP19(k0)
    sdc1		$f20, OFFSET_FP20(k0)
    sdc1		$f21, OFFSET_FP21(k0)
    sdc1		$f22, OFFSET_FP22(k0)
    sdc1		$f23, OFFSET_FP23(k0)
    sdc1		$f24, OFFSET_FP24(k0)
    sdc1		$f25, OFFSET_FP25(k0)
    sdc1		$f26, OFFSET_FP26(k0)
    sdc1		$f27, OFFSET_FP27(k0)
    sdc1		$f28, OFFSET_FP28(k0)
    sdc1		$f29, OFFSET_FP29(k0)
    sdc1		$f30, OFFSET_FP30(k0)
    sdc1		$f31, OFFSET_FP31(k0)
#endif
    
    .set noat
    sw	    $1, OFFSET_R1(k0)
    sw	    $2, OFFSET_R2(k0)
    sw	    $3, OFFSET_R3(k0)
    sw	    $4, OFFSET_R4(k0)
    sw	    $5, OFFSET_R5(k0)
    sw	    $6, OFFSET_R6(k0)
    sw	    $7, OFFSET_R7(k0)
    sw	    $8, OFFSET_R8(k0)
    sw	    $9, OFFSET_R9(k0)
    sw	    $10, OFFSET_R10(k0)
    sw	    $11, OFFSET_R11(k0)
    sw	    $12, OFFSET_R12(k0)
    sw	    $13, OFFSET_R13(k0)
    sw	    $14, OFFSET_R14(k0)
    sw	    $15, OFFSET_R15(k0)
    sw	    $16, OFFSET_R16(k0)
    sw	    $17, OFFSET_R17(k0)
    sw	    $18, OFFSET_R18(k0)
    sw	    $19, OFFSET_R19(k0)
    sw	    $20, OFFSET_R20(k0)
    sw	    $21, OFFSET_R21(k0)
    sw	    $22, OFFSET_R22(k0)
    sw	    $23, OFFSET_R23(k0)
    sw	    $24, OFFSET_R24(k0)
    sw	    $25, OFFSET_R25(k0)
    sw	    $28, OFFSET_R28(k0)
    sw	    $30, OFFSET_R30(k0)
    sw	    $31, OFFSET_R31(k0)
    
    // save the status pushed onto the stack
    lw	    k1, 12(sp)
    sw	    k1, OFFSET_R26(k0)
    lw	    k1, 8(sp)
    sw	    k1, OFFSET_R27(k0)
    lw	    k1, 4(sp)
    sw	    k1, OFFSET_STATUS(k0)
    lw	    k1, 0(sp)
    sw	    k1, OFFSET_EPC(k0)
    
    // loose the stack frame
    addi   sp, sp, ISR_CONTEXT_SIZE
    sw	    sp, OFFSET_R29(k0)
    
    /* s5 is used as the frame pointer. */
    add			s5, zero, sp
    
    j dispatch_task
    nop
    .end yield
    
    .ent		dispatch_task
dispatch_task:
  jal schedule		// get the first ready task.
  nop
  
  // schedule sets current task to the next task to run.
  la      k1, current_task
  lw	  k0, 0(k1)	    // first word is version which is sizeof(tcb_t)
  lh	  k1, 0(k0)	    // should be 64 bytes as of writing
  add	  k0, k1	    // offset to the state
  
  // create a stack frame
  lw	  sp, OFFSET_R29(k0)
  
  addiu  sp, sp, -ISR_CONTEXT_SIZE
  
  lw	    k1, OFFSET_R26(k0)
  sw	    k1, 12(sp)
  lw	    k1, OFFSET_R27(k0)
  sw	    k1, 8(sp)
  lw	    k1, OFFSET_STATUS(k0)
  sw	    k1, 4(sp)
  lw	    k1, OFFSET_EPC(k0)
  sw	    k1, 0(sp)
  
    lw	    k1, OFFSET_AC0(k0)
    mtlo   k1, $ac0
    lw	    k1, OFFSET_AC0+4(k0)
    mthi    k1, $ac0
   
    lw	    k1, OFFSET_AC1(k0)
    mtlo   k1, $ac1
    lw	    k1, OFFSET_AC1+4(k0)
    mthi    k1, $ac1
   
    lw	    k1, OFFSET_AC2(k0)
    mtlo   k1, $ac2
    lw	    k1, OFFSET_AC2+4(k0)
    mthi    k1, $ac2
   
    lw	    k1, OFFSET_AC3(k0)
    mtlo   k1, $ac3
    lw	    k1, OFFSET_AC3+4(k0)
    mthi    k1, $ac3

#ifdef __PIC32_HAS_FPU64
    .set at
    ldc1		$f0, OFFSET_FP0(k0)
    ldc1		$f1, OFFSET_FP1(k0)
    ldc1		$f2, OFFSET_FP2(k0)
    ldc1		$f3, OFFSET_FP3(k0)
    ldc1		$f4, OFFSET_FP4(k0)
    ldc1		$f5, OFFSET_FP5(k0)
    ldc1		$f6, OFFSET_FP6(k0)
    ldc1		$f7, OFFSET_FP7(k0)
    ldc1		$f8, OFFSET_FP8(k0)
    ldc1		$f9, OFFSET_FP9(k0)
    ldc1		$f10, OFFSET_FP10(k0)
    ldc1		$f11, OFFSET_FP11(k0)
    ldc1		$f12, OFFSET_FP12(k0)
    ldc1		$f13, OFFSET_FP13(k0)
    ldc1		$f14, OFFSET_FP14(k0)
    ldc1		$f15, OFFSET_FP15(k0)
    ldc1		$f16, OFFSET_FP16(k0)
    ldc1		$f17, OFFSET_FP17(k0)
    ldc1		$f18, OFFSET_FP18(k0)
    ldc1		$f19, OFFSET_FP19(k0)
    ldc1		$f20, OFFSET_FP20(k0)
    ldc1		$f21, OFFSET_FP21(k0)
    ldc1		$f22, OFFSET_FP22(k0)
    ldc1		$f23, OFFSET_FP23(k0)
    ldc1		$f24, OFFSET_FP24(k0)
    ldc1		$f25, OFFSET_FP25(k0)
    ldc1		$f26, OFFSET_FP26(k0)
    ldc1		$f27, OFFSET_FP27(k0)
    ldc1		$f28, OFFSET_FP28(k0)
    ldc1		$f29, OFFSET_FP29(k0)
    ldc1		$f30, OFFSET_FP30(k0)
    ldc1		$f31, OFFSET_FP31(k0)
    .set noat
    lw	    k1, OFFSET_FPSTATUS(k0)
    ctc1    k1, $f31
#endif
    
    lw	    $1, OFFSET_R1(k0)
    lw	    $2, OFFSET_R2(k0)
    lw	    $3, OFFSET_R3(k0)
    lw	    $4, OFFSET_R4(k0)
    lw	    $5, OFFSET_R5(k0)
    lw	    $6, OFFSET_R6(k0)
    lw	    $7, OFFSET_R7(k0)
    lw	    $8, OFFSET_R8(k0)
    lw	    $9, OFFSET_R9(k0)
    lw	    $10, OFFSET_R10(k0)
    lw	    $11, OFFSET_R11(k0)
    lw	    $12, OFFSET_R12(k0)
    lw	    $13, OFFSET_R13(k0)
    lw	    $14, OFFSET_R14(k0)
    lw	    $15, OFFSET_R15(k0)
    lw	    $16, OFFSET_R16(k0)
    lw	    $17, OFFSET_R17(k0)
    lw	    $18, OFFSET_R18(k0)
    lw	    $19, OFFSET_R19(k0)
    lw	    $20, OFFSET_R20(k0)
    lw	    $21, OFFSET_R21(k0)
    lw	    $22, OFFSET_R22(k0)
    lw	    $23, OFFSET_R23(k0)
    lw	    $24, OFFSET_R24(k0)
    lw	    $25, OFFSET_R25(k0)
    lw	    $28, OFFSET_R28(k0)
    lw	    $30, OFFSET_R30(k0)
    lw	    $31, OFFSET_R31(k0)
    
    // restore the critical regs from the stack
    lw	    k1, 4(sp)
    mtc0   k1, _CP0_STATUS
    
    lw	    k1, 0(sp)
    mtc0    k1, _CP0_EPC
    
    lw	    k0, 12(sp)
    lw	    k1, 8(sp)
    
    addiu   sp, sp, ISR_CONTEXT_SIZE
    ehb
  eret
  nop
  .end dispatch_task
    
.macro	isr vector,handler
    .global _\handler
    .equ     _\vector , _\handler
    .global  _\vector
    .section .\vector, code, keep
    .weak \handler
    eret
    nop

    .ent		_\handler
_\handler:
    /* Make room for the context. First save the current status so it can be
    manipulated, and the cause and EPC registers so their original values are
    captured. */
    addiu		sp, sp, -ISR_CONTEXT_SIZE
    sw			k0, 12(sp)
    sw			k1, 8(sp)
    mfc0		k1, _CP0_STATUS
    sw			k1, 4(sp)
    mfc0 		k0, _CP0_EPC
    sw			k0, 0(sp)
    
  la      k1, current_task
  lw	  k0, 0(k1)	    // first word is version which is sizeof(tcb_t)
  lh	  k1, 0(k0)	    // should be 64 bytes as of writing
  add	  k0, k1	    // offset to the state
    
    mflo   k1, $ac0
    sw	    k1, OFFSET_AC0(k0)
    mfhi    k1, $ac0
    sw	    k1, OFFSET_AC0+4(k0)
   
    mflo   k1, $ac1
    sw	    k1, OFFSET_AC1(k0)
    mfhi    k1, $ac1
    sw	    k1, OFFSET_AC1+4(k0)
   
    mflo   k1, $ac2
    sw	    k1, OFFSET_AC2(k0)
    mfhi    k1, $ac2
    sw	    k1, OFFSET_AC2+4(k0)
   
    mflo   k1, $ac3
    sw	    k1, OFFSET_AC3(k0)
    mfhi    k1, $ac3
    sw	    k1, OFFSET_AC3+4(k0)

#ifdef __PIC32_HAS_FPU64
    sdc1		$f0, OFFSET_FP0(k0)
    sdc1		$f1, OFFSET_FP1(k0)
    sdc1		$f2, OFFSET_FP2(k0)
    sdc1		$f3, OFFSET_FP3(k0)
    sdc1		$f4, OFFSET_FP4(k0)
    sdc1		$f5, OFFSET_FP5(k0)
    sdc1		$f6, OFFSET_FP6(k0)
    sdc1		$f7, OFFSET_FP7(k0)
    sdc1		$f8, OFFSET_FP8(k0)
    sdc1		$f9, OFFSET_FP9(k0)
    sdc1		$f10, OFFSET_FP10(k0)
    sdc1		$f11, OFFSET_FP11(k0)
    sdc1		$f12, OFFSET_FP12(k0)
    sdc1		$f13, OFFSET_FP13(k0)
    sdc1		$f14, OFFSET_FP14(k0)
    sdc1		$f15, OFFSET_FP15(k0)
    sdc1		$f16, OFFSET_FP16(k0)
    sdc1		$f17, OFFSET_FP17(k0)
    sdc1		$f18, OFFSET_FP18(k0)
    sdc1		$f19, OFFSET_FP19(k0)
    sdc1		$f20, OFFSET_FP20(k0)
    sdc1		$f21, OFFSET_FP21(k0)
    sdc1		$f22, OFFSET_FP22(k0)
    sdc1		$f23, OFFSET_FP23(k0)
    sdc1		$f24, OFFSET_FP24(k0)
    sdc1		$f25, OFFSET_FP25(k0)
    sdc1		$f26, OFFSET_FP26(k0)
    sdc1		$f27, OFFSET_FP27(k0)
    sdc1		$f28, OFFSET_FP28(k0)
    sdc1		$f29, OFFSET_FP29(k0)
    sdc1		$f30, OFFSET_FP30(k0)
    sdc1		$f31, OFFSET_FP31(k0)
    cfc1    k1, $f31
    sw	    k1, OFFSET_FPSTATUS(k0)
#endif
    
    sw	    $1, OFFSET_R1(k0)
    sw	    $2, OFFSET_R2(k0)
    sw	    $3, OFFSET_R3(k0)
    sw	    $4, OFFSET_R4(k0)
    sw	    $5, OFFSET_R5(k0)
    sw	    $6, OFFSET_R6(k0)
    sw	    $7, OFFSET_R7(k0)
    sw	    $8, OFFSET_R8(k0)
    sw	    $9, OFFSET_R9(k0)
    sw	    $10, OFFSET_R10(k0)
    sw	    $11, OFFSET_R11(k0)
    sw	    $12, OFFSET_R12(k0)
    sw	    $13, OFFSET_R13(k0)
    sw	    $14, OFFSET_R14(k0)
    sw	    $15, OFFSET_R15(k0)
    sw	    $16, OFFSET_R16(k0)
    sw	    $17, OFFSET_R17(k0)
    sw	    $18, OFFSET_R18(k0)
    sw	    $19, OFFSET_R19(k0)
    sw	    $20, OFFSET_R20(k0)
    sw	    $21, OFFSET_R21(k0)
    sw	    $22, OFFSET_R22(k0)
    sw	    $23, OFFSET_R23(k0)
    sw	    $24, OFFSET_R24(k0)
    sw	    $25, OFFSET_R25(k0)
    sw	    $28, OFFSET_R28(k0)
    sw	    $30, OFFSET_R30(k0)
    sw	    $31, OFFSET_R31(k0)
    
    // save the status pushed onto the stack
    lw	    k1, 12(sp)
    sw	    k1, OFFSET_R26(k0)
    lw	    k1, 8(sp)
    sw	    k1, OFFSET_R27(k0)
    lw	    k1, 4(sp)
    sw	    k1, OFFSET_STATUS(k0)
    lw	    k1, 0(sp)
    sw	    k1, OFFSET_EPC(k0)
    
    // loose the stack frame
    addi   sp, sp, ISR_CONTEXT_SIZE
    sw	    sp, OFFSET_R29(k0)
    
    /* s5 is used as the frame pointer. */
    add			s5, zero, sp

    // call the interrupt handler
    jal	\handler
    nop
    
    // and dispatch the next ready to run task
    j dispatch_task
    nop
    
    .end _\handler

    .endm

/******************************************************************/

    .set  nomips16
    .set  nomicromips
    .set  noreorder
    .set  noat
    
    isr vector_1, sw0_interrupt
    isr vector_4, t1_interrupt
    isr vector_6, ic1_interrupt
    isr vector_9, t2_interrupt
    isr vector_11, ic2_interrupt
    isr vector_14, t3_interrupt
    isr vector_16, ic3_interrupt
    isr vector_19, t4_interrupt
    isr vector_21, ic4_interrupt
    isr vector_14, t5_interrupt
    isr vector_26, ic5_interrupt
    isr vector_28, t6_interrupt
    isr vector_31, ic6_interrupt
    isr vector_32, t7_interrupt
    isr vector_34, ic7_interrupt
    isr vector_36, t8_interrupt
    isr vector_38, ic8_interrupt
    isr vector_40, t9_interrupt
    isr vector_42, ic9_interrupt
    isr vector_44, adc_interrupt
    isr vector_113, u1_rx_interrupt
    isr vector_114, u1_tx_interrupt
    isr vector_117, mi2c1_interrupt
    isr vector_118, cna_interrupt
    isr vector_119, cnb_interrupt
    isr vector_120, cnc_interrupt
    isr vector_121, cnd_interrupt
    isr vector_122, cne_interrupt
    isr vector_123, cnf_interrupt
    isr vector_124, cng_interrupt
    isr vector_125, cnh_interrupt
    isr vector_126, cnj_interrupt
    isr vector_127, cnk_interrupt
    isr vector_146, u2_rx_interrupt
    isr vector_147, u2_tx_interrupt
    isr vector_150, mi2c2_interrupt
    isr vector_151, can1_interrupt
    isr vector_152, can2_interrupt
    isr vector_158, u3_rx_interrupt
    isr vector_159, u3_tx_interrupt
    isr vector_162, mi2c3_interrupt
    isr vector_171, u4_rx_interrupt
    isr vector_172, u4_tx_interrupt
    isr vector_175, mi2c4_interrupt
    isr vector_180, u5_rx_interrupt
    isr vector_181, u5_tx_interrupt
    isr vector_184, mi2c5_interrupt
    isr vector_189, u6_rx_interrupt
    isr vector_190, u6_tx_interrupt
    isr vector_192, adc_eos_interrupt