/*
 * canfly_stubs.c - Atom OS User-Mode Syscall Stubs
 *
 * Auto-generated from syscalls.xml v1.0
 *
 * Each function marshals parameters into MIPS registers and executes
 * the SYSCALL instruction. The kernel handler dispatches based on
 * the syscall number in $v0.
 *
 * ABI: MIPS o32
 *   $v0       = syscall number (in) / return value (out)
 *   $a0..$a3  = first 4 parameters
 *   stack     = parameters 5+
 *
 * Copyright (c) Kotuku Aerospace Ltd. All rights reserved.
 */

#include "../../include/canfly.h"

// this file only compiles on a pic32mz processor.
#ifndef MSVC


/******************************************************************************/
/* Syscall Numbers                                                            */
/******************************************************************************/

/* kernel (Core kernel primitives: tasks, semaphores, queues, memory, time) */
#define SYSCALL_SYS_TICKS                                  1
#define SYSCALL_SYS_NOW                                    2
#define SYSCALL_SYS_CLOSE_HANDLE                           3
#define SYSCALL_SYS_NEUTRON_RANDOM                         4
#define SYSCALL_SYS_EXIT                                   5
#define SYSCALL_SYS_SUBSCRIBE                              6

/* config (Configuration database (registry) access. Public API uses cfg_* prefix.) */
#define SYSCALL_CFG_CREATE_KEY                             256
#define SYSCALL_CFG_OPEN_KEY                               257
#define SYSCALL_CFG_DELETE_KEY                             258
#define SYSCALL_CFG_DELETE_VALUE                           259
#define SYSCALL_CFG_ENUM_KEY                               260
#define SYSCALL_CFG_QUERY_MEMID                            261
#define SYSCALL_CFG_RENAME_VALUE                           262
#define SYSCALL_CFG_RENAME_KEY                             263
#define SYSCALL_CFG_GET_BOOL                               270
#define SYSCALL_CFG_SET_BOOL                               271
#define SYSCALL_CFG_GET_INT8                               272
#define SYSCALL_CFG_SET_INT8                               273
#define SYSCALL_CFG_GET_UINT8                              274
#define SYSCALL_CFG_SET_UINT8                              275
#define SYSCALL_CFG_GET_INT16                              276
#define SYSCALL_CFG_SET_INT16                              277
#define SYSCALL_CFG_GET_UINT16                             278
#define SYSCALL_CFG_SET_UINT16                             279
#define SYSCALL_CFG_GET_INT32                              280
#define SYSCALL_CFG_SET_INT32                              281
#define SYSCALL_CFG_GET_UINT32                             282
#define SYSCALL_CFG_SET_UINT32                             283
#define SYSCALL_CFG_GET_FLOAT                              284
#define SYSCALL_CFG_SET_FLOAT                              285
#define SYSCALL_CFG_GET_STRING                             286
#define SYSCALL_CFG_SET_STRING                             287
#define SYSCALL_CFG_GET_LLA                                288
#define SYSCALL_CFG_SET_LLA                                289
#define SYSCALL_CFG_GET_XYZ                                290
#define SYSCALL_CFG_SET_XYZ                                291
#define SYSCALL_CFG_GET_MATRIX                             292
#define SYSCALL_CFG_SET_MATRIX                             293
#define SYSCALL_CFG_GET_QTN                                294
#define SYSCALL_CFG_SET_QTN                                295
#define SYSCALL_CFG_GET_UTC                                296
#define SYSCALL_CFG_SET_UTC                                297

/* can (CAN bus message creation, extraction, and transmission) */
#define SYSCALL_SYS_CAN_SEND                               512
#define SYSCALL_SYS_GET_DEVICE_ID                          513
#define SYSCALL_SYS_SET_DEVICE_ID                          514

/* stream (Stream, file, and directory operations) */
#define SYSCALL_SYS_STREAM_OPEN                            768
#define SYSCALL_SYS_STREAM_DELETE                          769
#define SYSCALL_SYS_STREAM_RENAME                          770
#define SYSCALL_SYS_STREAM_READ                            775
#define SYSCALL_SYS_STREAM_WRITE                           776
#define SYSCALL_SYS_STREAM_GETPOS                          777
#define SYSCALL_SYS_STREAM_SETPOS                          778
#define SYSCALL_SYS_STREAM_LENGTH                          779
#define SYSCALL_SYS_STREAM_TRUNCATE                        780
#define SYSCALL_SYS_STREAM_EOF                             781
#define SYSCALL_SYS_STREAM_COPY                            782
#define SYSCALL_SYS_CREATE_DIRECTORY                       800
#define SYSCALL_SYS_REMOVE_DIRECTORY                       801
#define SYSCALL_SYS_OPEN_DIRECTORY                         802
#define SYSCALL_SYS_READ_DIRECTORY                         803
#define SYSCALL_SYS_REWIND_DIRECTORY                       804
#define SYSCALL_SYS_STREAM_STAT                            805
#define SYSCALL_SYS_FREESPACE                              810
#define SYSCALL_SYS_TOTALSPACE                             811

/* photon (Photon 2D graphics drawing interface) */
#define SYSCALL_SYS_CANVAS_CREATE                          1280
#define SYSCALL_SYS_CANVAS_CREATE_BITMAP                   1281
#define SYSCALL_SYS_CANVAS_CREATE_PNG                      1282
#define SYSCALL_SYS_CANVAS_EXTENTS                         1283
#define SYSCALL_SYS_POLYLINE                               1290
#define SYSCALL_SYS_ELLIPSE                                1291
#define SYSCALL_SYS_POLYGON                                1292
#define SYSCALL_SYS_RECTANGLE                              1293
#define SYSCALL_SYS_ROUND_RECT                             1294
#define SYSCALL_SYS_ARC                                    1295
#define SYSCALL_SYS_PIE                                    1296
#define SYSCALL_SYS_BIT_BLT                                1297
#define SYSCALL_SYS_GET_PIXEL                              1298
#define SYSCALL_SYS_SET_PIXEL                              1299
#define SYSCALL_SYS_DRAW_TEXT                              1310
#define SYSCALL_SYS_TEXT_EXTENT                            1311
#define SYSCALL_SYS_ROTATE_POINT                           1312
#define SYSCALL_SYS_REGISTER_TOUCH_WINDOW                  1313
#define SYSCALL_SYS_IS_WINDOW_TAP                          1314
#define SYSCALL_SYS_GET_TOUCH_MSG                          1315

/* window (Window creation, management, and message dispatch) */
#define SYSCALL_SYS_WINDOW_CREATE                          1536
#define SYSCALL_SYS_GET_SCREEN                             1537
#define SYSCALL_SYS_WINDOW_PARENT                          1540
#define SYSCALL_SYS_GET_FIRST_CHILD                        1541
#define SYSCALL_SYS_GET_NEXT_SIBLING                       1542
#define SYSCALL_SYS_GET_PREVIOUS_SIBLING                   1543
#define SYSCALL_SYS_SET_WNDDATA                            1550
#define SYSCALL_SYS_GET_WNDDATA                            1551
#define SYSCALL_SYS_GET_WINDOW_FLAGS                       1552
#define SYSCALL_SYS_SET_WINDOW_FLAGS                       1553
#define SYSCALL_SYS_WINDOW_GETPOS                          1554
#define SYSCALL_SYS_WINDOW_SETPOS                          1555
#define SYSCALL_SYS_WINDOW_RECT                            1556
#define SYSCALL_SYS_SHOW_WINDOW                            1560
#define SYSCALL_SYS_HIDE_WINDOW                            1561
#define SYSCALL_SYS_IS_VISIBLE                             1562
#define SYSCALL_SYS_ENABLE_WINDOW                          1563
#define SYSCALL_SYS_DISABLE_WINDOW                         1564
#define SYSCALL_SYS_IS_ENABLED                             1565
#define SYSCALL_SYS_INVALIDATE                             1566
#define SYSCALL_SYS_IS_INVALID                             1567
#define SYSCALL_SYS_GET_Z_ORDER                            1568
#define SYSCALL_SYS_SET_Z_ORDER                            1569
#define SYSCALL_SYS_BEGIN_PAINT                            1570
#define SYSCALL_SYS_END_PAINT                              1571
#define SYSCALL_SYS_POST_MESSAGE                           1580
#define SYSCALL_SYS_GET_MESSAGE                            1582
#define SYSCALL_SYS_GET_WNDPROC                            1583
#define SYSCALL_SYS_GET_FOCUSED_WINDOW                     1590
#define SYSCALL_SYS_SET_FOCUSED_WINDOW                     1591
#define SYSCALL_SYS_IS_FOCUSED                             1592
#define SYSCALL_SYS_GET_WINDOW_ID                          1595
#define SYSCALL_SYS_SET_WINDOW_ID                          1596
#define SYSCALL_SYS_GET_WINDOW_BY_ID                       1597
#define SYSCALL_SYS_GET_DISPLAY_BRIGHTNESS                 1650
#define SYSCALL_SYS_SET_DISPLAY_BRIGHTNESS                 1651
#define SYSCALL_SYS_GET_NEXT_ENABLED_WINDOW                1652
#define SYSCALL_SYS_GET_NEXT_TABSTOP_WINDOW                1653
#define SYSCALL_SYS_GET_PREVIOUS_TABSTOP_WINDOW            1654

/* map (Kernel-rendered map canvas API. The map renderer runs in kernel space
      with direct access to SDRAM tile cache. User-mode code provides position
      updates and display preferences; the kernel renders asynchronously and
      posts id_map_render_complete to the owning window when ready.) */
#define SYSCALL_SYS_MAP_CREATE_CANVAS                      1792
#define SYSCALL_SYS_MAP_RENDER_CANVAS                      1793
#define SYSCALL_SYS_MAP_UPDATE_POSITION                    1794
#define SYSCALL_SYS_MAP_SET_RANGE                          1795
#define SYSCALL_SYS_MAP_GET_RANGE                          1796
#define SYSCALL_SYS_MAP_SET_MODE                           1797
#define SYSCALL_SYS_MAP_GET_MODE                           1798
#define SYSCALL_SYS_MAP_SET_LAYER_VISIBLE                  1799
#define SYSCALL_SYS_MAP_GET_LAYER_VISIBLE                  1800
#define SYSCALL_SYS_MAP_PAN                                1801
#define SYSCALL_SYS_MAP_ZOOM                               1802
#define SYSCALL_SYS_MAP_SCREEN_TO_POSITION                 1803
#define SYSCALL_SYS_MAP_POSITION_TO_SCREEN                 1804

/* system () */
#define SYSCALL_SYS_GET_AIRCRAFT                           2048
#define SYSCALL_SYS_SET_AIRCRAFT                           2049
#define SYSCALL_SYS_TRACE_MESSAGE                          2050

/* fms () */
#define SYSCALL_FMS_ENUM_FLIGHT_PLANS                      2560
#define SYSCALL_FMS_GET_ACTIVE_FLIGHTPLAN                  2561
#define SYSCALL_FMS_SET_ACTIVE_FLIGHTPLAN_NUMBER           2562
#define SYSCALL_FMS_INVERT_ACTIVE_FLIGHTPLAN               2563
#define SYSCALL_FMS_ENUM_FLIGHT_PLAN                       2564
#define SYSCALL_FMS_ENUM_WAYPOINTS                         2565
#define SYSCALL_FMS_ENUM_NEAREST                           2566
#define SYSCALL_FMS_GOTO_WAYPOINT                          2567
#define SYSCALL_FMS_ADD_WAYPOINT                           2568
#define SYSCALL_FMS_DELETE_WAYPOINT                        2569
#define SYSCALL_FMS_DELETE_FLIGHTPLAN                      2570
#define SYSCALL_FMS_ADD_FLIGHTPLAN                         2571

#define SYSCALL_MAX 2571

/******************************************************************************/
/* kernel stubs                                                               */
/******************************************************************************/

result_t sys_ticks(uint32_t* value)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)value;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_TICKS), "r"(__p0)
  );

  return __result;
}

result_t sys_now(tm_t* tm)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)tm;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_NOW), "r"(__p0)
  );

  return __result;
}

result_t sys_close_handle(handle_t hndl)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hndl;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_CLOSE_HANDLE), "r"(__p0)
  );

  return __result;
}

result_t sys_neutron_random(uint32_t seed, uint32_t* rand)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)seed;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)rand;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_NEUTRON_RANDOM), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_exit(result_t result)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)result;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_EXIT), "r"(__p0)
  );

  return __result;
}

result_t sys_subscribe(uint16_t first_id, uint16_t last_id, handle_t hwnd, handle_t* hndl)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)first_id;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)last_id;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)hwnd;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)hndl;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_SUBSCRIBE), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

/******************************************************************************/
/* config stubs                                                               */
/******************************************************************************/

result_t cfg_create_key(memid_t parent, const char* name, memid_t* key, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)key;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_CREATE_KEY), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_open_key(memid_t parent, const char* name, memid_t* key, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)key;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_OPEN_KEY), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_delete_key(memid_t key, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)key;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_DELETE_KEY), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t cfg_delete_value(memid_t parent, const char* name, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_DELETE_VALUE), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t cfg_enum_key(memid_t key, field_datatype* type, uint16_t* length, void* data, uint16_t len, char* name, memid_t* child, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)key;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)type;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)length;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)data;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 16 bytes for 4 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -16\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "sw %8, 8($sp)\n\t"
    "sw %9, 12($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 16\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_ENUM_KEY), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)len), "r"((uint32_t)name), "r"((uint32_t)child), "r"((uint32_t)overlapped)
  );

  return __result;
}

result_t cfg_query_memid(memid_t entry, field_datatype* type, char* name, uint16_t* length, memid_t* parent, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)entry;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)type;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)name;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)length;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 8 bytes for 2 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -8\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 8\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_QUERY_MEMID), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)parent), "r"((uint32_t)overlapped)
  );

  return __result;
}

result_t cfg_rename_value(memid_t parent, const char* name, const char* new_name, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)new_name;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_RENAME_VALUE), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_rename_key(memid_t key, const char* new_name, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)key;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)new_name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_RENAME_KEY), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t cfg_get_bool(memid_t parent, const char* name, bool* result, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)result;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_BOOL), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_bool(memid_t parent, const char* name, bool value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_BOOL), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_int8(memid_t parent, const char* name, int8_t* result, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)result;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_INT8), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_int8(memid_t parent, const char* name, int8_t value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_INT8), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_uint8(memid_t parent, const char* name, uint8_t* result, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)result;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_UINT8), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_uint8(memid_t parent, const char* name, uint8_t value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_UINT8), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_int16(memid_t parent, const char* name, int16_t* result, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)result;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_INT16), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_int16(memid_t parent, const char* name, int16_t value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_INT16), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_uint16(memid_t parent, const char* name, uint16_t* result, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)result;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_UINT16), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_uint16(memid_t parent, const char* name, uint16_t value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_UINT16), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_int32(memid_t parent, const char* name, int32_t* result, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)result;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_INT32), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_int32(memid_t parent, const char* name, int32_t value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_INT32), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_uint32(memid_t parent, const char* name, uint32_t* result, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)result;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_UINT32), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_uint32(memid_t parent, const char* name, uint32_t value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_UINT32), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_float(memid_t parent, const char* name, float* result, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)result;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_FLOAT), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_float(memid_t parent, const char* name, float value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_FLOAT), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_string(memid_t parent, const char* name, char* value, uint16_t* length, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)length;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 4 bytes for 1 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -4\n\t"
    "sw %6, 0($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 4\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_STRING), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)overlapped)
  );

  return __result;
}

result_t cfg_set_string(memid_t parent, const char* name, const char* value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_STRING), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_lla(memid_t parent, const char* name, lla_t* result, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)result;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_LLA), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_lla(memid_t parent, const char* name, const lla_t* value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_LLA), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_xyz(memid_t parent, const char* name, xyz_t* result, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)result;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_XYZ), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_xyz(memid_t parent, const char* name, const xyz_t* value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_XYZ), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_matrix(memid_t parent, const char* name, matrix_t* result, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)result;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_MATRIX), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_matrix(memid_t parent, const char* name, const matrix_t* value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_MATRIX), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_qtn(memid_t parent, const char* name, qtn_t* result, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)result;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_QTN), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_qtn(memid_t parent, const char* name, const qtn_t* value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_QTN), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_get_utc(memid_t parent, const char* name, tm_t* value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_GET_UTC), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t cfg_set_utc(memid_t parent, const char* name, const tm_t* value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)value;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_CFG_SET_UTC), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

/******************************************************************************/
/* can stubs                                                                  */
/******************************************************************************/

result_t sys_can_send(const canmsg_t* msg, uint32_t timeout, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)msg;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)timeout;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_CAN_SEND), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_get_device_id(uint8_t* id)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)id;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_DEVICE_ID), "r"(__p0)
  );

  return __result;
}

result_t sys_set_device_id(uint8_t id)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)id;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_SET_DEVICE_ID), "r"(__p0)
  );

  return __result;
}

/******************************************************************************/
/* stream stubs                                                               */
/******************************************************************************/

result_t sys_stream_open(const char* path, uint32_t flags, handle_t* stream, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)path;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)flags;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)stream;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_STREAM_OPEN), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t sys_stream_delete(const char* path, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)path;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_STREAM_DELETE), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_stream_rename(const char* old_filename, const char* new_filename, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)old_filename;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)new_filename;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_STREAM_RENAME), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_stream_read(handle_t stream, void* buffer, uint32_t size, uint32_t* read, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)stream;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)buffer;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)size;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)read;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 4 bytes for 1 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -4\n\t"
    "sw %6, 0($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 4\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_STREAM_READ), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)overlapped)
  );

  return __result;
}

result_t sys_stream_write(handle_t stream, const void* buffer, uint32_t size, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)stream;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)buffer;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)size;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_STREAM_WRITE), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t sys_stream_getpos(handle_t stream, uint32_t* pos)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)stream;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)pos;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_STREAM_GETPOS), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_stream_setpos(handle_t stream, int32_t pos, uint32_t whence)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)stream;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)pos;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)whence;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_STREAM_SETPOS), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_stream_length(handle_t stream, uint32_t* length)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)stream;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)length;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_STREAM_LENGTH), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_stream_truncate(handle_t stream, uint32_t length, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)stream;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)length;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_STREAM_TRUNCATE), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_stream_eof(handle_t stream)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)stream;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_STREAM_EOF), "r"(__p0)
  );

  return __result;
}

result_t sys_stream_copy(handle_t from, handle_t to, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)from;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)to;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_STREAM_COPY), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_create_directory(const char* path, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)path;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_CREATE_DIRECTORY), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_remove_directory(const char* path, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)path;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_REMOVE_DIRECTORY), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_open_directory(const char* dirname, handle_t* dirp, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)dirname;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)dirp;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_OPEN_DIRECTORY), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_read_directory(handle_t dirp, stat_t* stat, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)dirp;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)stat;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_READ_DIRECTORY), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_rewind_directory(handle_t dirp, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)dirp;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_REWIND_DIRECTORY), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_stream_stat(const char* path, stat_t* st, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)path;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)st;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_STREAM_STAT), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_freespace(const char* path, uint32_t* space)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)path;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)space;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_FREESPACE), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_totalspace(const char* path, uint32_t* space)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)path;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)space;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_TOTALSPACE), "r"(__p0), "r"(__p1)
  );

  return __result;
}

/******************************************************************************/
/* photon stubs                                                               */
/******************************************************************************/

result_t sys_canvas_create(const extent_t* size, handle_t* hndl)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)size;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)hndl;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_CANVAS_CREATE), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_canvas_create_bitmap(const bitmap_t* bitmap, handle_t* hndl)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)bitmap;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)hndl;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_CANVAS_CREATE_BITMAP), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_canvas_create_png(handle_t stream, handle_t* hndl)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)stream;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)hndl;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_CANVAS_CREATE_PNG), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_canvas_extents(handle_t canvas, extent_t* ex)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)ex;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_CANVAS_EXTENTS), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_polyline(handle_t canvas, const rect_t* clip_rect, color_t pen, uint32_t count, const point_t* points)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)clip_rect;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)pen;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)count;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 4 bytes for 1 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -4\n\t"
    "sw %6, 0($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 4\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_POLYLINE), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)points)
  );

  return __result;
}

result_t sys_ellipse(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)clip_rect;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)pen;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)fill;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 4 bytes for 1 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -4\n\t"
    "sw %6, 0($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 4\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_ELLIPSE), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)area)
  );

  return __result;
}

result_t sys_polygon(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, uint32_t count, const point_t* points)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)clip_rect;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)pen;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)fill;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 8 bytes for 2 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -8\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 8\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_POLYGON), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)count), "r"((uint32_t)points)
  );

  return __result;
}

result_t sys_rectangle(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)clip_rect;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)pen;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)fill;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 4 bytes for 1 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -4\n\t"
    "sw %6, 0($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 4\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_RECTANGLE), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)area)
  );

  return __result;
}

result_t sys_round_rect(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area, gdi_dim_t radius)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)clip_rect;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)pen;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)fill;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 8 bytes for 2 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -8\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 8\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_ROUND_RECT), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)area), "r"((uint32_t)radius)
  );

  return __result;
}

result_t sys_arc(handle_t canvas, const rect_t* clip_rect, color_t pen, const point_t* pt, gdi_dim_t radius, int16_t start, int16_t end)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)clip_rect;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)pen;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)pt;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 12 bytes for 3 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -12\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "sw %8, 8($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 12\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_ARC), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)radius), "r"((uint32_t)start), "r"((uint32_t)end)
  );

  return __result;
}

result_t sys_pie(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const point_t* pt, int16_t start, int16_t end, gdi_dim_t radii, gdi_dim_t inner)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)clip_rect;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)pen;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)fill;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 20 bytes for 5 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -20\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "sw %8, 8($sp)\n\t"
    "sw %9, 12($sp)\n\t"
    "sw %10, 16($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 20\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_PIE), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)pt), "r"((uint32_t)start), "r"((uint32_t)end), "r"((uint32_t)radii), "r"((uint32_t)inner)
  );

  return __result;
}

result_t sys_bit_blt(handle_t canvas, const rect_t* clip_rect, const rect_t* dest_rect, handle_t src_canvas, const rect_t* src_clip_rect, const point_t* src_pt, raster_operation operation)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)clip_rect;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)dest_rect;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)src_canvas;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 12 bytes for 3 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -12\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "sw %8, 8($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 12\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_BIT_BLT), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)src_clip_rect), "r"((uint32_t)src_pt), "r"((uint32_t)operation)
  );

  return __result;
}

result_t sys_get_pixel(handle_t canvas, const rect_t* clip_rect, const point_t* pt, color_t* pix)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)clip_rect;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)pt;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)pix;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_PIXEL), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t sys_set_pixel(handle_t canvas, const rect_t* clip_rect, const point_t* pt, color_t c, color_t* pix)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)clip_rect;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)pt;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)c;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 4 bytes for 1 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -4\n\t"
    "sw %6, 0($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 4\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_SET_PIXEL), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)pix)
  );

  return __result;
}

result_t sys_draw_text(handle_t canvas, const rect_t* clip_rect, const font_t* font, color_t fg, color_t bg, uint16_t count, const char* str, const point_t* src_pt, const rect_t * txt_clip_rect, text_flags format, uint16_t * char_widths)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)clip_rect;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)font;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)fg;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 28 bytes for 7 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -28\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "sw %8, 8($sp)\n\t"
    "sw %9, 12($sp)\n\t"
    "sw %10, 16($sp)\n\t"
    "sw %11, 20($sp)\n\t"
    "sw %12, 24($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 28\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_DRAW_TEXT), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)bg), "r"((uint32_t)count), "r"((uint32_t)str), "r"((uint32_t)src_pt), "r"((uint32_t)txt_clip_rect), "r"((uint32_t)format), "r"((uint32_t)char_widths)
  );

  return __result;
}

result_t sys_text_extent(const font_t* font, uint16_t count, const char* str, extent_t* ex)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)font;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)count;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)str;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)ex;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_TEXT_EXTENT), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t sys_rotate_point(const point_t* center, int16_t angle, point_t* pt)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)center;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)angle;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)pt;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_ROTATE_POINT), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_register_touch_window(handle_t hndl, uint16_t flags)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hndl;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)flags;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_REGISTER_TOUCH_WINDOW), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_is_window_tap(handle_t hndl, const canmsg_t* msg, uint16_t blur)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hndl;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)msg;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)blur;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_IS_WINDOW_TAP), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_get_touch_msg(const canmsg_t* msg, touch_msg_t* touch_msg)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)msg;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)touch_msg;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_TOUCH_MSG), "r"(__p0), "r"(__p1)
  );

  return __result;
}

/******************************************************************************/
/* window stubs                                                               */
/******************************************************************************/

result_t sys_window_create(handle_t parent, const rect_t* bounds, wndproc_fn cb, void* wnddata, uint32_t flags, handle_t* hndl)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)parent;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)bounds;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)cb;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)wnddata;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 8 bytes for 2 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -8\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 8\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_WINDOW_CREATE), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)flags), "r"((uint32_t)hndl)
  );

  return __result;
}

result_t sys_get_screen(handle_t* hndl)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hndl;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_SCREEN), "r"(__p0)
  );

  return __result;
}

result_t sys_window_parent(handle_t hwnd, handle_t* parent)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)parent;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_WINDOW_PARENT), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_get_first_child(handle_t hwnd, handle_t* child)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)child;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_FIRST_CHILD), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_get_next_sibling(handle_t hwnd, handle_t* sibling)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)sibling;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_NEXT_SIBLING), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_get_previous_sibling(handle_t hwnd, handle_t* sibling)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)sibling;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_PREVIOUS_SIBLING), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_set_wnddata(handle_t hwnd, void* wnddata)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)wnddata;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_SET_WNDDATA), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_get_wnddata(handle_t hwnd, void** wnddata)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)wnddata;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_WNDDATA), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_get_window_flags(handle_t hwnd, uint32_t* flags)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)flags;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_WINDOW_FLAGS), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_set_window_flags(handle_t hwnd, uint32_t flags)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)flags;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_SET_WINDOW_FLAGS), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_window_getpos(handle_t hwnd, rect_t* pos)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)pos;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_WINDOW_GETPOS), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_window_setpos(handle_t hwnd, const rect_t* pos)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)pos;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_WINDOW_SETPOS), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_window_rect(handle_t hwnd, rect_t* rect)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)rect;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_WINDOW_RECT), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_show_window(handle_t hwnd)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_SHOW_WINDOW), "r"(__p0)
  );

  return __result;
}

result_t sys_hide_window(handle_t hwnd)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_HIDE_WINDOW), "r"(__p0)
  );

  return __result;
}

result_t sys_is_visible(handle_t hwnd)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_IS_VISIBLE), "r"(__p0)
  );

  return __result;
}

result_t sys_enable_window(handle_t hwnd)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_ENABLE_WINDOW), "r"(__p0)
  );

  return __result;
}

result_t sys_disable_window(handle_t hwnd)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_DISABLE_WINDOW), "r"(__p0)
  );

  return __result;
}

result_t sys_is_enabled(handle_t hwnd)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_IS_ENABLED), "r"(__p0)
  );

  return __result;
}

result_t sys_invalidate(handle_t hwnd)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_INVALIDATE), "r"(__p0)
  );

  return __result;
}

result_t sys_is_invalid(handle_t hwnd)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_IS_INVALID), "r"(__p0)
  );

  return __result;
}

result_t sys_get_z_order(handle_t hwnd, uint8_t* value)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)value;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_Z_ORDER), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_set_z_order(handle_t hwnd, uint8_t value)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)value;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_SET_Z_ORDER), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_begin_paint(handle_t hwnd, handle_t* canvas)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)canvas;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_BEGIN_PAINT), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_end_paint(handle_t hwnd)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_END_PAINT), "r"(__p0)
  );

  return __result;
}

result_t sys_post_message(handle_t hwnd, const canmsg_t* msg, uint32_t delay)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)msg;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)delay;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_POST_MESSAGE), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_get_message(handle_t* hwnd, canmsg_t* msg)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)msg;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_MESSAGE), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_get_wndproc(handle_t hwnd, wndproc_fn* wndproc, void** wnddata)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)wndproc;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)wnddata;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_WNDPROC), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_get_focused_window(handle_t* hndl)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hndl;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_FOCUSED_WINDOW), "r"(__p0)
  );

  return __result;
}

result_t sys_set_focused_window(handle_t hndl)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hndl;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_SET_FOCUSED_WINDOW), "r"(__p0)
  );

  return __result;
}

result_t sys_is_focused(handle_t hwnd)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_IS_FOCUSED), "r"(__p0)
  );

  return __result;
}

result_t sys_get_window_id(handle_t hndl, uint8_t* id)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hndl;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)id;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_WINDOW_ID), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_set_window_id(handle_t hndl, uint8_t id)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hndl;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)id;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_SET_WINDOW_ID), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_get_window_by_id(handle_t hwnd, uint8_t id, handle_t* child)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)id;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)child;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_WINDOW_BY_ID), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_get_display_brightness(uint16_t* value)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)value;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_DISPLAY_BRIGHTNESS), "r"(__p0)
  );

  return __result;
}

result_t sys_set_display_brightness(uint16_t value)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)value;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_SET_DISPLAY_BRIGHTNESS), "r"(__p0)
  );

  return __result;
}

result_t sys_get_next_enabled_window(handle_t hwnd, handle_t* next)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)next;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_NEXT_ENABLED_WINDOW), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_get_next_tabstop_window(handle_t hwnd, handle_t* next)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)next;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_NEXT_TABSTOP_WINDOW), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_get_previous_tabstop_window(handle_t hwnd, handle_t* prev)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)prev;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_PREVIOUS_TABSTOP_WINDOW), "r"(__p0), "r"(__p1)
  );

  return __result;
}

/******************************************************************************/
/* map stubs                                                                  */
/******************************************************************************/

result_t sys_map_create_canvas(handle_t hwnd, const extent_t * extents, const map_theme_t * theme, handle_t* canvas)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)hwnd;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)extents;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)theme;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)canvas;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_CREATE_CANVAS), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t sys_map_render_canvas(handle_t canvas, const rect_t * dest, handle_t surface)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)dest;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)surface;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_RENDER_CANVAS), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_map_update_position(handle_t canvas, const lla_t* position, int32_t heading, int32_t track)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)position;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)heading;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)track;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_UPDATE_POSITION), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3)
  );

  return __result;
}

result_t sys_map_set_range(handle_t canvas, uint32_t range_mtrs1000)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)range_mtrs1000;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_SET_RANGE), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_map_get_range(handle_t canvas, uint32_t* range_mtrs1000)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)range_mtrs1000;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_GET_RANGE), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_map_set_mode(handle_t canvas, map_display_mode mode)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)mode;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_SET_MODE), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_map_get_mode(handle_t canvas, map_display_mode* mode)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)mode;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_GET_MODE), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_map_set_layer_visible(handle_t canvas, uint32_t layer_id)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)layer_id;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_SET_LAYER_VISIBLE), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_map_get_layer_visible(handle_t canvas, uint32_t* layer_id)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)layer_id;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_GET_LAYER_VISIBLE), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_map_pan(handle_t canvas, const extent_t * move_by)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)move_by;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_PAN), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_map_zoom(handle_t canvas, int32_t zoom_by)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)zoom_by;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_ZOOM), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_map_screen_to_position(handle_t canvas, const point_t* screen, lla_t* position)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)screen;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)position;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_SCREEN_TO_POSITION), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t sys_map_position_to_screen(handle_t canvas, const lla_t* position, point_t* screen)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)canvas;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)position;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)screen;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_MAP_POSITION_TO_SCREEN), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

/******************************************************************************/
/* system stubs                                                               */
/******************************************************************************/

result_t sys_get_aircraft(aircraft_t* value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)value;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_GET_AIRCRAFT), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_set_aircraft(const aircraft_t* value, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)value;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_SET_AIRCRAFT), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t sys_trace_message(uint16_t level, const char* msg)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)level;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)msg;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_SYS_TRACE_MESSAGE), "r"(__p0), "r"(__p1)
  );

  return __result;
}

/******************************************************************************/
/* fms stubs                                                                  */
/******************************************************************************/

result_t fms_enum_flight_plans(handle_t* handle, char* name, char* comment, char* description, char* type, bool* active, uint32_t* cookie, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)handle;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)comment;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)description;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 16 bytes for 4 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -16\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "sw %8, 8($sp)\n\t"
    "sw %9, 12($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 16\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_FMS_ENUM_FLIGHT_PLANS), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)type), "r"((uint32_t)active), "r"((uint32_t)cookie), "r"((uint32_t)overlapped)
  );

  return __result;
}

result_t fms_get_active_flightplan(uint32_t* cookie, char* name, char* comment, char* description, char* type, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)cookie;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)name;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)comment;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)description;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 8 bytes for 2 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -8\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 8\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_FMS_GET_ACTIVE_FLIGHTPLAN), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)type), "r"((uint32_t)overlapped)
  );

  return __result;
}

result_t fms_set_active_flightplan_number(uint32_t cookie, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)cookie;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_FMS_SET_ACTIVE_FLIGHTPLAN_NUMBER), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t fms_invert_active_flightplan(uint32_t* cookie, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)cookie;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_FMS_INVERT_ACTIVE_FLIGHTPLAN), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t fms_enum_flight_plan(handle_t* handle, uint32_t cookie, uint32_t* id, uint32_t* sequence, lla_t* position, uint32_t* type, char* name, char* comment, char* description, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)handle;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)cookie;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)id;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)sequence;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 24 bytes for 6 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -24\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "sw %8, 8($sp)\n\t"
    "sw %9, 12($sp)\n\t"
    "sw %10, 16($sp)\n\t"
    "sw %11, 20($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 24\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_FMS_ENUM_FLIGHT_PLAN), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)position), "r"((uint32_t)type), "r"((uint32_t)name), "r"((uint32_t)comment), "r"((uint32_t)description), "r"((uint32_t)overlapped)
  );

  return __result;
}

result_t fms_enum_waypoints(handle_t* handle, const char * filter, uint32_t* id, lla_t* position, uint32_t* type, char* name, char* comment, char* description, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)handle;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)filter;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)id;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)position;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 20 bytes for 5 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -20\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "sw %8, 8($sp)\n\t"
    "sw %9, 12($sp)\n\t"
    "sw %10, 16($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 20\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_FMS_ENUM_WAYPOINTS), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)type), "r"((uint32_t)name), "r"((uint32_t)comment), "r"((uint32_t)description), "r"((uint32_t)overlapped)
  );

  return __result;
}

result_t fms_enum_nearest(handle_t* handle, const char * filter, uint32_t distance, uint32_t wpt_type_filter, uint32_t* id, lla_t* position, uint32_t* type, char* name, char* comment, char* description, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)handle;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)filter;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)distance;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)wpt_type_filter;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 28 bytes for 7 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -28\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "sw %8, 8($sp)\n\t"
    "sw %9, 12($sp)\n\t"
    "sw %10, 16($sp)\n\t"
    "sw %11, 20($sp)\n\t"
    "sw %12, 24($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 28\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_FMS_ENUM_NEAREST), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)id), "r"((uint32_t)position), "r"((uint32_t)type), "r"((uint32_t)name), "r"((uint32_t)comment), "r"((uint32_t)description), "r"((uint32_t)overlapped)
  );

  return __result;
}

result_t fms_goto_waypoint(uint32_t wpt, uint32_t autopilot_enable, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)wpt;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)autopilot_enable;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_FMS_GOTO_WAYPOINT), "r"(__p0), "r"(__p1), "r"(__p2)
  );

  return __result;
}

result_t fms_add_waypoint(const lla_t* position, uint32_t type, const char* name, const char* comment, const char* description, int32_t* id, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)position;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)type;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)name;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)comment;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 12 bytes for 3 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -12\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "sw %8, 8($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 12\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_FMS_ADD_WAYPOINT), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)description), "r"((uint32_t)id), "r"((uint32_t)overlapped)
  );

  return __result;
}

result_t fms_delete_waypoint(int32_t id, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)id;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_FMS_DELETE_WAYPOINT), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t fms_delete_flightplan(int32_t id, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)id;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)overlapped;

  __asm__ volatile (
    "li $v0, %1\n\t"
    "syscall\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_FMS_DELETE_FLIGHTPLAN), "r"(__p0), "r"(__p1)
  );

  return __result;
}

result_t fms_add_flightplan(const char* name, const char* comment, const char* description, const char* type, uint32_t len, const uint32_t* waypoints, uint32_t* cookie, overlapped_t * overlapped)
{
  register result_t __result __asm__("$v0");
  register uint32_t __p0 __asm__("$a0") = (uint32_t)name;
  register uint32_t __p1 __asm__("$a1") = (uint32_t)comment;
  register uint32_t __p2 __asm__("$a2") = (uint32_t)description;
  register uint32_t __p3 __asm__("$a3") = (uint32_t)type;

  /* Parameters beyond 4 are passed on the stack per MIPS o32 ABI */
  /* Allocate 16 bytes for 4 stack parameters */

  __asm__ volatile (
    "addiu $sp, $sp, -16\n\t"
    "sw %6, 0($sp)\n\t"
    "sw %7, 4($sp)\n\t"
    "sw %8, 8($sp)\n\t"
    "sw %9, 12($sp)\n\t"
    "li $v0, %1\n\t"
    "syscall\n\t"
    "addiu $sp, $sp, 16\n\t"
    : "=r"(__result)
    : "i"(SYSCALL_FMS_ADD_FLIGHTPLAN), "r"(__p0), "r"(__p1), "r"(__p2), "r"(__p3), "r"((uint32_t)len), "r"((uint32_t)waypoints), "r"((uint32_t)cookie), "r"((uint32_t)overlapped)
  );

  return __result;
}

#endif /* MSVC */