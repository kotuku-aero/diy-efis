
OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(_reset)
/*
 * Provide for a minimum stack and heap size
 * - _min_stack_size - represents the minimum space that must be made
 *                     available for the stack.  Can be overridden from
 *                     the command line using the linker's --defsym option.
 * - _min_heap_size  - represents the minimum space that must be made
 *                     available for the heap.  Must be specified on
 *                     the command line using the linker's --defsym option.
 */
EXTERN (_min_stack_size _min_heap_size)
PROVIDE(_min_stack_size = 0x400) ;
PROVIDE(_min_heap_size = 0x400) ;

/*
we allow a 32byte bootstrap header that is added by the script
that builds an application image
 */
PROVIDE(_RESET_ADDR = 0x00000020);

MEMORY
{
    /* define a 384k memory area */
  user_mem_base     (rwx)  : ORIGIN = 0x00000020, LENGTH = 0x000fFFE0
}

SECTIONS
{
  /* Boot Sections */
  .reset _RESET_ADDR :
  {
    KEEP(*(.reset))
    KEEP(*(.reset.startup))
  } > user_mem_base
  /*  The startup code is in the .reset.startup section.
   *  Keep this here for backwards compatibility with older
   *  C32 v1.xx releases.
   */
  .startup  :
  {
    KEEP(*(.startup))
  } > user_mem_base

  /* Code Sections - Note that input sections *(.text) and *(.text.*)
   * are not mapped here. The best-fit allocator locates them,
   * so that .text may flow around absolute sections as needed.
   */
  .text :
  {
    *(.stub .gnu.linkonce.t.*)
    KEEP (*(.text.*personality*))
    *(.mips16.fn.*)
    *(.mips16.call.*)
    *(.gnu.warning)
    . = ALIGN(4) ;
  } >user_mem_base
  /* Global-namespace object initialization */
  .init   :
  {
    KEEP (*crti.o(.init))
    KEEP (*crtbegin.o(.init))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o *crtn.o ).init))
    KEEP (*crtend.o(.init))
    KEEP (*crtn.o(.init))
    . = ALIGN(4) ;
  } >user_mem_base
  .fini   :
  {
    KEEP (*(.fini))
    . = ALIGN(4) ;
  } >user_mem_base
  .preinit_array   :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4) ;
  } >user_mem_base
  .init_array   :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4) ;
  } >user_mem_base
  .fini_array   :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4) ;
  } >user_mem_base
  .ctors   :
  {
    /* XC32 uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
       the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    . = ALIGN(4) ;
  } >user_mem_base
  .dtors   :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    . = ALIGN(4) ;
  } >user_mem_base
  /* Read-only sections */
  .rodata   :
  {
    *( .gnu.linkonce.r.*)
    *(.rodata1)
    . = ALIGN(4) ;
  } >user_mem_base
  /*
   * Small initialized constant global and static data can be placed in the
   * .sdata2 section.  This is different from .sdata, which contains small
   * initialized non-constant global and static data.
   */
  .sdata2 ALIGN(4) :
  {
    *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
    . = ALIGN(4) ;
  } >user_mem_base
  /*
   * Uninitialized constant global and static data (i.e., variables which will
   * always be zero).  Again, this is different from .sbss, which contains
   * small non-initialized, non-constant global and static data.
   */
  .sbss2 ALIGN(4) :
  {
    *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*)
    . = ALIGN(4) ;
  } >user_mem_base
  .eh_frame_hdr   :
  {
    *(.eh_frame_hdr)
  } >user_mem_base
    . = ALIGN(4) ;
  .eh_frame   : ONLY_IF_RO
  {
    KEEP (*(.eh_frame))
  } >user_mem_base
    . = ALIGN(4) ;
  .gcc_except_table   : ONLY_IF_RO
  {
    *(.gcc_except_table .gcc_except_table.*)
  } >user_mem_base
    . = ALIGN(4) ;
  .jcr   :
  {
    KEEP (*(.jcr))
    . = ALIGN(4) ;
  } >user_mem_base
  /* Export the start of data memory for crt0_EF.S */
  _data_begin = .;
  /* LMA (flash address) of initialized data for crt0_EF.S to copy from */
  _data_init = LOADADDR(.jcr);
  .eh_frame    : ONLY_IF_RW
  {
    KEEP (*(.eh_frame))
  } >user_mem_base
    . = ALIGN(4) ;
  .gcc_except_table    : ONLY_IF_RW
  {
    *(.gcc_except_table .gcc_except_table.*)
  } >user_mem_base
    . = ALIGN(4) ;
  /*
   *  Note that input sections named .data* are not mapped here.
   *  The best-fit allocator locates them, so that they may flow
   *  around absolute sections as needed.
   */
  .data   :
  {
    *( .gnu.linkonce.d.*)
    SORT(CONSTRUCTORS)
    *(.data1)
    . = ALIGN(4) ;
  } >user_mem_base
  . = .;
  _gp = ALIGN(16) + 0x7ff0;
  .got ALIGN(4) :
  {
    *(.got.plt) *(.got)
    . = ALIGN(4) ;
  } >user_mem_base
  /*
   * We want the small data sections together, so single-instruction offsets
   * can access them all, and initialized data all before uninitialized, so
   * we can shorten the on-disk segment size.
   */
  .sdata ALIGN(4) :
  {
    _sdata_begin = . ;
    *(.sdata .sdata.* .gnu.linkonce.s.*)
    . = ALIGN(4) ;
    _sdata_end = . ;
  } >user_mem_base
  .lit8           :
  {
    *(.lit8)
  } >user_mem_base
  .lit4           :
  {
    *(.lit4)
  } >user_mem_base
  . = ALIGN (4) ;
  _data_end = . ;
  _bss_begin = . ;
  .sbss ALIGN(4) :
  {
    _sbss_begin = . ;
    *(.dynsbss)
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
    _sbss_end = . ;
    . = ALIGN(4) ;
  } >user_mem_base
  /*
   *  Align here to ensure that the .bss section occupies space up to
   *  _end.  Align after .bss to ensure correct alignment even if the
   *  .bss section disappears because there are no input sections.
   *
   *  Note that input sections named .bss* are no longer mapped here.
   *  The best-fit allocator locates them, so that they may flow
   *  around absolute sections as needed.
   *
   */
  .bss     :
  {
    *(.dynbss)
    *(COMMON)
   /* Align here to ensure that the .bss section occupies space up to
      _end.  Align after .bss to ensure correct alignment even if the
      .bss section disappears because there are no input sections. */
   . = ALIGN(. != 0 ? 4 : 1);
  } >user_mem_base
  . = ALIGN(4) ;
  _end = . ;
  _bss_end = . ;
  /*
   *  The heap and stack are best-fit allocated by the linker after other
   *  data and bss sections have been allocated.
   */
    /* The .pdr section belongs in the absolute section */
    /DISCARD/ : { *(.pdr) }
  .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
  .gptab.sbss : { *(.gptab.bss) *(.gptab.sbss) }
  .mdebug.abi32 0 : { KEEP(*(.mdebug.abi32)) }
  .mdebug.abiN32 0 : { KEEP(*(.mdebug.abiN32)) }
  .mdebug.abi64 0 : { KEEP(*(.mdebug.abi64)) }
  .mdebug.abiO64 0 : { KEEP(*(.mdebug.abiO64)) }
  .mdebug.eabi32 0 : { KEEP(*(.mdebug.eabi32)) }
  .mdebug.eabi64 0 : { KEEP(*(.mdebug.eabi64)) }
  .gcc_compiled_long32 : { KEEP(*(.gcc_compiled_long32)) }
  .gcc_compiled_long64 : { KEEP(*(.gcc_compiled_long64)) }
  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections used by GDB for source-level debugging.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *.elf(.debug) *(.debug) }
  .line           0 : { *.elf(.line) *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *.elf(.debug_srcinfo) *(.debug_srcinfo) }
  .debug_sfnames  0 : { *.elf(.debug_sfnames) *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *.elf(.debug_aranges) *(.debug_aranges) }
  .debug_pubnames 0 : { *.elf(.debug_pubnames) *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *.elf(.debug_info .gnu.linkonce.wi.*) *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *.elf(.debug_abbrev) *(.debug_abbrev) }
  .debug_line     0 : { *.elf(.debug_line) *(.debug_line) }
  .debug_frame    0 : { *.elf(.debug_frame) *(.debug_frame) }
  .debug_str      0 : { *.elf(.debug_str) *(.debug_str) }
  .debug_loc      0 : { *.elf(.debug_loc) *(.debug_loc) }
  .debug_macinfo  0 : { *.elf(.debug_macinfo) *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *.elf(.debug_weaknames) *(.debug_weaknames) }
  .debug_funcnames 0 : { *.elf(.debug_funcnames) *(.debug_funcnames) }
  .debug_typenames 0 : { *.elf(.debug_typenames) *(.debug_typenames) }
  .debug_varnames  0 : { *.elf(.debug_varnames) *(.debug_varnames) }
  .debug_pubtypes 0 : { *.elf(.debug_pubtypes) *(.debug_pubtypes) }
  .debug_ranges   0 : { *.elf(.debug_ranges) *(.debug_ranges) }
  /DISCARD/ : { *(.rel.dyn) }
  .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
  /DISCARD/ : { *(.note.GNU-stack) }
  /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) *(.discard) }
}
