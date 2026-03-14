#ifndef __sys_canfly_h__
#define __sys_canfly_h__
/*
 * Canfly OS Public SDK Header
 *
 * Auto-generated from syscalls.xml v1.0
 *
 * This header declares the complete user-mode API for applications
 * running on the Atom operating system. Functions declared here
 * are implemented as MIPS SYSCALL stubs that trap into the kernel.
 *
 * Copyright (c) Kotuku Aerospace Ltd. All rights reserved.
 */

// these are the data types shared between the kernel and user space.
#include "canfly_types.h"

#ifdef WIN32
// this will bring in the MSVC runtime emulation library
// must be on the library search path in the CMakeLists.txt
#pragma comment(lib, "w32_canfly.lib")
#define STDCALL __stdcall

#ifdef KRYPTON_LIB
#define SYSCALL __declspec(dllexport)
#else
#define SYSCALL __declspec(dllimport)
#endif
#else
// Linux: import symbols from shared library
#define SYSCALL __attribute__((visibility("default")))
#define STDCALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PIC32_BUILD
// call this on the emulated environment to start the kernel and
// support libraries running.
extern SYSCALL result_t STDCALL canfly_init(int argc, char **argv);
#endif


/*
 * CAN message subscription constraints:
 * - Maximum 8 subscription blocks per task (32 bytes)
 * - IDs 1500..2047 are reserved by neutron and cannot be subscribed
 * - Proton window events (1400..1499) are always delivered
 */
#define MAX_SUBSCRIPTION_BLOCKS 8
#define CANFLY_RESERVED_FIRST   1500
#define CANFLY_RESERVED_LAST    2047



/******************************************************************************/
/* Category: kernel - Core kernel primitives: tasks, semaphores, queues, memory, time */
/******************************************************************************/

/**
 * Return the 1kHz tick counter
 * @param value [out]  * 
 * @return result_t
 * @syscall 1
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_ticks(uint32_t* value);
#else
extern result_t sys_ticks(uint32_t* value);
#endif

static inline result_t ticks(uint32_t* value)
  {
  return sys_ticks(value);
  }

/**
 * Return the current UTC time based on GPS fix
 * @param tm [out]  * 
 * @return result_t
 * @syscall 2
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_now(tm_t* tm);
#else
extern result_t sys_now(tm_t* tm);
#endif

static inline result_t now(tm_t* tm)
  {
  return sys_now(tm);
  }

/**
 * Release all resources for a handle
 * @param hndl  * 
 * @return result_t
 * @syscall 3
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_close_handle(handle_t hndl);
#else
extern result_t sys_close_handle(handle_t hndl);
#endif

static inline result_t close_handle(handle_t hndl)
  {
  return sys_close_handle(hndl);
  }

/**
 * Generate a random number (uses hardware RNG if available)
 * @param seed  * 
 * @param rand [out]  * 
 * @return result_t
 * @syscall 4
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_neutron_random(uint32_t seed, uint32_t* rand);
#else
extern result_t sys_neutron_random(uint32_t seed, uint32_t* rand);
#endif

static inline result_t neutron_random(uint32_t seed, uint32_t* rand)
  {
  return sys_neutron_random(seed, rand);
  }

/**
 * Exit the current program
 * @param result  * 
 * @return result_t
 * @syscall 5
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_exit(result_t result);
#else
extern result_t sys_exit(result_t result);
#endif

/**
 * Subscribe to a block of CAN message IDs. Messages with IDs in
 * the range [first_id, last_id] inclusive will be posted to the
 * specified window's message queue.
 * 
 * Restrictions:
 * - first_id must be less than or equal to last_id
 * - Neither ID may be in the range 1500..2047 (reserved for pipes)
 * - Maximum 8 subscription blocks per task
 * 
 * Returns a handle that can be closed to unsubscribe.
 * @param first_id  * 
 * @param last_id  * 
 * @param hwnd  * 
 * @param hndl [out]  * 
 * @return result_t
 * @syscall 6
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_subscribe(uint16_t first_id, uint16_t last_id, handle_t hwnd, handle_t* hndl);
#else
extern result_t sys_subscribe(uint16_t first_id, uint16_t last_id, handle_t hwnd, handle_t* hndl);
#endif

static inline result_t subscribe(uint16_t first_id, uint16_t last_id, handle_t hwnd, handle_t* hndl)
  {
  return sys_subscribe(first_id, last_id, hwnd, hndl);
  }

/******************************************************************************/
/* Category: config - Configuration database (registry) access. Public API uses cfg_* prefix. */
/******************************************************************************/

/**
 * Create a configuration key (opens existing if present)
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param key [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 256
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_create_key(memid_t parent, const char* name, memid_t* key, overlapped_t * overlapped);
#else
extern result_t cfg_create_key(memid_t parent, const char* name, memid_t* key, overlapped_t * overlapped);
#endif

/**
 * Open an existing configuration key
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param key [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 257
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_open_key(memid_t parent, const char* name, memid_t* key, overlapped_t * overlapped);
#else
extern result_t cfg_open_key(memid_t parent, const char* name, memid_t* key, overlapped_t * overlapped);
#endif

/**
 * Remove a key, all sub-keys and values
 * @note This call may use overlapped I/O.
 * @param key  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 258
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_delete_key(memid_t key, overlapped_t * overlapped);
#else
extern result_t cfg_delete_key(memid_t key, overlapped_t * overlapped);
#endif

/**
 * Remove a value
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 259
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_delete_value(memid_t parent, const char* name, overlapped_t * overlapped);
#else
extern result_t cfg_delete_value(memid_t parent, const char* name, overlapped_t * overlapped);
#endif

/**
 * Enumerate children of a key
 * @note This call may use overlapped I/O.
 * @param key  * 
 * @param type [in,out]  * 
 * @param length [in,out]  * 
 * @param data [out]  * 
 * @param len  * 
 * @param name [out]  * 
 * @param child [in,out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 260
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_enum_key(memid_t key, field_datatype* type, uint16_t* length, void* data, uint16_t len, char* name, memid_t* child, overlapped_t * overlapped);
#else
extern result_t cfg_enum_key(memid_t key, field_datatype* type, uint16_t* length, void* data, uint16_t len, char* name, memid_t* child, overlapped_t * overlapped);
#endif

/**
 * Return information about a memid
 * @note This call may use overlapped I/O.
 * @param entry  * 
 * @param type [out]  * 
 * @param name [out]  * 
 * @param length [out]  * 
 * @param parent [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 261
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_query_memid(memid_t entry, field_datatype* type, char* name, uint16_t* length, memid_t* parent, overlapped_t * overlapped);
#else
extern result_t cfg_query_memid(memid_t entry, field_datatype* type, char* name, uint16_t* length, memid_t* parent, overlapped_t * overlapped);
#endif

/**
 * Rename a value
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param new_name  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 262
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_rename_value(memid_t parent, const char* name, const char* new_name, overlapped_t * overlapped);
#else
extern result_t cfg_rename_value(memid_t parent, const char* name, const char* new_name, overlapped_t * overlapped);
#endif

/**
 * Rename a key
 * @note This call may use overlapped I/O.
 * @param key  * 
 * @param new_name  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 263
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_rename_key(memid_t key, const char* new_name, overlapped_t * overlapped);
#else
extern result_t cfg_rename_key(memid_t key, const char* new_name, overlapped_t * overlapped);
#endif

/**
 * Read a bool from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param result [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 270
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_bool(memid_t parent, const char* name, bool* result, overlapped_t * overlapped);
#else
extern result_t cfg_get_bool(memid_t parent, const char* name, bool* result, overlapped_t * overlapped);
#endif

/**
 * Write a bool to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 271
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_bool(memid_t parent, const char* name, bool value, overlapped_t * overlapped);
#else
extern result_t cfg_set_bool(memid_t parent, const char* name, bool value, overlapped_t * overlapped);
#endif

/**
 * Read an int8_t from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param result [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 272
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_int8(memid_t parent, const char* name, int8_t* result, overlapped_t * overlapped);
#else
extern result_t cfg_get_int8(memid_t parent, const char* name, int8_t* result, overlapped_t * overlapped);
#endif

/**
 * Write an int8_t to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 273
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_int8(memid_t parent, const char* name, int8_t value, overlapped_t * overlapped);
#else
extern result_t cfg_set_int8(memid_t parent, const char* name, int8_t value, overlapped_t * overlapped);
#endif

/**
 * Read a uint8_t from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param result [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 274
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_uint8(memid_t parent, const char* name, uint8_t* result, overlapped_t * overlapped);
#else
extern result_t cfg_get_uint8(memid_t parent, const char* name, uint8_t* result, overlapped_t * overlapped);
#endif

/**
 * Write a uint8_t to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 275
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_uint8(memid_t parent, const char* name, uint8_t value, overlapped_t * overlapped);
#else
extern result_t cfg_set_uint8(memid_t parent, const char* name, uint8_t value, overlapped_t * overlapped);
#endif

/**
 * Read an int16_t from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param result [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 276
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_int16(memid_t parent, const char* name, int16_t* result, overlapped_t * overlapped);
#else
extern result_t cfg_get_int16(memid_t parent, const char* name, int16_t* result, overlapped_t * overlapped);
#endif

/**
 * Write an int16_t to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 277
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_int16(memid_t parent, const char* name, int16_t value, overlapped_t * overlapped);
#else
extern result_t cfg_set_int16(memid_t parent, const char* name, int16_t value, overlapped_t * overlapped);
#endif

/**
 * Read a uint16_t from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param result [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 278
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_uint16(memid_t parent, const char* name, uint16_t* result, overlapped_t * overlapped);
#else
extern result_t cfg_get_uint16(memid_t parent, const char* name, uint16_t* result, overlapped_t * overlapped);
#endif

/**
 * Write a uint16_t to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 279
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_uint16(memid_t parent, const char* name, uint16_t value, overlapped_t * overlapped);
#else
extern result_t cfg_set_uint16(memid_t parent, const char* name, uint16_t value, overlapped_t * overlapped);
#endif

/**
 * Read an int32_t from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param result [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 280
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_int32(memid_t parent, const char* name, int32_t* result, overlapped_t * overlapped);
#else
extern result_t cfg_get_int32(memid_t parent, const char* name, int32_t* result, overlapped_t * overlapped);
#endif

/**
 * Write an int32_t to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 281
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_int32(memid_t parent, const char* name, int32_t value, overlapped_t * overlapped);
#else
extern result_t cfg_set_int32(memid_t parent, const char* name, int32_t value, overlapped_t * overlapped);
#endif

/**
 * Read a uint32_t from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param result [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 282
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_uint32(memid_t parent, const char* name, uint32_t* result, overlapped_t * overlapped);
#else
extern result_t cfg_get_uint32(memid_t parent, const char* name, uint32_t* result, overlapped_t * overlapped);
#endif

/**
 * Write a uint32_t to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 283
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_uint32(memid_t parent, const char* name, uint32_t value, overlapped_t * overlapped);
#else
extern result_t cfg_set_uint32(memid_t parent, const char* name, uint32_t value, overlapped_t * overlapped);
#endif

/**
 * Read a float from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param result [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 284
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_float(memid_t parent, const char* name, float* result, overlapped_t * overlapped);
#else
extern result_t cfg_get_float(memid_t parent, const char* name, float* result, overlapped_t * overlapped);
#endif

/**
 * Write a float to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 285
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_float(memid_t parent, const char* name, float value, overlapped_t * overlapped);
#else
extern result_t cfg_set_float(memid_t parent, const char* name, float value, overlapped_t * overlapped);
#endif

/**
 * Read a string from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value [out]  * 
 * @param length [in,out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 286
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_string(memid_t parent, const char* name, char* value, uint16_t* length, overlapped_t * overlapped);
#else
extern result_t cfg_get_string(memid_t parent, const char* name, char* value, uint16_t* length, overlapped_t * overlapped);
#endif

/**
 * Write a string to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 287
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_string(memid_t parent, const char* name, const char* value, overlapped_t * overlapped);
#else
extern result_t cfg_set_string(memid_t parent, const char* name, const char* value, overlapped_t * overlapped);
#endif

/**
 * Read an lla_t from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param result [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 288
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_lla(memid_t parent, const char* name, lla_t* result, overlapped_t * overlapped);
#else
extern result_t cfg_get_lla(memid_t parent, const char* name, lla_t* result, overlapped_t * overlapped);
#endif

/**
 * Write an lla_t to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 289
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_lla(memid_t parent, const char* name, const lla_t* value, overlapped_t * overlapped);
#else
extern result_t cfg_set_lla(memid_t parent, const char* name, const lla_t* value, overlapped_t * overlapped);
#endif

/**
 * Read an xyz_t from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param result [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 290
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_xyz(memid_t parent, const char* name, xyz_t* result, overlapped_t * overlapped);
#else
extern result_t cfg_get_xyz(memid_t parent, const char* name, xyz_t* result, overlapped_t * overlapped);
#endif

/**
 * Write an xyz_t to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 291
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_xyz(memid_t parent, const char* name, const xyz_t* value, overlapped_t * overlapped);
#else
extern result_t cfg_set_xyz(memid_t parent, const char* name, const xyz_t* value, overlapped_t * overlapped);
#endif

/**
 * Read a matrix_t from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param result [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 292
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_matrix(memid_t parent, const char* name, matrix_t* result, overlapped_t * overlapped);
#else
extern result_t cfg_get_matrix(memid_t parent, const char* name, matrix_t* result, overlapped_t * overlapped);
#endif

/**
 * Write a matrix_t to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 293
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_matrix(memid_t parent, const char* name, const matrix_t* value, overlapped_t * overlapped);
#else
extern result_t cfg_set_matrix(memid_t parent, const char* name, const matrix_t* value, overlapped_t * overlapped);
#endif

/**
 * Read a qtn_t from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param result [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 294
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_qtn(memid_t parent, const char* name, qtn_t* result, overlapped_t * overlapped);
#else
extern result_t cfg_get_qtn(memid_t parent, const char* name, qtn_t* result, overlapped_t * overlapped);
#endif

/**
 * Write a qtn_t to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 295
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_qtn(memid_t parent, const char* name, const qtn_t* value, overlapped_t * overlapped);
#else
extern result_t cfg_set_qtn(memid_t parent, const char* name, const qtn_t* value, overlapped_t * overlapped);
#endif

/**
 * Read a UTC time from the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 296
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_get_utc(memid_t parent, const char* name, tm_t* value, overlapped_t * overlapped);
#else
extern result_t cfg_get_utc(memid_t parent, const char* name, tm_t* value, overlapped_t * overlapped);
#endif

/**
 * Write a UTC time to the config database
 * @note This call may use overlapped I/O.
 * @param parent  * 
 * @param name  * 
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 297
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL cfg_set_utc(memid_t parent, const char* name, const tm_t* value, overlapped_t * overlapped);
#else
extern result_t cfg_set_utc(memid_t parent, const char* name, const tm_t* value, overlapped_t * overlapped);
#endif

/******************************************************************************/
/* Category: can - CAN bus message creation, extraction, and transmission     */
/******************************************************************************/

/**
 * Queue a CAN message to be sent
 * @note This call may use overlapped I/O.
 * @param msg  * 
 * @param timeout  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 512
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_can_send(const canmsg_t* msg, uint32_t timeout, overlapped_t * overlapped);
#else
extern result_t sys_can_send(const canmsg_t* msg, uint32_t timeout, overlapped_t * overlapped);
#endif

static inline result_t can_send(const canmsg_t* msg, uint32_t timeout, overlapped_t * overlapped)
  {
  return sys_can_send(msg, timeout, overlapped);
  }

/**
 * Return the node-id of the running application
 * @param id [out]  * 
 * @return result_t
 * @syscall 513
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_device_id(uint8_t* id);
#else
extern result_t sys_get_device_id(uint8_t* id);
#endif

static inline result_t get_device_id(uint8_t* id)
  {
  return sys_get_device_id(id);
  }

/**
 * Return the node-id of the running application
 * @param id  * 
 * @return result_t
 * @syscall 514
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_set_device_id(uint8_t id);
#else
extern result_t sys_set_device_id(uint8_t id);
#endif

static inline result_t set_device_id(uint8_t id)
  {
  return sys_set_device_id(id);
  }

/******************************************************************************/
/* Category: stream - Stream, file, and directory operations                  */
/******************************************************************************/

/**
 * Open a stream in the file system
 * @note This call may use overlapped I/O.
 * @param path  * 
 * @param flags  * 
 * @param stream [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 768
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_stream_open(const char* path, uint32_t flags, handle_t* stream, overlapped_t * overlapped);
#else
extern result_t sys_stream_open(const char* path, uint32_t flags, handle_t* stream, overlapped_t * overlapped);
#endif

static inline result_t stream_open(const char* path, uint32_t flags, handle_t* stream, overlapped_t * overlapped)
  {
  return sys_stream_open(path, flags, stream, overlapped);
  }

/**
 * Remove a stream
 * @note This call may use overlapped I/O.
 * @param path  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 769
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_stream_delete(const char* path, overlapped_t * overlapped);
#else
extern result_t sys_stream_delete(const char* path, overlapped_t * overlapped);
#endif

static inline result_t stream_delete(const char* path, overlapped_t * overlapped)
  {
  return sys_stream_delete(path, overlapped);
  }

/**
 * Rename a stream
 * @note This call may use overlapped I/O.
 * @param old_filename  * 
 * @param new_filename  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 770
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_stream_rename(const char* old_filename, const char* new_filename, overlapped_t * overlapped);
#else
extern result_t sys_stream_rename(const char* old_filename, const char* new_filename, overlapped_t * overlapped);
#endif

static inline result_t stream_rename(const char* old_filename, const char* new_filename, overlapped_t * overlapped)
  {
  return sys_stream_rename(old_filename, new_filename, overlapped);
  }

/**
 * Read bytes from a stream
 * @note This call may use overlapped I/O.
 * @param stream  * 
 * @param buffer [out]  * 
 * @param size  * 
 * @param read [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 775
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_stream_read(handle_t stream, void* buffer, uint32_t size, uint32_t* read, overlapped_t * overlapped);
#else
extern result_t sys_stream_read(handle_t stream, void* buffer, uint32_t size, uint32_t* read, overlapped_t * overlapped);
#endif

static inline result_t stream_read(handle_t stream, void* buffer, uint32_t size, uint32_t* read, overlapped_t * overlapped)
  {
  return sys_stream_read(stream, buffer, size, read, overlapped);
  }

/**
 * Write bytes to a stream
 * @note This call may use overlapped I/O.
 * @param stream  * 
 * @param buffer  * 
 * @param size  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 776
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_stream_write(handle_t stream, const void* buffer, uint32_t size, overlapped_t * overlapped);
#else
extern result_t sys_stream_write(handle_t stream, const void* buffer, uint32_t size, overlapped_t * overlapped);
#endif

static inline result_t stream_write(handle_t stream, const void* buffer, uint32_t size, overlapped_t * overlapped)
  {
  return sys_stream_write(stream, buffer, size, overlapped);
  }

/**
 * Get the current stream position
 * @param stream  * 
 * @param pos [out]  * 
 * @return result_t
 * @syscall 777
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_stream_getpos(handle_t stream, uint32_t* pos);
#else
extern result_t sys_stream_getpos(handle_t stream, uint32_t* pos);
#endif

static inline result_t stream_getpos(handle_t stream, uint32_t* pos)
  {
  return sys_stream_getpos(stream, pos);
  }

/**
 * Set the stream position
 * @param stream  * 
 * @param pos  * 
 * @param whence  * 
 * @return result_t
 * @syscall 778
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_stream_setpos(handle_t stream, int32_t pos, uint32_t whence);
#else
extern result_t sys_stream_setpos(handle_t stream, int32_t pos, uint32_t whence);
#endif

static inline result_t stream_setpos(handle_t stream, int32_t pos, uint32_t whence)
  {
  return sys_stream_setpos(stream, pos, whence);
  }

/**
 * Return the number of bytes in a stream
 * @param stream  * 
 * @param length [out]  * 
 * @return result_t
 * @syscall 779
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_stream_length(handle_t stream, uint32_t* length);
#else
extern result_t sys_stream_length(handle_t stream, uint32_t* length);
#endif

static inline result_t stream_length(handle_t stream, uint32_t* length)
  {
  return sys_stream_length(stream, length);
  }

/**
 * Set the stream length
 * @note This call may use overlapped I/O.
 * @param stream  * 
 * @param length  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 780
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_stream_truncate(handle_t stream, uint32_t length, overlapped_t * overlapped);
#else
extern result_t sys_stream_truncate(handle_t stream, uint32_t length, overlapped_t * overlapped);
#endif

static inline result_t stream_truncate(handle_t stream, uint32_t length, overlapped_t * overlapped)
  {
  return sys_stream_truncate(stream, length, overlapped);
  }

/**
 * Check for end of stream (s_ok if at EOF)
 * @param stream  * 
 * @return result_t
 * @syscall 781
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_stream_eof(handle_t stream);
#else
extern result_t sys_stream_eof(handle_t stream);
#endif

static inline result_t stream_eof(handle_t stream)
  {
  return sys_stream_eof(stream);
  }

/**
 * Copy between two streams
 * @note This call may use overlapped I/O.
 * @param from  * 
 * @param to  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 782
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_stream_copy(handle_t from, handle_t to, overlapped_t * overlapped);
#else
extern result_t sys_stream_copy(handle_t from, handle_t to, overlapped_t * overlapped);
#endif

static inline result_t stream_copy(handle_t from, handle_t to, overlapped_t * overlapped)
  {
  return sys_stream_copy(from, to, overlapped);
  }

/**
 * Create a directory
 * @note This call may use overlapped I/O.
 * @param path  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 800
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_create_directory(const char* path, overlapped_t * overlapped);
#else
extern result_t sys_create_directory(const char* path, overlapped_t * overlapped);
#endif

static inline result_t create_directory(const char* path, overlapped_t * overlapped)
  {
  return sys_create_directory(path, overlapped);
  }

/**
 * Remove an empty directory
 * @note This call may use overlapped I/O.
 * @param path  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 801
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_remove_directory(const char* path, overlapped_t * overlapped);
#else
extern result_t sys_remove_directory(const char* path, overlapped_t * overlapped);
#endif

static inline result_t remove_directory(const char* path, overlapped_t * overlapped)
  {
  return sys_remove_directory(path, overlapped);
  }

/**
 * Open a directory for enumeration
 * @note This call may use overlapped I/O.
 * @param dirname  * 
 * @param dirp [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 802
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_open_directory(const char* dirname, handle_t* dirp, overlapped_t * overlapped);
#else
extern result_t sys_open_directory(const char* dirname, handle_t* dirp, overlapped_t * overlapped);
#endif

static inline result_t open_directory(const char* dirname, handle_t* dirp, overlapped_t * overlapped)
  {
  return sys_open_directory(dirname, dirp, overlapped);
  }

/**
 * Read the next directory entry
 * @note This call may use overlapped I/O.
 * @param dirp  * 
 * @param stat [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 803
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_read_directory(handle_t dirp, stat_t* stat, overlapped_t * overlapped);
#else
extern result_t sys_read_directory(handle_t dirp, stat_t* stat, overlapped_t * overlapped);
#endif

static inline result_t read_directory(handle_t dirp, stat_t* stat, overlapped_t * overlapped)
  {
  return sys_read_directory(dirp, stat, overlapped);
  }

/**
 * Rewind directory enumeration
 * @note This call may use overlapped I/O.
 * @param dirp  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 804
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_rewind_directory(handle_t dirp, overlapped_t * overlapped);
#else
extern result_t sys_rewind_directory(handle_t dirp, overlapped_t * overlapped);
#endif

static inline result_t rewind_directory(handle_t dirp, overlapped_t * overlapped)
  {
  return sys_rewind_directory(dirp, overlapped);
  }

/**
 * Return information about a file or directory
 * @note This call may use overlapped I/O.
 * @param path  * 
 * @param st [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 805
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_stream_stat(const char* path, stat_t* st, overlapped_t * overlapped);
#else
extern result_t sys_stream_stat(const char* path, stat_t* st, overlapped_t * overlapped);
#endif

static inline result_t stream_stat(const char* path, stat_t* st, overlapped_t * overlapped)
  {
  return sys_stream_stat(path, st, overlapped);
  }

/**
 * Return available space on a device
 * @param path  * 
 * @param space [out]  * 
 * @return result_t
 * @syscall 810
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_freespace(const char* path, uint32_t* space);
#else
extern result_t sys_freespace(const char* path, uint32_t* space);
#endif

static inline result_t freespace(const char* path, uint32_t* space)
  {
  return sys_freespace(path, space);
  }

/**
 * Return total space on a device
 * @param path  * 
 * @param space [out]  * 
 * @return result_t
 * @syscall 811
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_totalspace(const char* path, uint32_t* space);
#else
extern result_t sys_totalspace(const char* path, uint32_t* space);
#endif

static inline result_t totalspace(const char* path, uint32_t* space)
  {
  return sys_totalspace(path, space);
  }

/******************************************************************************/
/* Category: photon - Photon 2D graphics drawing interface                    */
/******************************************************************************/

/**
 * Create an off-screen canvas
 * @param size  * 
 * @param hndl [out]  * 
 * @return result_t
 * @syscall 1280
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_canvas_create(const extent_t* size, handle_t* hndl);
#else
extern result_t sys_canvas_create(const extent_t* size, handle_t* hndl);
#endif

static inline result_t canvas_create(const extent_t* size, handle_t* hndl)
  {
  return sys_canvas_create(size, hndl);
  }

/**
 * Create a canvas from a bitmap
 * @param bitmap  * 
 * @param hndl [out]  * 
 * @return result_t
 * @syscall 1281
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_canvas_create_bitmap(const bitmap_t* bitmap, handle_t* hndl);
#else
extern result_t sys_canvas_create_bitmap(const bitmap_t* bitmap, handle_t* hndl);
#endif

static inline result_t canvas_create_bitmap(const bitmap_t* bitmap, handle_t* hndl)
  {
  return sys_canvas_create_bitmap(bitmap, hndl);
  }

/**
 * Create a canvas from a PNG stream
 * @param stream  * 
 * @param hndl [out]  * 
 * @return result_t
 * @syscall 1282
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_canvas_create_png(handle_t stream, handle_t* hndl);
#else
extern result_t sys_canvas_create_png(handle_t stream, handle_t* hndl);
#endif

static inline result_t canvas_create_png(handle_t stream, handle_t* hndl)
  {
  return sys_canvas_create_png(stream, hndl);
  }

/**
 * Return the extents of a canvas
 * @param canvas  * 
 * @param ex [out]  * 
 * @return result_t
 * @syscall 1283
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_canvas_extents(handle_t canvas, extent_t* ex);
#else
extern result_t sys_canvas_extents(handle_t canvas, extent_t* ex);
#endif

static inline result_t canvas_extents(handle_t canvas, extent_t* ex)
  {
  return sys_canvas_extents(canvas, ex);
  }

/**
 * Draw a polyline
 * @param canvas  * 
 * @param clip_rect  * 
 * @param pen  * 
 * @param count  * 
 * @param points  * 
 * @return result_t
 * @syscall 1290
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_polyline(handle_t canvas, const rect_t* clip_rect, color_t pen, uint32_t count, const point_t* points);
#else
extern result_t sys_polyline(handle_t canvas, const rect_t* clip_rect, color_t pen, uint32_t count, const point_t* points);
#endif

static inline result_t polyline(handle_t canvas, const rect_t* clip_rect, color_t pen, uint32_t count, const point_t* points)
  {
  return sys_polyline(canvas, clip_rect, pen, count, points);
  }

/**
 * Draw an ellipse
 * @param canvas  * 
 * @param clip_rect  * 
 * @param pen  * 
 * @param fill  * 
 * @param area  * 
 * @return result_t
 * @syscall 1291
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_ellipse(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area);
#else
extern result_t sys_ellipse(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area);
#endif

static inline result_t ellipse(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area)
  {
  return sys_ellipse(canvas, clip_rect, pen, fill, area);
  }

/**
 * Draw and optionally fill a polygon
 * @param canvas  * 
 * @param clip_rect  * 
 * @param pen  * 
 * @param fill  * 
 * @param count  * 
 * @param points  * 
 * @return result_t
 * @syscall 1292
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_polygon(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, uint32_t count, const point_t* points);
#else
extern result_t sys_polygon(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, uint32_t count, const point_t* points);
#endif

static inline result_t polygon(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, uint32_t count, const point_t* points)
  {
  return sys_polygon(canvas, clip_rect, pen, fill, count, points);
  }

/**
 * Draw a rectangle
 * @param canvas  * 
 * @param clip_rect  * 
 * @param pen  * 
 * @param fill  * 
 * @param area  * 
 * @return result_t
 * @syscall 1293
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_rectangle(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area);
#else
extern result_t sys_rectangle(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area);
#endif

static inline result_t rectangle(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area)
  {
  return sys_rectangle(canvas, clip_rect, pen, fill, area);
  }

/**
 * Draw a rectangle with rounded corners
 * @param canvas  * 
 * @param clip_rect  * 
 * @param pen  * 
 * @param fill  * 
 * @param area  * 
 * @param radius  * 
 * @return result_t
 * @syscall 1294
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_round_rect(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area, gdi_dim_t radius);
#else
extern result_t sys_round_rect(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area, gdi_dim_t radius);
#endif

static inline result_t round_rect(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area, gdi_dim_t radius)
  {
  return sys_round_rect(canvas, clip_rect, pen, fill, area, radius);
  }

/**
 * Draw an arc
 * @param canvas  * 
 * @param clip_rect  * 
 * @param pen  * 
 * @param pt  * 
 * @param radius  * 
 * @param start  * 
 * @param end  * 
 * @return result_t
 * @syscall 1295
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_arc(handle_t canvas, const rect_t* clip_rect, color_t pen, const point_t* pt, gdi_dim_t radius, int16_t start, int16_t end);
#else
extern result_t sys_arc(handle_t canvas, const rect_t* clip_rect, color_t pen, const point_t* pt, gdi_dim_t radius, int16_t start, int16_t end);
#endif

static inline result_t arc(handle_t canvas, const rect_t* clip_rect, color_t pen, const point_t* pt, gdi_dim_t radius, int16_t start, int16_t end)
  {
  return sys_arc(canvas, clip_rect, pen, pt, radius, start, end);
  }

/**
 * Draw a pie (filled arc segment)
 * @param canvas  * 
 * @param clip_rect  * 
 * @param pen  * 
 * @param fill  * 
 * @param pt  * 
 * @param start  * 
 * @param end  * 
 * @param radii  * 
 * @param inner  * 
 * @return result_t
 * @syscall 1296
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_pie(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const point_t* pt, int16_t start, int16_t end, gdi_dim_t radii, gdi_dim_t inner);
#else
extern result_t sys_pie(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const point_t* pt, int16_t start, int16_t end, gdi_dim_t radii, gdi_dim_t inner);
#endif

static inline result_t pie(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, const point_t* pt, int16_t start, int16_t end, gdi_dim_t radii, gdi_dim_t inner)
  {
  return sys_pie(canvas, clip_rect, pen, fill, pt, start, end, radii, inner);
  }

/**
 * Copy pixels between canvases
 * @param canvas  * 
 * @param clip_rect  * 
 * @param dest_rect  * 
 * @param src_canvas  * 
 * @param src_clip_rect  * 
 * @param src_pt  * 
 * @param operation  * 
 * @return result_t
 * @syscall 1297
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_bit_blt(handle_t canvas, const rect_t* clip_rect, const rect_t* dest_rect, handle_t src_canvas, const rect_t* src_clip_rect, const point_t* src_pt, raster_operation operation);
#else
extern result_t sys_bit_blt(handle_t canvas, const rect_t* clip_rect, const rect_t* dest_rect, handle_t src_canvas, const rect_t* src_clip_rect, const point_t* src_pt, raster_operation operation);
#endif

static inline result_t bit_blt(handle_t canvas, const rect_t* clip_rect, const rect_t* dest_rect, handle_t src_canvas, const rect_t* src_clip_rect, const point_t* src_pt, raster_operation operation)
  {
  return sys_bit_blt(canvas, clip_rect, dest_rect, src_canvas, src_clip_rect, src_pt, operation);
  }

/**
 * Get a pixel color
 * @param canvas  * 
 * @param clip_rect  * 
 * @param pt  * 
 * @param pix [out]  * 
 * @return result_t
 * @syscall 1298
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_pixel(handle_t canvas, const rect_t* clip_rect, const point_t* pt, color_t* pix);
#else
extern result_t sys_get_pixel(handle_t canvas, const rect_t* clip_rect, const point_t* pt, color_t* pix);
#endif

static inline result_t get_pixel(handle_t canvas, const rect_t* clip_rect, const point_t* pt, color_t* pix)
  {
  return sys_get_pixel(canvas, clip_rect, pt, pix);
  }

/**
 * Set a pixel color
 * @param canvas  * 
 * @param clip_rect  * 
 * @param pt  * 
 * @param c  * 
 * @param pix [out]  * 
 * @return result_t
 * @syscall 1299
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_set_pixel(handle_t canvas, const rect_t* clip_rect, const point_t* pt, color_t c, color_t* pix);
#else
extern result_t sys_set_pixel(handle_t canvas, const rect_t* clip_rect, const point_t* pt, color_t c, color_t* pix);
#endif

static inline result_t set_pixel(handle_t canvas, const rect_t* clip_rect, const point_t* pt, color_t c, color_t* pix)
  {
  return sys_set_pixel(canvas, clip_rect, pt, c, pix);
  }

/**
 * Draw text on a canvas
 * @param canvas  * 
 * @param clip_rect  * 
 * @param font  * 
 * @param fg  * 
 * @param bg  * 
 * @param count  * 
 * @param str  * 
 * @param src_pt  * 
 * @param txt_clip_rect  * 
 * @param format  * 
 * @param char_widths [out]  * 
 * @return result_t
 * @syscall 1310
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_draw_text(handle_t canvas, const rect_t* clip_rect, const font_t* font, color_t fg, color_t bg, uint16_t count, const char* str, const point_t* src_pt, const rect_t * txt_clip_rect, text_flags format, uint16_t * char_widths);
#else
extern result_t sys_draw_text(handle_t canvas, const rect_t* clip_rect, const font_t* font, color_t fg, color_t bg, uint16_t count, const char* str, const point_t* src_pt, const rect_t * txt_clip_rect, text_flags format, uint16_t * char_widths);
#endif

static inline result_t draw_text(handle_t canvas, const rect_t* clip_rect, const font_t* font, color_t fg, color_t bg, uint16_t count, const char* str, const point_t* src_pt, const rect_t * txt_clip_rect, text_flags format, uint16_t * char_widths)
  {
  return sys_draw_text(canvas, clip_rect, font, fg, bg, count, str, src_pt, txt_clip_rect, format, char_widths);
  }

/**
 * Measure the extents of a text string
 * @param font  * 
 * @param count  * 
 * @param str  * 
 * @param ex [out]  * 
 * @return result_t
 * @syscall 1311
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_text_extent(const font_t* font, uint16_t count, const char* str, extent_t* ex);
#else
extern result_t sys_text_extent(const font_t* font, uint16_t count, const char* str, extent_t* ex);
#endif

static inline result_t text_extent(const font_t* font, uint16_t count, const char* str, extent_t* ex)
  {
  return sys_text_extent(font, count, str, ex);
  }

/**
 * Rotate a point around a center point by a given angle
 * @param center  * 
 * @param angle  * 
 * @param pt [out]  * 
 * @return result_t
 * @syscall 1312
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_rotate_point(const point_t* center, int16_t angle, point_t* pt);
#else
extern result_t sys_rotate_point(const point_t* center, int16_t angle, point_t* pt);
#endif

static inline result_t rotate_point(const point_t* center, int16_t angle, point_t* pt)
  {
  return sys_rotate_point(center, angle, pt);
  }

/**
 * Register a window as a touch window
 * @param hndl  * 
 * @param flags  * 
 * @return result_t
 * @syscall 1313
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_register_touch_window(handle_t hndl, uint16_t flags);
#else
extern result_t sys_register_touch_window(handle_t hndl, uint16_t flags);
#endif

static inline result_t register_touch_window(handle_t hndl, uint16_t flags)
  {
  return sys_register_touch_window(hndl, flags);
  }

/**
 * Check if the touch message is a window tap on the window
 * @param hndl  * 
 * @param msg  * 
 * @param blur  * 
 * @return result_t
 * @syscall 1314
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_is_window_tap(handle_t hndl, const canmsg_t* msg, uint16_t blur);
#else
extern result_t sys_is_window_tap(handle_t hndl, const canmsg_t* msg, uint16_t blur);
#endif

static inline result_t is_window_tap(handle_t hndl, const canmsg_t* msg, uint16_t blur)
  {
  return sys_is_window_tap(hndl, msg, blur);
  }

/**
 * get the actual touch message that is queued to the window
 * @param msg  * 
 * @param touch_msg  * 
 * @return result_t
 * @syscall 1315
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_touch_msg(const canmsg_t* msg, touch_msg_t* touch_msg);
#else
extern result_t sys_get_touch_msg(const canmsg_t* msg, touch_msg_t* touch_msg);
#endif

static inline result_t get_touch_msg(const canmsg_t* msg, touch_msg_t* touch_msg)
  {
  return sys_get_touch_msg(msg, touch_msg);
  }

/******************************************************************************/
/* Category: window - Window creation, management, and message dispatch       */
/******************************************************************************/

/**
 * Create a new child window. The wndproc and wnddata are stored with
 * the window but are never called by the kernel. User-mode dispatch_message
 * retrieves them via get_wndproc and calls the wndproc in user space.
 * @param parent  * 
 * @param bounds  * 
 * @param cb  * 
 * @param wnddata  * 
 * @param flags  * 
 * @param hndl [out]  * 
 * @return result_t
 * @syscall 1536
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_window_create(handle_t parent, const rect_t* bounds, wndproc_fn cb, void* wnddata, uint32_t flags, handle_t* hndl);
#else
extern result_t sys_window_create(handle_t parent, const rect_t* bounds, wndproc_fn cb, void* wnddata, uint32_t flags, handle_t* hndl);
#endif

static inline result_t window_create(handle_t parent, const rect_t* bounds, wndproc_fn cb, void* wnddata, uint32_t flags, handle_t* hndl)
  {
  return sys_window_create(parent, bounds, cb, wnddata, flags, hndl);
  }

/**
 * Return the root screen window
 * @param hndl [out]  * 
 * @return result_t
 * @syscall 1537
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_screen(handle_t* hndl);
#else
extern result_t sys_get_screen(handle_t* hndl);
#endif

static inline result_t get_screen(handle_t* hndl)
  {
  return sys_get_screen(hndl);
  }

/**
 * Get the parent of a window
 * @param hwnd  * 
 * @param parent [out]  * 
 * @return result_t
 * @syscall 1540
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_window_parent(handle_t hwnd, handle_t* parent);
#else
extern result_t sys_window_parent(handle_t hwnd, handle_t* parent);
#endif

static inline result_t window_parent(handle_t hwnd, handle_t* parent)
  {
  return sys_window_parent(hwnd, parent);
  }

/**
 * Return the first child of a window
 * @param hwnd  * 
 * @param child [out]  * 
 * @return result_t
 * @syscall 1541
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_first_child(handle_t hwnd, handle_t* child);
#else
extern result_t sys_get_first_child(handle_t hwnd, handle_t* child);
#endif

static inline result_t get_first_child(handle_t hwnd, handle_t* child)
  {
  return sys_get_first_child(hwnd, child);
  }

/**
 * Return the next sibling of a window
 * @param hwnd  * 
 * @param sibling [out]  * 
 * @return result_t
 * @syscall 1542
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_next_sibling(handle_t hwnd, handle_t* sibling);
#else
extern result_t sys_get_next_sibling(handle_t hwnd, handle_t* sibling);
#endif

static inline result_t get_next_sibling(handle_t hwnd, handle_t* sibling)
  {
  return sys_get_next_sibling(hwnd, sibling);
  }

/**
 * Return the previous sibling of a window
 * @param hwnd  * 
 * @param sibling [out]  * 
 * @return result_t
 * @syscall 1543
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_previous_sibling(handle_t hwnd, handle_t* sibling);
#else
extern result_t sys_get_previous_sibling(handle_t hwnd, handle_t* sibling);
#endif

static inline result_t get_previous_sibling(handle_t hwnd, handle_t* sibling)
  {
  return sys_get_previous_sibling(hwnd, sibling);
  }

/**
 * Associate user data with a window
 * @param hwnd  * 
 * @param wnddata  * 
 * @return result_t
 * @syscall 1550
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_set_wnddata(handle_t hwnd, void* wnddata);
#else
extern result_t sys_set_wnddata(handle_t hwnd, void* wnddata);
#endif

static inline result_t set_wnddata(handle_t hwnd, void* wnddata)
  {
  return sys_set_wnddata(hwnd, wnddata);
  }

/**
 * Return user data associated with a window
 * @param hwnd  * 
 * @param wnddata [out]  * 
 * @return result_t
 * @syscall 1551
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_wnddata(handle_t hwnd, void** wnddata);
#else
extern result_t sys_get_wnddata(handle_t hwnd, void** wnddata);
#endif

static inline result_t get_wnddata(handle_t hwnd, void** wnddata)
  {
  return sys_get_wnddata(hwnd, wnddata);
  }

/**
 * Get the flags/style of a window
 * @param hwnd  * 
 * @param flags [out]  * 
 * @return result_t
 * @syscall 1552
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_window_flags(handle_t hwnd, uint32_t* flags);
#else
extern result_t sys_get_window_flags(handle_t hwnd, uint32_t* flags);
#endif

static inline result_t get_window_flags(handle_t hwnd, uint32_t* flags)
  {
  return sys_get_window_flags(hwnd, flags);
  }

/**
 * Set the flags/style of a window
 * @param hwnd  * 
 * @param flags  * 
 * @return result_t
 * @syscall 1553
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_set_window_flags(handle_t hwnd, uint32_t flags);
#else
extern result_t sys_set_window_flags(handle_t hwnd, uint32_t flags);
#endif

static inline result_t set_window_flags(handle_t hwnd, uint32_t flags)
  {
  return sys_set_window_flags(hwnd, flags);
  }

/**
 * Get the position of a window relative to its parent
 * @param hwnd  * 
 * @param pos [out]  * 
 * @return result_t
 * @syscall 1554
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_window_getpos(handle_t hwnd, rect_t* pos);
#else
extern result_t sys_window_getpos(handle_t hwnd, rect_t* pos);
#endif

static inline result_t window_getpos(handle_t hwnd, rect_t* pos)
  {
  return sys_window_getpos(hwnd, pos);
  }

/**
 * Set the position of a window
 * @param hwnd  * 
 * @param pos  * 
 * @return result_t
 * @syscall 1555
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_window_setpos(handle_t hwnd, const rect_t* pos);
#else
extern result_t sys_window_setpos(handle_t hwnd, const rect_t* pos);
#endif

static inline result_t window_setpos(handle_t hwnd, const rect_t* pos)
  {
  return sys_window_setpos(hwnd, pos);
  }

/**
 * Return the window rectangle (screen coordinates)
 * @param hwnd  * 
 * @param rect [out]  * 
 * @return result_t
 * @syscall 1556
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_window_rect(handle_t hwnd, rect_t* rect);
#else
extern result_t sys_window_rect(handle_t hwnd, rect_t* rect);
#endif

static inline result_t window_rect(handle_t hwnd, rect_t* rect)
  {
  return sys_window_rect(hwnd, rect);
  }

/**
 * Make a window visible
 * @param hwnd  * 
 * @return result_t
 * @syscall 1560
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_show_window(handle_t hwnd);
#else
extern result_t sys_show_window(handle_t hwnd);
#endif

static inline result_t show_window(handle_t hwnd)
  {
  return sys_show_window(hwnd);
  }

/**
 * Hide a window
 * @param hwnd  * 
 * @return result_t
 * @syscall 1561
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_hide_window(handle_t hwnd);
#else
extern result_t sys_hide_window(handle_t hwnd);
#endif

static inline result_t hide_window(handle_t hwnd)
  {
  return sys_hide_window(hwnd);
  }

/**
 * Check if a window is visible (s_ok if visible)
 * @param hwnd  * 
 * @return result_t
 * @syscall 1562
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_is_visible(handle_t hwnd);
#else
extern result_t sys_is_visible(handle_t hwnd);
#endif

static inline result_t is_visible(handle_t hwnd)
  {
  return sys_is_visible(hwnd);
  }

/**
 * Enable a window for input
 * @param hwnd  * 
 * @return result_t
 * @syscall 1563
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_enable_window(handle_t hwnd);
#else
extern result_t sys_enable_window(handle_t hwnd);
#endif

static inline result_t enable_window(handle_t hwnd)
  {
  return sys_enable_window(hwnd);
  }

/**
 * Disable a window from input
 * @param hwnd  * 
 * @return result_t
 * @syscall 1564
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_disable_window(handle_t hwnd);
#else
extern result_t sys_disable_window(handle_t hwnd);
#endif

static inline result_t disable_window(handle_t hwnd)
  {
  return sys_disable_window(hwnd);
  }

/**
 * Check if a window is enabled
 * @param hwnd  * 
 * @return result_t
 * @syscall 1565
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_is_enabled(handle_t hwnd);
#else
extern result_t sys_is_enabled(handle_t hwnd);
#endif

static inline result_t is_enabled(handle_t hwnd)
  {
  return sys_is_enabled(hwnd);
  }

/**
 * Mark a window as needing repaint
 * @param hwnd  * 
 * @return result_t
 * @syscall 1566
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_invalidate(handle_t hwnd);
#else
extern result_t sys_invalidate(handle_t hwnd);
#endif

static inline result_t invalidate(handle_t hwnd)
  {
  return sys_invalidate(hwnd);
  }

/**
 * Check if a window needs repaint
 * @param hwnd  * 
 * @return result_t
 * @syscall 1567
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_is_invalid(handle_t hwnd);
#else
extern result_t sys_is_invalid(handle_t hwnd);
#endif

static inline result_t is_invalid(handle_t hwnd)
  {
  return sys_is_invalid(hwnd);
  }

/**
 * Get the paint order of a window
 * @param hwnd  * 
 * @param value [out]  * 
 * @return result_t
 * @syscall 1568
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_z_order(handle_t hwnd, uint8_t* value);
#else
extern result_t sys_get_z_order(handle_t hwnd, uint8_t* value);
#endif

static inline result_t get_z_order(handle_t hwnd, uint8_t* value)
  {
  return sys_get_z_order(hwnd, value);
  }

/**
 * Set the paint order of a window
 * @param hwnd  * 
 * @param value  * 
 * @return result_t
 * @syscall 1569
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_set_z_order(handle_t hwnd, uint8_t value);
#else
extern result_t sys_set_z_order(handle_t hwnd, uint8_t value);
#endif

static inline result_t set_z_order(handle_t hwnd, uint8_t value)
  {
  return sys_set_z_order(hwnd, value);
  }

/**
 * Begin painting on a window (returns canvas handle)
 * @param hwnd  * 
 * @param canvas [out]  * 
 * @return result_t
 * @syscall 1570
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_begin_paint(handle_t hwnd, handle_t* canvas);
#else
extern result_t sys_begin_paint(handle_t hwnd, handle_t* canvas);
#endif

static inline result_t begin_paint(handle_t hwnd, handle_t* canvas)
  {
  return sys_begin_paint(hwnd, canvas);
  }

/**
 * End painting on a window
 * @param hwnd  * 
 * @return result_t
 * @syscall 1571
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_end_paint(handle_t hwnd);
#else
extern result_t sys_end_paint(handle_t hwnd);
#endif

static inline result_t end_paint(handle_t hwnd)
  {
  return sys_end_paint(hwnd);
  }

/**
 * Post a message to a window queue (non-blocking)
 * @param hwnd  * 
 * @param msg  * 
 * @param delay  * 
 * @return result_t
 * @syscall 1580
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_post_message(handle_t hwnd, const canmsg_t* msg, uint32_t delay);
#else
extern result_t sys_post_message(handle_t hwnd, const canmsg_t* msg, uint32_t delay);
#endif

static inline result_t post_message(handle_t hwnd, const canmsg_t* msg, uint32_t delay)
  {
  return sys_post_message(hwnd, msg, delay);
  }

/**
 * Get the next message from the window queue
 * @param hwnd [out]  * 
 * @param msg [out]  * 
 * @return result_t
 * @syscall 1582
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_message(handle_t* hwnd, canmsg_t* msg);
#else
extern result_t sys_get_message(handle_t* hwnd, canmsg_t* msg);
#endif

static inline result_t get_message(handle_t* hwnd, canmsg_t* msg)
  {
  return sys_get_message(hwnd, msg);
  }

/**
 * Get the window procedure and data for a window (used by user-mode dispatch)
 * @param hwnd  * 
 * @param wndproc [out]  * 
 * @param wnddata [out]  * 
 * @return result_t
 * @syscall 1583
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_wndproc(handle_t hwnd, wndproc_fn* wndproc, void** wnddata);
#else
extern result_t sys_get_wndproc(handle_t hwnd, wndproc_fn* wndproc, void** wnddata);
#endif

static inline result_t get_wndproc(handle_t hwnd, wndproc_fn* wndproc, void** wnddata)
  {
  return sys_get_wndproc(hwnd, wndproc, wnddata);
  }

/**
 * Get the window with current input focus
 * @param hndl [out]  * 
 * @return result_t
 * @syscall 1590
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_focused_window(handle_t* hndl);
#else
extern result_t sys_get_focused_window(handle_t* hndl);
#endif

static inline result_t get_focused_window(handle_t* hndl)
  {
  return sys_get_focused_window(hndl);
  }

/**
 * Set the window with input focus
 * @param hndl  * 
 * @return result_t
 * @syscall 1591
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_set_focused_window(handle_t hndl);
#else
extern result_t sys_set_focused_window(handle_t hndl);
#endif

static inline result_t set_focused_window(handle_t hndl)
  {
  return sys_set_focused_window(hndl);
  }

/**
 * Check if a window has focus
 * @param hwnd  * 
 * @return result_t
 * @syscall 1592
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_is_focused(handle_t hwnd);
#else
extern result_t sys_is_focused(handle_t hwnd);
#endif

static inline result_t is_focused(handle_t hwnd)
  {
  return sys_is_focused(hwnd);
  }

/**
 * Get the ID of a window
 * @param hndl  * 
 * @param id [out]  * 
 * @return result_t
 * @syscall 1595
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_window_id(handle_t hndl, uint8_t* id);
#else
extern result_t sys_get_window_id(handle_t hndl, uint8_t* id);
#endif

static inline result_t get_window_id(handle_t hndl, uint8_t* id)
  {
  return sys_get_window_id(hndl, id);
  }

/**
 * Set the ID of a window
 * @param hndl  * 
 * @param id  * 
 * @return result_t
 * @syscall 1596
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_set_window_id(handle_t hndl, uint8_t id);
#else
extern result_t sys_set_window_id(handle_t hndl, uint8_t id);
#endif

static inline result_t set_window_id(handle_t hndl, uint8_t id)
  {
  return sys_set_window_id(hndl, id);
  }

/**
 * Find a child window by its ID
 * @param hwnd  * 
 * @param id  * 
 * @param child [out]  * 
 * @return result_t
 * @syscall 1597
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_window_by_id(handle_t hwnd, uint8_t id, handle_t* child);
#else
extern result_t sys_get_window_by_id(handle_t hwnd, uint8_t id, handle_t* child);
#endif

static inline result_t get_window_by_id(handle_t hwnd, uint8_t id, handle_t* child)
  {
  return sys_get_window_by_id(hwnd, id, child);
  }

/**
 * Get the brightness of the display backlight
 * @param value [out]  * 
 * @return result_t
 * @syscall 1650
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_display_brightness(uint16_t* value);
#else
extern result_t sys_get_display_brightness(uint16_t* value);
#endif

static inline result_t get_display_brightness(uint16_t* value)
  {
  return sys_get_display_brightness(value);
  }

/**
 * Set the brightness of the display backlight
 * @param value  * 
 * @return result_t
 * @syscall 1651
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_set_display_brightness(uint16_t value);
#else
extern result_t sys_set_display_brightness(uint16_t value);
#endif

static inline result_t set_display_brightness(uint16_t value)
  {
  return sys_set_display_brightness(value);
  }

/**
 * Return the next enabled window after the passed in window
 * @param hwnd  * 
 * @param next [out]  * 
 * @return result_t
 * @syscall 1652
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_next_enabled_window(handle_t hwnd, handle_t* next);
#else
extern result_t sys_get_next_enabled_window(handle_t hwnd, handle_t* next);
#endif

static inline result_t get_next_enabled_window(handle_t hwnd, handle_t* next)
  {
  return sys_get_next_enabled_window(hwnd, next);
  }

/**
 * Return the next tabstop window after the passed in window
 * @param hwnd  * 
 * @param next [out]  * 
 * @return result_t
 * @syscall 1653
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_next_tabstop_window(handle_t hwnd, handle_t* next);
#else
extern result_t sys_get_next_tabstop_window(handle_t hwnd, handle_t* next);
#endif

static inline result_t get_next_tabstop_window(handle_t hwnd, handle_t* next)
  {
  return sys_get_next_tabstop_window(hwnd, next);
  }

/**
 * Return the previous tabstop window after the passed in window
 * @param hwnd  * 
 * @param prev [out]  * 
 * @return result_t
 * @syscall 1654
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_previous_tabstop_window(handle_t hwnd, handle_t* prev);
#else
extern result_t sys_get_previous_tabstop_window(handle_t hwnd, handle_t* prev);
#endif

static inline result_t get_previous_tabstop_window(handle_t hwnd, handle_t* prev)
  {
  return sys_get_previous_tabstop_window(hwnd, prev);
  }

/******************************************************************************/
/* Category: map - Kernel-rendered map canvas API. The map renderer runs in kernel space
      with direct access to SDRAM tile cache. User-mode code provides position
      updates and display preferences; the kernel renders asynchronously and
      posts id_map_render_complete to the owning window when ready. */
/******************************************************************************/

/**
 * Create a kernel-managed map rendering canvas
 * @param hwnd  * handle to the window that the canvas will be assoiated with.  It forms
 * a background canvas, that will normally just be rendered.
 * @param extents  * The size of the map canvas to be created
 * @param theme  * These are the colors to display for the moving map.  The address of it is
 * used in the kernel and must not change after the canvas is made
 * @param canvas [out]  * 
 * @return result_t
 * @syscall 1792
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_create_canvas(handle_t hwnd, const extent_t * extents, const map_theme_t * theme, handle_t* canvas);
#else
extern result_t sys_map_create_canvas(handle_t hwnd, const extent_t * extents, const map_theme_t * theme, handle_t* canvas);
#endif

static inline result_t map_create_canvas(handle_t hwnd, const extent_t * extents, const map_theme_t * theme, handle_t* canvas)
  {
  return sys_map_create_canvas(hwnd, extents, theme, canvas);
  }

/**
 * Render the background image onto the drawing surface
 * @param canvas  * The map rendering canvas
 * @param dest  * The rectangle to render the map into
 * @param surface  * The drawing surface to copy the rendered background onto
 * @return result_t
 * @syscall 1793
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_render_canvas(handle_t canvas, const rect_t * dest, handle_t surface);
#else
extern result_t sys_map_render_canvas(handle_t canvas, const rect_t * dest, handle_t surface);
#endif

static inline result_t map_render_canvas(handle_t canvas, const rect_t * dest, handle_t surface)
  {
  return sys_map_render_canvas(canvas, dest, surface);
  }

/**
 * Update the map position. The kernel will re-render asynchronously.
 * When rendering is complete, id_map_render_complete is posted to the
 * associated window.
 * @param canvas  * 
 * @param position  * 
 * @param heading  * 
 * @param track  * 
 * @return result_t
 * @syscall 1794
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_update_position(handle_t canvas, const lla_t* position, int32_t heading, int32_t track);
#else
extern result_t sys_map_update_position(handle_t canvas, const lla_t* position, int32_t heading, int32_t track);
#endif

static inline result_t map_update_position(handle_t canvas, const lla_t* position, int32_t heading, int32_t track)
  {
  return sys_map_update_position(canvas, position, heading, track);
  }

/**
 * Set the map display range in meters
 * @param canvas  * 
 * @param range_mtrs1000  * this is the tange of the map in meters * 1000
 * @return result_t
 * @syscall 1795
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_set_range(handle_t canvas, uint32_t range_mtrs1000);
#else
extern result_t sys_map_set_range(handle_t canvas, uint32_t range_mtrs1000);
#endif

static inline result_t map_set_range(handle_t canvas, uint32_t range_mtrs1000)
  {
  return sys_map_set_range(canvas, range_mtrs1000);
  }

/**
 * Set the map display range in meters
 * @param canvas  * 
 * @param range_mtrs1000 [out]  * this is the tange of the map in meters * 1000
 * @return result_t
 * @syscall 1796
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_get_range(handle_t canvas, uint32_t* range_mtrs1000);
#else
extern result_t sys_map_get_range(handle_t canvas, uint32_t* range_mtrs1000);
#endif

static inline result_t map_get_range(handle_t canvas, uint32_t* range_mtrs1000)
  {
  return sys_map_get_range(canvas, range_mtrs1000);
  }

/**
 * Set the map display orientation mode
 * @param canvas  * 
 * @param mode  * 
 * @return result_t
 * @syscall 1797
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_set_mode(handle_t canvas, map_display_mode mode);
#else
extern result_t sys_map_set_mode(handle_t canvas, map_display_mode mode);
#endif

static inline result_t map_set_mode(handle_t canvas, map_display_mode mode)
  {
  return sys_map_set_mode(canvas, mode);
  }

/**
 * Set the map display orientation mode
 * @param canvas  * 
 * @param mode [out]  * 
 * @return result_t
 * @syscall 1798
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_get_mode(handle_t canvas, map_display_mode* mode);
#else
extern result_t sys_map_get_mode(handle_t canvas, map_display_mode* mode);
#endif

static inline result_t map_get_mode(handle_t canvas, map_display_mode* mode)
  {
  return sys_map_get_mode(canvas, mode);
  }

/**
 * Show or hide a specific map layer
 * @param canvas  * 
 * @param layer_id  * 
 * @return result_t
 * @syscall 1799
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_set_layer_visible(handle_t canvas, uint32_t layer_id);
#else
extern result_t sys_map_set_layer_visible(handle_t canvas, uint32_t layer_id);
#endif

static inline result_t map_set_layer_visible(handle_t canvas, uint32_t layer_id)
  {
  return sys_map_set_layer_visible(canvas, layer_id);
  }

/**
 * Show or hide a specific map layer
 * @param canvas  * 
 * @param layer_id [out]  * 
 * @return result_t
 * @syscall 1800
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_get_layer_visible(handle_t canvas, uint32_t* layer_id);
#else
extern result_t sys_map_get_layer_visible(handle_t canvas, uint32_t* layer_id);
#endif

static inline result_t map_get_layer_visible(handle_t canvas, uint32_t* layer_id)
  {
  return sys_map_get_layer_visible(canvas, layer_id);
  }

/**
 * Pan the map by a specific number of pixels
 * @param canvas  * 
 * @param move_by  * Distance to pan the display by
 * @return result_t
 * @syscall 1801
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_pan(handle_t canvas, const extent_t * move_by);
#else
extern result_t sys_map_pan(handle_t canvas, const extent_t * move_by);
#endif

static inline result_t map_pan(handle_t canvas, const extent_t * move_by)
  {
  return sys_map_pan(canvas, move_by);
  }

/**
 * Zoom the map by a scale in pixels
 * @param canvas  * 
 * @param zoom_by  * relative percentage to zoom the map by
 * @return result_t
 * @syscall 1802
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_zoom(handle_t canvas, int32_t zoom_by);
#else
extern result_t sys_map_zoom(handle_t canvas, int32_t zoom_by);
#endif

static inline result_t map_zoom(handle_t canvas, int32_t zoom_by)
  {
  return sys_map_zoom(canvas, zoom_by);
  }

/**
 * Convert a screen point to a geographic position
 * @param canvas  * 
 * @param screen  * 
 * @param position [out]  * 
 * @return result_t
 * @syscall 1803
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_screen_to_position(handle_t canvas, const point_t* screen, lla_t* position);
#else
extern result_t sys_map_screen_to_position(handle_t canvas, const point_t* screen, lla_t* position);
#endif

static inline result_t map_screen_to_position(handle_t canvas, const point_t* screen, lla_t* position)
  {
  return sys_map_screen_to_position(canvas, screen, position);
  }

/**
 * Convert a geographic position to a screen point
 * @param canvas  * 
 * @param position  * 
 * @param screen [out]  * 
 * @return result_t
 * @syscall 1804
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_map_position_to_screen(handle_t canvas, const lla_t* position, point_t* screen);
#else
extern result_t sys_map_position_to_screen(handle_t canvas, const lla_t* position, point_t* screen);
#endif

static inline result_t map_position_to_screen(handle_t canvas, const lla_t* position, point_t* screen)
  {
  return sys_map_position_to_screen(canvas, position, screen);
  }

/******************************************************************************/
/* Category: system -                                                         */
/******************************************************************************/

/**
 * Get an aircraft performance parameter
 * @note This call may use overlapped I/O.
 * @param value [out]  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2048
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_get_aircraft(aircraft_t* value, overlapped_t * overlapped);
#else
extern result_t sys_get_aircraft(aircraft_t* value, overlapped_t * overlapped);
#endif

static inline result_t get_aircraft(aircraft_t* value, overlapped_t * overlapped)
  {
  return sys_get_aircraft(value, overlapped);
  }

/**
 * Set an aircraft performance parameter
 * @note This call may use overlapped I/O.
 * @param value  * 
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2049
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_set_aircraft(const aircraft_t* value, overlapped_t * overlapped);
#else
extern result_t sys_set_aircraft(const aircraft_t* value, overlapped_t * overlapped);
#endif

static inline result_t set_aircraft(const aircraft_t* value, overlapped_t * overlapped)
  {
  return sys_set_aircraft(value, overlapped);
  }

/**
 * Send a trace message to the debugger
 * @param level  * Trace Level
 * 0 = emergency
 * 1 = alert
 * 2 = critical
 * 3 = error
 * 4 = notice
 * 5 = info
 * 6 = debug
 * @param msg  * Printf formatted documentation
 * @return result_t
 * @syscall 2050
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL sys_trace_message(uint16_t level, const char* msg);
#else
extern result_t sys_trace_message(uint16_t level, const char* msg);
#endif

static inline result_t trace_message(uint16_t level, const char* msg)
  {
  return sys_trace_message(level, msg);
  }

/******************************************************************************/
/* Category: fms -                                                            */
/******************************************************************************/

/**
 * @note This call may use overlapped I/O.
 * @param handle [in,out]  * on the first call this is nullptr, it must then be returned on all subsequent calls.
 * close_handle must be called when the enumeration is over
 * @param name [out]  * Name of the flight plan. ULength of buffer must be CFG_NAME_MAX
 * @param comment [out]  * Comment of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param description [out]  * Description of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param type [out]  * Type of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param active [out]  * True if this is the active flightplan
 * @param cookie [out]  * Cookie to pass to enumerate_flight_plan to get the details of the
 * flightplan.
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2560
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL fms_enum_flight_plans(handle_t* handle, char* name, char* comment, char* description, char* type, bool* active, uint32_t* cookie, overlapped_t * overlapped);
#else
extern result_t fms_enum_flight_plans(handle_t* handle, char* name, char* comment, char* description, char* type, bool* active, uint32_t* cookie, overlapped_t * overlapped);
#endif

/**
 * @note This call may use overlapped I/O.
 * @param cookie [out]  * if this syscall returns s_ok then this is the active flight plan id
 * @param name [out]  * Name of the flight plan. ULength of buffer must be CFG_NAME_MAX
 * @param comment [out]  * Name of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param description [out]  * Name of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param type [out]  * Name of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2561
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL fms_get_active_flightplan(uint32_t* cookie, char* name, char* comment, char* description, char* type, overlapped_t * overlapped);
#else
extern result_t fms_get_active_flightplan(uint32_t* cookie, char* name, char* comment, char* description, char* type, overlapped_t * overlapped);
#endif

/**
 * @note This call may use overlapped I/O.
 * @param cookie  * Cookie of the flightplan to make active
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2562
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL fms_set_active_flightplan_number(uint32_t cookie, overlapped_t * overlapped);
#else
extern result_t fms_set_active_flightplan_number(uint32_t cookie, overlapped_t * overlapped);
#endif

/**
 * @note This call may use overlapped I/O.
 * @param cookie [out]  * Cookie of the flightplan that was inverted (the original is not changed)
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2563
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL fms_invert_active_flightplan(uint32_t* cookie, overlapped_t * overlapped);
#else
extern result_t fms_invert_active_flightplan(uint32_t* cookie, overlapped_t * overlapped);
#endif

/**
 * @note This call may use overlapped I/O.
 * @param handle [in,out]  * on the first call this is nullptr, it must then be returned on all subsequent calls.
 * close_handle must be called when the enumeration is over
 * @param cookie  * This is the cookie given by the sys_enum_flight_plans or sys_get_active_flightplan
 * @param id [out]  * The id of the route point that is being returned on this call
 * @param sequence [out]  * The sequence number of the route point. No guarantee that the returned
 * points are in this order
 * @param position [out]  * Position and elevation of the waypoint
 * @param type [out]  * This is the waypoint type enumeration.
 * @param name [out]  * Name of the flight plan. ULength of buffer must be CFG_NAME_MAX
 * @param comment [out]  * Comment of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param description [out]  * Description of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2564
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL fms_enum_flight_plan(handle_t* handle, uint32_t cookie, uint32_t* id, uint32_t* sequence, lla_t* position, uint32_t* type, char* name, char* comment, char* description, overlapped_t * overlapped);
#else
extern result_t fms_enum_flight_plan(handle_t* handle, uint32_t cookie, uint32_t* id, uint32_t* sequence, lla_t* position, uint32_t* type, char* name, char* comment, char* description, overlapped_t * overlapped);
#endif

/**
 * @note This call may use overlapped I/O.
 * @param handle [in,out]  * on the first call this is nullptr, it must then be returned on all subsequent calls.
 * close_handle must be called when the enumeration is over
 * @param filter  * This parameter allows a filter to be used to select possible
 * waypoint matches. The filter can have wild card characters of * and ?
 * Is only used when the handle parameter is nullptr, otherwise it is ignored
 * on subsequenxt calls
 * @param id [out]  * The id of the route point that is being returned on this call
 * @param sequence [out]  * The sequence number of the route point. No guarantee that the returned
 * points are in this order
 * @param position [out]  * Position of the waypoint
 * @param type [out]  * This is the waypoint type enumeration.
 * @param name [out]  * Name of the waypoint. ULength of buffer must be CFG_NAME_MAX
 * @param comment [out]  * Comment of the waypoint. ULength of buffer must be CFG_STRING_MAX
 * @param description [out]  * Description of the waypoint. ULength of buffer must be CFG_STRING_MAX
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2565
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL fms_enum_waypoints(handle_t* handle, const char * filter, uint32_t* id, uint32_t* sequence, lla_t* position, uint32_t* type, char* name, char* comment, char* description, overlapped_t * overlapped);
#else
extern result_t fms_enum_waypoints(handle_t* handle, const char * filter, uint32_t* id, uint32_t* sequence, lla_t* position, uint32_t* type, char* name, char* comment, char* description, overlapped_t * overlapped);
#endif

/**
 * @note This call may use overlapped I/O.
 * @param handle [in,out]  * on the first call this is nullptr, it must then be returned on all subsequent calls.
 * close_handle must be called when the enumeration is over
 * @param filter  * This parameter allows a filter to be used to select possible
 * waypoint matches. The filter can have wild card characters of * and ?
 * Is only used when the handle parameter is nullptr, otherwise it is ignored
 * on subsequenxt calls
 * @param distance  * Optional distance in meters from the current position of the aircraft
 * to search for. If 0 then the distance is a system default in the
 * kHUB system
 * @param wpt_type_filter  * This allows for finding a specific type of nearest waypoint. if the
 * value is WPT_TYPE_UNSPECIFIED then all types are returned
 * @param id [out]  * The id of the route point that is being returned on this call
 * @param sequence [out]  * The sequence number of the route point. No guarantee that the returned
 * points are in this order
 * @param position [out]  * Position of the waypoint
 * @param type [out]  * This is the waypoint type enumeration.
 * @param name [out]  * Name of the waypoint. ULength of buffer must be CFG_NAME_MAX
 * @param comment [out]  * Comment of the waypoint. ULength of buffer must be CFG_STRING_MAX
 * @param description [out]  * Description of the waypoint. ULength of buffer must be CFG_STRING_MAX
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2566
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL fms_enum_nearest(handle_t* handle, const char * filter, uint32_t distance, uint32_t wpt_type_filter, uint32_t* id, uint32_t* sequence, lla_t* position, uint32_t* type, char* name, char* comment, char* description, overlapped_t * overlapped);
#else
extern result_t fms_enum_nearest(handle_t* handle, const char * filter, uint32_t distance, uint32_t wpt_type_filter, uint32_t* id, uint32_t* sequence, lla_t* position, uint32_t* type, char* name, char* comment, char* description, overlapped_t * overlapped);
#endif

/**
 * @note This call may use overlapped I/O.
 * @param wpt  * Go to the waypoint indicated by the wpt id.
 * @param autopilot_enable  * Enable the autopilot modes
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2567
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL fms_goto_waypoint(uint32_t wpt, uint32_t autopilot_enable, overlapped_t * overlapped);
#else
extern result_t fms_goto_waypoint(uint32_t wpt, uint32_t autopilot_enable, overlapped_t * overlapped);
#endif

/**
 * @note This call may use overlapped I/O.
 * @param position  * Position of the waypoint
 * @param type  * This is the waypoint type enumeration.
 * @param name  * Name of the flight plan. ULength of buffer must be CFG_NAME_MAX
 * @param comment  * Name of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param description  * Name of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param id [out]  * If a new waypoint is added, this is the waypoint id of the
 * new waypoint
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2568
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL fms_add_waypoint(const lla_t* position, uint32_t type, const char* name, const char* comment, const char* description, int32_t* id, overlapped_t * overlapped);
#else
extern result_t fms_add_waypoint(const lla_t* position, uint32_t type, const char* name, const char* comment, const char* description, int32_t* id, overlapped_t * overlapped);
#endif

/**
 * @note This call may use overlapped I/O.
 * @param id  * This is the id of the waypoint to delete
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2569
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL fms_delete_waypoint(int32_t id, overlapped_t * overlapped);
#else
extern result_t fms_delete_waypoint(int32_t id, overlapped_t * overlapped);
#endif

/**
 * Remove a flightplan from the fms
 * @note This call may use overlapped I/O.
 * @param id  * Id of the flightplan to remove.  It it is the active flightplan and the
 * autopilot is active then this call will fail
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2570
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL fms_delete_flightplan(int32_t id, overlapped_t * overlapped);
#else
extern result_t fms_delete_flightplan(int32_t id, overlapped_t * overlapped);
#endif

/**
 * Add a new flightplan to the FMS and allocate an ID to it
 * @note This call may use overlapped I/O.
 * @param name  * Name of the flight plan. ULength of buffer must be CFG_NAME_MAX
 * @param comment  * Name of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param description  * Name of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param type  * Name of the flight plan. ULength of buffer must be CFG_STRING_MAX
 * @param len  * Number of waypoints to add
 * @param waypoints  * List of waypoints to add to the flightplan.  The order of the waypoints is kept
 * and will have route sequence numbers added to them
 * @param cookie [out]  * if this syscall returns s_ok then this is the flight plan id
 * @param overlapped  * If provided then the call returns immediately and the id_overlapped message will be
 * called when the operation completes
 * @return result_t
 * @syscall 2571
 */
#ifndef PIC32_BUILD
extern SYSCALL result_t STDCALL fms_add_flightplan(const char* name, const char* comment, const char* description, const char* type, uint32_t len, const uint32_t* waypoints, uint32_t* cookie, overlapped_t * overlapped);
#else
extern result_t fms_add_flightplan(const char* name, const char* comment, const char* description, const char* type, uint32_t len, const uint32_t* waypoints, uint32_t* cookie, overlapped_t * overlapped);
#endif

/******************************************************************************/
/* User-mode library functions (not syscalls)                                 */
/******************************************************************************/

/**
 * Dispatch a message to the correct window procedure.
 * Retrieves the wndproc via get_wndproc (syscall) and calls it in user space.
 * @param hwnd Window handle
 * @param msg Message to dispatch
 * @return Result from the window procedure
 */
extern result_t dispatch_message(handle_t hwnd, const canmsg_t *msg);

/**
 * Default window message processing. Handles id_paint (calls begin_paint,
 * invokes paint handler, calls end_paint), focus changes, and other
 * standard window messages.
 * @param wnd Window handle
 * @param msg Message to process
 * @param wnddata Window data
 * @return Result of processing
 */
extern result_t defwndproc(handle_t wnd, const canmsg_t *msg, void *wnddata);

/**
 * Send a message synchronously to a window. Finds the target window's
 * wndproc and calls it directly without going through the message queue.
 * @param hwnd Window handle (0 to broadcast)
 * @param msg Message to send
 * @return Result from the window procedure
 */
extern result_t send_message(handle_t hwnd, const canmsg_t *msg);

/**
 * Check for dialog control messages and handle tab navigation.
 * @param hwnd Window handle
 * @param msg Message to check
 * @return s_ok if the message was handled
 */
extern result_t is_dialog_message(handle_t hwnd, const canmsg_t *msg);

#ifdef __cplusplus
}
#endif

#endif
