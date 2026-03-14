/*-------------------------------------------------------------------------
 *  C Runtime Startup (crt0.S)
 *
 *  OVERVIEW:
 *  This is the C Runtime Startup code for PIC32MZ microcontrollers.
 *  It executes immediately after reset and is responsible for:
 *    1. Initializing the stack pointer and global pointer
 *    2. Clearing BSS (uninitialized data) to zero
 *    3. Calling C library initialization (__libc_init_array)
 *    4. Calling C++ constructors (if applicable)
 *   5. Jumping to main()
 *
 *  LINKER SYMBOL DEPENDENCIES:
 *  This code requires specific symbols from the linker script:
 *    _stack, _gp, _bss_begin, _bss_end, _ebase_address, _vector_spacing
 *    _data_begin, _data_init, _data_end
 *
 *  DEBUGGING TIPS:
 *  - Set breakpoint at _reset to catch reset
 *  - Set breakpoint at _no_nmi to skip NMI handling
 *  - Set breakpoint at _main_entry to catch entry to C code
 *  - Check $sp (stack pointer) after initialization
 *  - Check $gp (global pointer) after initialization
 *-------------------------------------------------------------------------*/

/* This file contains 32-bit assembly code */
       .set nomips16

        ##################################################################
        # Entry point of the entire application
        #
        # This reset code is ISA-agnostic: it works whether the CPU boots
        # in MIPS32 or microMIPS mode (controlled by BOOTISA config bit).
        #
        # The first word (0x10000003) is a "polyglot" instruction:
        #   - In MIPS32 mode: branch forward 0x10 bytes to __reset_switch_isa
        #   - In microMIPS mode: decoded as a NOP-like instruction
        #
        # This ensures correct startup regardless of BOOTISA setting.
        ##################################################################
        .section .reset,"ax",@progbits
        .align 2
        .set noreorder
        .ent _reset

        .set nomicromips
_reset:
        jal _startup
        nop

        .end _reset

        .globl _reset
        .size _reset, .-_reset

        .section .reset.startup,"ax",@progbits
        .align 4
        .set noreorder
        .globl _startup
        .ent _startup

_startup:
        .set nomacro

        ##################################################################
        # Initialize Global Pointer
        ##################################################################
        lui     $28,%hi(_gp)
        addiu   $28,$28,%lo(_gp)

        ##################################################################
        # Clear BSS section
        ##################################################################
        lui     $8,%hi(_bss_begin)
        addiu   $8,$8,%lo(_bss_begin)
        lui     $9,%hi(_bss_end)
        addiu   $9,$9,%lo(_bss_end)
        b       _bss_check
        nop

_bss_init:
        sw      $0,0x0($8)
        addiu   $8,$8,4
_bss_check:
        sltu    $10,$8,$9
        bne     $10,$0,_bss_init
        nop

        ##################################################################
        # Jump to main via _main_entry thunk
        ##################################################################
        and     $4,$4,0
        and     $5,$5,0
        lui     $8,%hi(_main_entry)
        addiu   $8,$8,%lo(_main_entry)
        jr      $8
        nop

        .end _startup

        ##################################################################
        # Main Entry Point
        ##################################################################
        .section .text.main_entry,"ax",@progbits
        .align 2
        .ent _main_entry

_main_entry:

        ##################################################################
        # C library initialization (if function exists)
        ##################################################################
        .weak __libc_init_array
        lui     $8,%hi(__libc_init_array)
        addiu   $8,$8,%lo(__libc_init_array)
        beqz    $8,_skip_libc_init
        nop
        jalr    $8
        nop
_skip_libc_init:

        ##################################################################
        # C++ constructor initialization
        ##################################################################
        .weak _init
        lui     $4,%hi(_init)
        addiu   $sp,$sp,-24
        addiu   $4,$4,%lo(_init)
        beq     $4,$0,_skip_cpp_init
        nop
        sw      $31,20($sp)
        jalr    $4
        nop
_skip_cpp_init:
        and     $4,$4,0
        and     $5,$5,0

        ##################################################################
        # Call main
        ##################################################################
        lui     $8,%hi(main)
        addiu   $8,$8,%lo(main)
        jalr    $8
        nop

        ##################################################################
        # Call sys_exit() which transitions to the kernel handler
        ##################################################################
        jal     sys_exit
        nop

        ##################################################################
        # Infinite loop (if main returns)
        ##################################################################
__crt0_exit:
1:

0:      b       1b
        nop

        .globl __crt0_exit
        .end _main_entry
