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
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#ifndef __ion_debugger_h__
#define __ion_debugger_h__

#include "../neutron/neutron.h"

#ifdef __cplusplus
extern "C" {
#endif

  static inline uint32_t create_index(uint16_t assm_index, uint16_t method_index) { return (assm_index << 16) | method_index; }
  static inline uint16_t get_assembly_index(uint32_t index) { return (uint16_t)(index >> 16); }
  static inline uint16_t get_method_index(uint32_t index) { return (uint16_t)index; }
  static inline uint16_t get_type_index(uint32_t index) { return (uint16_t)index; }
  static inline uint16_t get_field_index(uint32_t index) { return (uint16_t)index; }

  typedef struct _call_t
    {
    uint32_t m_index;
    uint32_t m_IP;
    } call_t;

  typedef struct _thread_stack_t
    {
    uint32_t m_pid;

    uint32_t m_num;
    uint32_t m_status;
    uint32_t m_flags;
    call_t m_data[0];
    } thread_stack_t;

  typedef struct debugging_thread_list_t {
      uint32_t m_num;
      uint32_t m_pids[0];
    } debugging_thread_list_t;


  typedef struct _debugging_value_t
    {
    void *m_referenceID;
    uint32_t m_dt;                // CLR_RT_HeapBlock::DataType ()
    uint32_t m_flags;             // CLR_RT_HeapBlock::DataFlags()
    uint32_t m_size;              // CLR_RT_HeapBlock::DataSize ()

    //
    // For primitive types
    //
    uint8_t m_builtinValue[128]; // Space for string preview...

    //
    // For DATATYPE_STRING
    //
    uint32_t m_bytesInString;
    const char *m_charsInString;

    //
    // For DATATYPE_VALUETYPE or DATATYPE_CLASSTYPE
    //
    uint32_t    m_td;

    //
    // For DATATYPE_SZARRAY
    //
    uint32_t              m_array_numOfElements;
    uint32_t              m_array_depth;
    uint32_t    m_array_typeIndex;

    //
    // For values from an array.
    //
    void *m_arrayref_referenceID;
    uint32_t m_arrayref_index;
    } debugging_value_t;

  typedef struct _execution_breakpoint_t {
#define c_STEP_IN 0x0001
#define c_STEP_OVER 0x0002
#define c_STEP_OUT 0x0004
#define c_HARD 0x0008
#define c_EXCEPTION_THROWN 0x0010
#define c_EXCEPTION_CAUGHT 0x0020
#define c_EXCEPTION_UNCAUGHT 0x0040
#define c_THREAD_TERMINATED 0x0080
#define c_THREAD_CREATED 0x0100
#define c_ASSEMBLIES_LOADED 0x0200
#define c_LAST_BREAKPOINT 0x0400
#define c_STEP_JMC 0x0800
#define c_BREAK 0x1000
#define c_EVAL_COMPLETE 0x2000
#define c_EXCEPTION_UNWIND 0x4000
#define c_EXCEPTION_FINALLY 0x8000

#define c_STEP (c_STEP_IN | c_STEP_OVER | c_STEP_OUT)

#define c_PID_ANY 0x7FFFFFFF

#define c_DEPTH_EXCEPTION_FIRST_CHANCE 0x00000000
#define c_DEPTH_EXCEPTION_USERS_CHANCE 0x00000001
#define c_DEPTH_EXCEPTION_HANDLER_FOUND 0x00000002

#define c_DEPTH_STEP_NORMAL 0x00000010
#define c_DEPTH_STEP_RETURN 0x00000020
#define c_DEPTH_STEP_CALL 0x00000030
#define c_DEPTH_STEP_EXCEPTION_FILTER 0x00000040
#define c_DEPTH_STEP_EXCEPTION_HANDLER 0x00000050
#define c_DEPTH_STEP_INTERCEPT 0x00000060
#define c_DEPTH_STEP_EXIT 0x00000070

#define c_DEPTH_UNCAUGHT 0xFFFFFFFF


    uint16_t             m_id;
    uint16_t             m_flags;

    int32_t              m_pid;
    uint32_t             m_depth;

    //
    // m_IPStart, m_IPEnd are used for optimizing stepping operations.
    // A STEP_IN | STEP_OVER breakpoint will be hit in the given stack frame
    // only if the IP is outside of the given range [m_IPStart m_IPEnd).
    //
    uint32_t             m_IPStart;
    uint32_t             m_IPEnd;

    uint32_t m_method_index;
    uint32_t             m_IP;

    uint32_t   m_type_index;

    uint32_t             m_depthExceptionHandler;
    } execution_breakpoint_t;

  typedef struct _stack_info_t {
    uint32_t m_md;
    uint32_t             m_IP;
    uint32_t             m_numOfArguments;
    uint32_t             m_numOfLocals;
    uint32_t             m_depthOfEvalStack;
    } stack_info_t;

  // reboot flags
#define c_NormalReboot 0
#define c_ClrOnly 2
#define c_WaitForDebugger 4

// Debugger conditions
#define c_fDebugger_StateInitialize 0x00000000
#define c_fDebugger_StateResolutionFailed 0x00000001
#define c_fDebugger_StateProgramRunning 0x00000400
#define c_fDebugger_StateProgramExited 0x00000800
#define c_fDebugger_StateMask c_fDebugger_StateProgramRunning + c_fDebugger_StateProgramExited
#define c_fDebugger_BreakpointsDisabled 0x00001000
#define c_fDebugger_Quiet 0x00010000 // Do not spew debug text to the debugger
#define c_fDebugger_ExitPending 0x00020000
#define c_fDebugger_PauseTimers 0x04000000 // Threads associated with timers are created in "suspended" mode.
#define c_fDebugger_NoCompaction 0x08000000 // Don't perform compaction during execution.
//
#define c_fDebugger_SourceLevelDebugging 0x10000000
#define c_fDebugger_RebootPending 0x20000000
#define c_fDebugger_Enabled 0x40000000
#define c_fDebugger_Stopped 0x80000000

  extern result_t debug_resolve_type(uint32_t type_id, char *buffer, size_t len);
  extern result_t debug_restart(uint32_t flags);                // restart the application
  extern result_t debug_set_execution_flags(uint32_t flags_to_set, uint32_t flags_to_reset);
  extern result_t debug_get_execution_flags(uint32_t *flags);
  extern result_t debug_create_list_of_threads(debugging_thread_list_t **reply);
  extern result_t debug_resolve_field(uint32_t fd, uint32_t *td, uint32_t *index, char *name, size_t len);
  extern result_t debug_resolve_method(uint32_t md, uint32_t *td, char *name, size_t len);
  extern result_t debug_resolve_virtual_method(uint32_t vmd, void *obj, uint32_t *td, char *name, size_t len, uint32_t *md);
  extern result_t debug_set_just_my_code_method(uint32_t index, bool is_it);
  extern result_t debug_set_just_my_code_type(uint32_t index, bool is_it);
  extern result_t debug_set_just_my_code_assembly(uint32_t index, bool is_it);
  extern result_t debug_thread_pause(uint32_t pid);
  extern result_t debug_thread_resume(uint32_t pid);
  extern result_t debug_thread_get_values(uint32_t pid, debugging_value_t *values, size_t count);
  extern result_t debug_exception_get_values(uint32_t pid, debugging_value_t *values, size_t count);
  extern result_t debug_thread_unwind(uint32_t pid, uint32_t depth);
  extern result_t debug_stack_info(uint32_t pid, uint32_t depth, stack_info_t *info);
  extern result_t debug_set_ip(uint32_t pid, uint32_t depth, uint32_t ip, uint32_t depthOfEvalStack);

#define c_Local 0
#define c_Argument 1
#define c_EvalStack 2

  extern result_t debug_get_value_stack(uint32_t pid, uint32_t depth, uint32_t kind, uint32_t index, debugging_value_t *values, size_t count);
  extern result_t debug_get_value_field(void *obj, uint32_t offset, uint32_t fd, debugging_value_t *values, size_t count);
  extern result_t debug_get_value_array(void *obj, uint32_t index, debugging_value_t *values, size_t count);
  extern result_t debug_get_value_block(void *obj, debugging_value_t *values, size_t count);
  extern result_t debug_get_value_scratchpad(uint32_t index, debugging_value_t *values, size_t count);
  extern result_t debug_set_scratchpad_size(uint32_t size);
  extern result_t debug_set_value_scratchpad(void *obj, uint32_t datatype, uint8_t *value, size_t value_len);
  extern result_t debug_set_value_scratchpad_array(void *obj, uint32_t index, uint8_t *value, size_t value_len);
  extern result_t debug_allocate_scratchpad_object(int index, uint32_t td, debugging_value_t *values, size_t count);
  extern result_t debug_allocate_scratchpad_string(int index, uint32_t size, debugging_value_t *values, size_t count);
  extern result_t debug_allocate_scratchpad_array(int index, uint32_t td, uint32_t depth, uint32_t num_elem, debugging_value_t *values, size_t count);

  typedef struct _assembly_info_t {
    size_t version;
    const char *name;
    } assembly_info_t;

  extern result_t debug_value_assign(void *src, void *dst, debugging_value_t *values, size_t count);
  extern result_t debug_get_assembly(uint32_t index, assembly_info_t *assembly);
  // callbacks from the CLS system

  typedef struct _debugger_event_args_t {
    uint32_t version;
    uint32_t flags;         // reason(s) for the breakpoint
    execution_breakpoint_t *reason; // if a custom breakpoint this is the reason
    uint32_t pid;           // process
    uint32_t depth;
    uint32_t exception_handler_depth;
    // TODO: this should be CLR_RT_StackFrame
    void *stack;
    const uint8_t *ip;
    } debugger_event_args_t;

  extern result_t debug_break();
  extern result_t debug_stop(const debugger_event_args_t *args);
  extern result_t debug_breakpoint(const debugger_event_args_t *args);
  extern result_t debug_exception(const debugger_event_args_t *args);
  extern result_t debug_software_break(const debugger_event_args_t *args);
  extern result_t debug_program_exit(const debugger_event_args_t *args);
  extern result_t debug_restart_program(const debugger_event_args_t *args);


#ifdef __cplusplus
}
#endif

#endif
