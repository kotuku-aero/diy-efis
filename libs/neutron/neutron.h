#ifndef __neutron_h__
#define __neutron_h__

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

// used when structures are stored in some
// sort of file.  Allows for packed structures
#ifdef _WIN32
#define PACKED
#pragma pack(push, 1)
#endif
#ifdef __GNUC__
#define PACKED __attribute__((__packed__))
#endif


/**
 * @file neutron.h
 * Neutron micro kernel definitions
 */
  /**
 * @brief Generic error value
*/
  typedef int result_t;
  /**
   * @brief handle to a configuration memory key
  */
  typedef uint16_t memid_t;
  /**
   * @brief Handle to an object
  */
  typedef void *handle_t;

  /**
   * @struct canmsg_t
   * This is the message that is passed around the CanFly infrastructure
   * defined by Neutron
   * @param flags	Length, reply, ID
   * @param canas Encoded canas_msg_t
   */
  typedef struct _canmsg_t
    {
    // Bits 15:12 - Length
    // Bit 11 - binary data (used by a pipe)
    // bits 10-0 ID
    uint16_t flags;
    uint8_t data[8];
    } canmsg_t;

typedef uint8_t byte_t;

#ifndef __cplusplus
#define nullptr ((void *)0)
#endif

/**
 * @brief structure type for a time.
*/
typedef struct _tm_t {
  uint16_t year;
  uint16_t month;
  uint16_t day;
  uint16_t hour;
  uint16_t minute;
  uint16_t second;
  uint16_t milliseconds;
  } tm_t;

enum {
  s_orphaned = 3,
  s_dropped = 2,
  s_relocated = 1,
  s_ok = 0,
  s_false = -1,
  e_unexpected = -2,
  e_invalid_handle = -3,
  e_not_implemented = -4,
  e_bad_pointer = -5,
  e_bad_parameter = -6,
  e_more_data = -7,
  e_no_more_information = -8,
  e_path_not_found = -9,
  e_operation_pending = -10,
  e_operation_cancelled = -11,
  e_invalid_operation = -12,
  e_buffer_too_small = -13,
  e_generic_error = -14,
  e_timeout = -15,
  e_no_space = -16,
  e_not_enough_memory = -17,
  e_not_found = -18,
  e_bad_handle = -19,
  e_bad_ioctl = -20,
  e_ioctl_buffer_too_small = -21,
  e_not_initialized = -22,
  e_exists = -23,
  e_wrong_type = -24,
  e_parse_error = -25,
  e_incomplete_command = -26,
  e_bad_type = -27,
  e_not_supported = -28,
  e_out_of_range = -29,
  e_object_released = -30,
  e_unsupported_instruction = -31,
  e_overflow = -32,
  e_stream_closed = -33,
  e_corrupt = -34,
  e_not_directory = -35,
  e_not_file = -36,
  e_not_empty = -37,
  e_file_too_large = -38,
  e_no_attributes = -39,
  };

static inline bool failed(result_t r)
  {
  return r < 0;
  }

static inline bool succeeded(result_t r)
  {
  return r >= 0;
  }

// Un-used
#define unused_id 0

// these are the default node settings

#define CANFLYID(id, num, type, descr) id = num,

enum canfly_id {
#include "../../resources/CanFlyID.def"
  };

#undef CANFLYID

// enumeration for a status field
typedef enum _e_board_status {
  bs_unknown = 0,
  bs_starting = 1,
  bs_running = 2,
  bs_inhibited = 3,
  bs_fault = 16,
  bs_bootrequest = 128,         // is 
  } e_board_status;

// A Status message is sent from each node.  Only one status message ID is used
// and the following bitfields are defined
typedef struct _status_msg_t {
  uint8_t msg_type;             // set to CANFLY_BINARY
  uint8_t node_id;              // is well known, or if kMAG can be the same id initially
  uint8_t board_status;         // see enum above
  uint8_t board_type;           // type of board
  uint32_t serial_number;       // board serial
} status_msg_t;

// this is the message used to set a node's id
typedef struct _set_node_id_msg_t {
  uint8_t msg_type;             // set to CANFLY_BINARY
  uint32_t serial_number;       // device to set
  uint8_t existing_id;          // must match
  uint8_t new_id;               // and set to
  uint8_t new_subtype;          // allows setting the subtype as well
  } set_node_id_msg_t;

// there are up to 16 LU's 0..14 are available
#define scan_id                     15          // serial adapter for CanFLY

// service types.  There can be 16 services per node
#define service_console 0           // all nodes support a console
#define service_neutron 1           // all nodes support the neutron protobuffer config api
#define service_node 15             // node specific service.  used to set custom
                                    // services, for example kmag, khub and kedu
                                    // all implement 15, but use different proto
                                    // definitions

// these are used for the canflyID mapping
#define CANFLY_NODATA 0         // No data in the message
#define CANFLY_ERROR 1          // An error code is in the message
#define CANFLY_UINT8 2          // An unsigned integer
#define CANFLY_INT8 3           // An integer
#define CANFLY_UINT16 4         // A 16 bit integer
#define CANFLY_INT16 5          // A signed 16 bit integer
#define CANFLY_UINT32 6         // An unsigned 32 bit integer
#define CANFLY_INT32 7          // A signed 32 bit integer
#define CANFLY_BOOL_TRUE 8       // A true flag
#define CANFLY_BOOL_FALSE 9     // a false flag
#define CANFLY_FLOAT 10          // A floating point number
#define CANFLY_UTC 11           // an encoded UTC time, seconds since 2000-01-01

#define CANFLY_BINARY 0xFF      // the id is a binary type.

#define CANFLY_BOOL 0x100       // only used to send a flag, never sent

#define LENGTH_MASK 0xF000
#define ID_MASK 0x07FF
#define BINARY_MASK 0x08000

////////////////////////////////////////////////////////////////////////////////////
//
//  File system support.
//

// Maximum name size in bytes, may be redefined to reduce the size of the
// info struct. Limited to <= 1022. Stored in superblock and must be
// respected by other littlefs drivers.
#ifndef FILE_NAME_MAX
#define FILE_NAME_MAX 255
#endif

// Maximum size of a file in bytes, may be redefined to limit to support other
// drivers. Limited on disk to <= 4294967296. However, above 2147483647 the
// functions lfs_file_seek, lfs_file_size, and lfs_file_tell will return
// incorrect values due to using signed integers. Stored in superblock and
// must be respected by other littlefs drivers.
#ifndef FILE_LENGTH_MAX
#define FILE_LENGTH_MAX 2147483647
#endif

// Maximum size of custom attributes in bytes, may be redefined, but there is
// no real benefit to using a smaller LFS_ATTR_MAX. Limited to <= 1022.
#ifndef MAX_FILE_ATTR
#define MAX_FILE_ATTR 1022
#endif

struct _filesystem_t;
typedef struct _filesystem_t filesystem_t;

#define FILE_TYPE_DIR 0x02
#define FILE_TYPE_FILE 0x01

typedef struct _stat_t {
  // Set when the file is a directory
  uint8_t type;

  // Size of the file, only valid for files.
  uint32_t size;

  // Name of the file stored as a null-terminated string. Limited to
  // FILE_NAME_MAX+1, which can be changed by redefining FILE_NAME_MAX to
  // reduce RAM. FILE_NAME_MAX is stored in superblock and must be
  // respected by other drivers.
  char name[FILE_NAME_MAX + 1];

  } stat_t;

// open flags
// Open a file as read only
#define STREAM_O_RD 0x0001
// Open a file as write only
#define STREAM_O_WR 0x0002
// read-write stream
#define STREAM_O_RDWR 0x0003
// Create a file if it does not exist
#define STREAM_O_CREAT 0x0100   
// Fail if a file already exists
#define STREAM_O_EXCL 0x0200    
// Truncate the existing file to zero size
#define STREAM_O_TRUNC 0x0400   
// Move to end of file on every write
#define STREAM_O_APPEND 0x0800

// Seek relative to an absolute position
#define STREAM_SEEK_SET 0
// Seek relative to the current file position
#define STREAM_SEEK_CUR 1
// Seek relative to the end of the file
#define STREAM_SEEK_END 2


/**
 * @brief Set the length of a CANbus message
 * @param msg Message
 * @param len Length to assign
 * @remark This method should only be called for internal
 * messages (canID >= 1400 && canID < 1520) as the
 * published ID's all have very specific uses.
*/
static inline void set_can_len(canmsg_t *msg, uint16_t len)
  {
  msg->flags &= ~LENGTH_MASK;
  msg->flags |= (len << 12) & LENGTH_MASK;
  }
/**
 * @brief Set the ID of a CANbus message
 * @param msg Message
 * @param id ID to assign
 * @remark This method should only be called for internal
 * messages (canID >= 1400 && canID < 1520) as the
 * published ID's all have very specific uses.
 * Use the create_can_msg_<datatype> functions to
 * create defined can messages
*/
static inline void set_can_id(canmsg_t *msg, uint16_t id)
  {
  msg->flags &= ~ID_MASK;
  msg->flags |= (id & ID_MASK);
  }
/**
 * @brief Get the length of a CANbus message
 * @param msg message
 * @return length
 * @remark The length of data types (can_id < 1520) is one more
 * than the length of the actual type.  The canfly protocol puts
 * the type of the data as the first byte of the message, followed
 * by a big endian encoded data field.
*/
static inline uint8_t get_can_len(const canmsg_t *msg)
  {
  return (uint8_t)((msg->flags & LENGTH_MASK) >> 12);
  }
/**
 * @brief Return the CANbus ID of the message
 * @param msg Message
 * @return ID of the message
*/
static inline uint16_t get_can_id(const canmsg_t *msg)
  {
  return msg->flags & ID_MASK;
  }
/**
 * @brief Return true if the can message is a binary format
 * @param msg Message
 * @return true if the message is a binary message
 * @remark This is returned true for all can ID's greater
 * that 1520 which is used by the pipe messages.
*/
static inline bool get_can_is_binary(const canmsg_t *msg)
  {
  return msg->flags * BINARY_MASK;
  }
/**
 * @brief Set a can message as binary
 * @param msg Message
 * @param is_it true if a binary message
*/
static inline void set_can_is_binary(canmsg_t *msg, bool is_it)
  {
  msg->flags &= ~BINARY_MASK;
  if (is_it)
    msg->flags |= BINARY_MASK;
  }
/**
 * @brief Return the encoded type of a message
 * @param msg Message
 * @return One of the CANFLY_ data types
*/
static inline uint16_t get_can_type(const canmsg_t *msg)
  {
  if ((msg->flags & BINARY_MASK) != 0)
    return CANFLY_BINARY;
  // non binary messages store the data type as the
  // first byte of the message
  switch(msg->data[0])
    {
    case CANFLY_BOOL_FALSE :
    case CANFLY_BOOL_TRUE :
      return CANFLY_BOOL;
    default:
      return msg->data[0];
    }
  }

/**
 * @brief create a status message
 * @param msg   message to create
 * @param node_id   Id of the node
 * @param node_type Type of node
 * @param status    Running status
 * @return s_ok if created ok
 */
extern result_t create_can_msg_status(canmsg_t *msg,
                               uint8_t node_id,
                               uint8_t node_type,
                               e_board_status status);
/**
 * @brief Decode an id_status message
 * @param msg       message
 * @param node_id   sender node id (0..14)
 * @param node_type Type of node (id_ahrs_id.. etc)
 * @param serial_number  Serial number of the board
 * @param status    Status of the board
 * @return 
 */
extern result_t get_param_status(const canmsg_t *msg,
                          uint8_t *node_id,
                          uint8_t *node_type,
                          uint32_t *serial_number,
                          e_board_status *status);

static inline bool is_status_msg(const canmsg_t *msg)
  {
  return get_can_id(msg) >= id_status_node_0 && get_can_id(msg) <= id_status_node_15;
  }

/**
 * @brief Create a can message sending an boolean value
 * @param msg           Message to construct
 * @param message_id    11 bit CanFly ID
 * @param data          Data to send
 *@return s_ok if created ok
 */
extern result_t create_can_msg_bool(canmsg_t *msg, uint16_t message_id, bool value);

/**
 * @fn const canmsg_t *create_can_msg_int8(canmsg_t *msg, uint16_t message_id, int8_t value)
 * @brief Create a can message sending an 8bit integer
 * @param msg           Message to construct
 * @param message_id    11 bit CanFly ID
 * @param data          Data to send
 *@return s_ok if created ok
 */
extern result_t create_can_msg_int8(canmsg_t *msg, uint16_t message_id, int8_t value);
/**
 * @fn const canmsg_t *create_can_msg_uint8(canmsg_t *msg, uint16_t message_id, uint8_t value)
 * @brief Create a can message sending an 8bit unsigned integer
 * @param msg           Message to construct
 * @param message_id    11 bit CanFly ID
 * @param value          Data to send
 *@return s_ok if created ok
 */
 extern result_t create_can_msg_uint8(canmsg_t *msg, uint16_t message_id, uint8_t value);
/**
 * @fn const canmsg_t *create_can_msg_int16(canmsg_t *msg, uint16_t message_id, int16_t data)
 * Create a can message sending a 16bit integer
 * @param msg           Message to construct
 * @param message_id    11 bit CanFly ID
 * @param data          Data to send
 *@return s_ok if created ok
 */
 extern result_t create_can_msg_int16(canmsg_t *msg, uint16_t message_id, int16_t data);
/**
 * @fn const canmsg_t *create_can_msg_uint16(canmsg_t *msg, uint16_t message_id, uint16_t data)
 * Create a can message sending a 16bit unsigned integer
 * @param msg           Message to construct
 * @param message_id    11 bit CanFly ID
 * @param data          Data to send
 *@return s_ok if created ok
 */
 extern result_t create_can_msg_uint16(canmsg_t *msg, uint16_t message_id, uint16_t data);
/**
 * @fn create_can_msg_int32(canmsg_t *msg, uint16_t message_id, int32_t data)
 * Create a can message sending a 32bit integer
 * @param msg           Message to construct
 * @param message_id    11 bit CanFly ID
 * @param data          Data to send
 *@return s_ok if created ok
 */
 extern result_t create_can_msg_int32(canmsg_t *msg, uint16_t message_id, int32_t data);
/**
 * @fn create_can_msg_uint32(canmsg_t *msg, uint16_t message_id, uint32_t data)
 * Create a can message sending a 32bit unsigned integer
 * @param msg           Message to construct
 * @param message_id    11 bit CanFly ID
 * @param data          Data to send
 *@return s_ok if created ok
 */
 extern result_t create_can_msg_uint32(canmsg_t *msg, uint16_t message_id, uint32_t data);
/**
 * @fn create_can_msg_float(canmsg_t *msg, uint16_t message_id, float data)
 * Create a can message sending a floating point value
 * @param msg           Message to construct
 * @param message_id    11 bit CanFly ID
 * @param data          Data to send
 *@return s_ok if created ok
 */
 extern result_t create_can_msg_float(canmsg_t *msg, uint16_t message_id, float data);
/**
 * @fn create_can_msg_utc(canmsg_t *msg, uint16_t message_id, const tm_t *value)
 * @brief Create a can message sending a utc time
 * @param msg           Message to construct
 * @param message_id    11 bit CanFly ID
 * @param data          Data to send
 *@return s_ok if created ok
 */
 extern result_t create_can_msg_utc(canmsg_t *msg, uint16_t message_id, const tm_t *value);
 /**
  * @brief create an empty message
  * @param msg Message to constuct
  * @param message_id 11 bit CanFly ID
  * @return s_ok if created ok
 */
 extern result_t create_can_msg_nodata(canmsg_t *msg, uint16_t message_id);
 /**
  * @brief Create a can message with an error code
 * @param msg           Message to construct
 * @param message_id    11 bit CanFly ID
  * @param error        Error to send
  * @return  s_ok if created ok
 */
 extern result_t create_can_msg_error(canmsg_t *msg, uint16_t message_id, uint32_t error);
/**
 * @fn get_param_int8(const canmsg_t *msg, int8_t *value)
 * @param msg         Message to extract parameter from
 * @param value       value of the message
 * @return s_ok if the message can be coerced to an int8, and the data is returned
 */
 extern result_t get_param_bool(const canmsg_t *msg, bool *value);
/**
 * @fn get_param_int8(const canmsg_t *msg, int8_t *value)
 * @param msg         Message to extract parameter from
 * @param value       value of the message
 * @return s_ok if the message can be coerced to an int8, and the data is returned
 */
 extern result_t get_param_int8(const canmsg_t *msg, int8_t *value);
/**
* @fn get_param_uint8(const canmsg_t *msg, uint8_t *value)
* @param msg         Message to extract parameter from
* @param value       extracted value
* @return s_ok if the message can be coerced to an uint8, and the data is returned
*/
extern result_t get_param_uint8(const canmsg_t *msg, uint8_t *value);
/**
 * @fn get_param_int16(const canmsg_t *msg, int16_t *value)
 * @param msg         Message to extract parameter from
 * @param value       extracted value
 * @return s_ok if the message can be coerced to an int16, and the data is returned
 */
 extern result_t get_param_int16(const canmsg_t *msg, int16_t *value);
/**
 * @fn get_param_uint16(const canmsg_t *msg, uint16_t *value)
 * @param msg         Message to extract parameter from
 * @param index       Index of the value (0..1)
 * @param value       extracted value
 * @return s_ok if the message can be coerced to an int, and the data is returned
 */
 extern result_t get_param_uint16(const canmsg_t *msg, uint16_t *value);
/**
 * @fn get_param_int32(const canmsg_t *msg, int32_t *value)
 * @param msg         Message to extract parameter from
 * @param value       extracted value
 * @return s_ok if the message can be coerced to an int32, and the data is returned
 */
 extern result_t get_param_int32(const canmsg_t *msg, int32_t *value);
/**
 * @fn get_param_uint32(const canmsg_t *msg, uint32_t *value)
 * @param msg         Message to extract parameter from
 * @param value       extracted value
 * @return s_ok if the message can be coerced to an uint32, and the data is returned
 */
 extern result_t get_param_uint32(const canmsg_t *msg, uint32_t *value);
/**
 * @brief Unpack the UTC message into a time structure
 * @param msg   message to unpack
 * @param value time values, note milliseconds is set to 0
 * @return s_ok if the message was a valid utc message
*/
 extern result_t get_param_utc(const canmsg_t *msg, tm_t *value);
/**
 * @fn get_param_float(const canmsg_t *msg, float *value)
 * @param msg         Message to extract parameter from
 * @param value       extracted value
 * @return s_ok if the message can be coerced to a float, and the data is returned
 */
 extern result_t get_param_float(const canmsg_t *msg, float *value);
//
#define DEFAULT_CAN_TIMEOUT INDEFINITE_WAIT
/**
 * @fn can_send(canmsg_t *)
 * @brief queue a can message to be sent.
 * @param message to send
 * @param timeout milliseconds to wait to send the message
 * @return result_t s_ok if sent OK.
 */
extern result_t can_send(const canmsg_t *msg, uint32_t timeout);
/**
 * @fn publish_local(const canmsg_t *msg, uint32_t timeout)
 * @brief send a message to the local message queue
 * @param msg
 * @param timeout
 * @return 
 */
extern result_t publish_local(const canmsg_t *msg, uint32_t timeout);
/**
 * @brief encapsulated variant
*/
typedef enum _variant_type {
  v_none,
  v_bool,
  v_int8,
  v_uint8,
  v_int16,
  v_uint16,
  v_int32,
  v_uint32,
  v_float,
  v_utc
  } variant_type;

typedef  struct _variant_t {
  variant_type vt;
  union {
    bool boolean;
    int8_t int8;
    uint8_t uint8;
    int16_t int16;
    uint16_t uint16;
    int32_t int32;
    uint32_t uint32;
    float flt;
    tm_t utc;
    } value;
  } variant_t;

extern const variant_t *create_variant_nodata(variant_t *v);
extern const variant_t *create_variant_bool(bool value, variant_t *v);
extern const variant_t *create_variant_int8(int8_t value, variant_t *v);
extern const variant_t *create_variant_uint8(uint8_t value, variant_t *v);
extern const variant_t *create_variant_int16(int16_t value, variant_t *v);
extern const variant_t *create_variant_uint16(uint16_t value, variant_t *v);
extern const variant_t *create_variant_int32(int32_t value, variant_t *v);
extern const variant_t *create_variant_uint32(uint32_t value, variant_t *v);
extern const variant_t *create_variant_float(float value, variant_t *v);
extern const variant_t *create_variant_utc(const tm_t *value, variant_t *v);
extern result_t msg_to_variant(const canmsg_t *msg, variant_t *v);
extern result_t variant_to_msg(const variant_t *v, uint16_t id, uint16_t type, canmsg_t *msg);
extern result_t coerce_to_bool(const variant_t *src, bool *value);
extern result_t coerce_to_int8(const variant_t *src, int8_t *value);
extern result_t coerce_to_uint8(const variant_t *src, uint8_t *value);
extern result_t coerce_to_int16(const variant_t *src, int16_t *value);
extern result_t coerce_to_uint16(const variant_t *src, uint16_t *value);
extern result_t coerce_to_int32(const variant_t *src, int32_t *value);
extern result_t coerce_to_uint32(const variant_t *src, uint32_t *value);
extern result_t coerce_to_float(const variant_t *src, float *value);
extern result_t coerce_to_utc(const variant_t *src, tm_t *value);
extern result_t coerce_variant(const variant_t *src, variant_t *dst, variant_type to_type);
extern const variant_t *copy_variant(const variant_t *src, variant_t *dst);
extern int compare_variant(const variant_t *v1, const variant_t *v2);
extern uint16_t can_type_from_variant(variant_type vt);

#define numelements(a) (sizeof(a) / sizeof(a[0]))

#define NO_WAIT 0
#define INDEFINITE_WAIT 0xFFFFFFFF

typedef void (*task_callback)(void *parg);
/**
 * @fn semaphore_create(handle_t *semp)
 * Create a new semaphore
 * @param semp newly constructed semaphore
 * @return s_ok if resources available to create the semaphore
 */
extern result_t semaphore_create(handle_t *semp);
/**
 * @fn semaphore_signal(handle_t semaphore)
 * signal that an event is set.
 * @param event_mask
 * @return s_ok if signaled ok
 */
extern result_t semaphore_signal(handle_t semaphore);
/**
 * @fn semaphore_wait(handle_t semaphore, uint32_t ticks)
 * Suspend the calling task, waiting for an event
 * @param semaphore semaphore to wait on
 * @param ticks milliseconds to wait for a signal event
 * @returns s_ok if the event was signaled, s_false if timeout
 */
extern result_t semaphore_wait(handle_t semaphore, uint32_t ticks);
/**
 * @brief return s_ok if the semaphore has wait tasks
 * @param semaphore semaphore to query
 * @return s_ok if has tasks, s_false if not, otherwise error
*/
extern result_t has_wait_tasks(handle_t semaphore);

////////////////////////////////////////////////////////////////////////////////
//
// system information functions
//
/**
* @fn neutron_random(uint32_t seed, uint32_t *rand)
* @brief create a random number
* @param seed  optional seed for random number
* @param rand  generated random number
* @return s_ok if available
* @remark this will use a hardware random number if available
*/
extern result_t neutron_random(uint32_t seed, uint32_t *rand);
/**
* @fn sysinfo(uint32_t *machine_id, uint32_t *build)
* @param machine_id serial number of the device
* @param build build number of the operating system
* @param image_name  Name of the image that is running
* @return s_ok if the system is ok
*/
extern result_t sysinfo(uint32_t *machine_id, uint32_t *build, const char **image_name);

////////////////////////////////////////////////////////////////////////////////
//
//  Task functions

#define IDLE_PRIORITY 0
#define BELOW_NORMAL 4
#define NORMAL_PRIORITY 8
#define HIGH_PRIORITY 12

// these are in native stack types (pic32mz it is 32bits)
#define IDLE_STACK_SIZE 1024
#define DEFAULT_STACK_SIZE 1024

extern result_t interlocked_increment(volatile int32_t *value);
extern result_t interlocked_decrement(volatile int32_t *value);

/**
 * @fn task_create(const char *name, uint16_t stack_size, task_callback callback, void *parg, uint8_t priority, handle_t *task)
 * Create a new scheduled task
 * @param name          name of the task
 * @param stack_size    size of stack to allocate (words)
 * @param callback      task main routine
 * @param parg          argument to pass to task
 * @param priority      initial priority
 * @param task          created task handle
 * @return s_ok if task created ok
 */
extern result_t task_create(const char *name, uint16_t stack_size, task_callback callback,
  void *parg, uint8_t priority, handle_t *task);
/**
 * @fn get_current_task()
 * return the handle to the currently executing task
 * @return task handle
 */
extern result_t get_current_task(handle_t *task);
/**
 * @fn set_handle_triority(handle_t task, uint8_t priority)
 * Change the priority of a scheduled task
 * @param task        task to change
 * @param priority    priority to set
 * @return s_ok if priority set
 */
extern result_t set_task_priority(handle_t task, uint8_t priority);
/**
 * @fn get_handle_triority(handle_t task, uint8_t *priority)
 * Return the priority for a task
 * @param task        task to query
 * @param priority    assigned priority
 * @return s_ok if task is valid
 */
extern result_t get_task_priority(handle_t task, uint8_t *priority);
/**
* @fn get_task_name(handle_t task, const char **name)
* @brief Return the name of a task
* @param task        task to query
* @param name        the name
* @return s_ok if task is valid
* @remark the name is the same address passed into task_create
*/
extern result_t get_task_name(handle_t task, const char **name);
/**
 * @fn task_suspend(handle_t task)
 * Suspend a task
 * @param task
 * @return s_ok if suspended
 */
extern result_t task_suspend(handle_t task);
/**
 * @fn task_resume(handle_t task)
 * @brief Resume a suspended task
 * @param task  task to resume
 * @return s_ok if task resumed ok
 */
extern result_t task_resume(handle_t task);
/**
 * @fn task_sleep(uint32_t n)
 * Suspend a task
 * @param n milliseconds to suspend
 * @return s_ok after delay
 */
extern result_t task_sleep(handle_t task, uint32_t n);
/**
 * @fn exit the caller thread
 * @param code code to return if a thread watch is set
 * @return never
 */
extern result_t task_exit(uint32_t code);
/**
 * @fn ticks()
 * return the 1khz tick counter
 * @return
 */
extern result_t ticks(uint32_t *value);
/**
 * @fn yield()
 * This is used to force a context switch.
*/
extern result_t yield();
/**
* @fn now(tm_t *tm)
* This function returns the current time based on a gps time fix.
* @param tm   Where to receive the time.
* @return s_ok if the time is known.
* @remark This function will return e_unexpected if the CanFly messages with the
* utc date and utc time have not been received.
*/
extern result_t now(tm_t *tm);
/**
* @fn settime(const tm_t *tm)
* set the time of the clock.  If a canfly time or date message is received it will update this
* @param tm values to assign
* @return s_ok if a valid time
*/
extern result_t settime(const tm_t *tm);

//////////////////////////////////////////////////////////////////////////////
//
//  Queue Functions
/**
 * @fn deque_create(uint16_t element_size, uint16_t length, handle_t *deque)
 * Create a queue and initialize it
 * @param element_size  Size of elements in the deque
 * @param length        Number of elements in the deque
 * @param deque         handle to the deque
 * @return s_ok if created ok
 */
extern result_t deque_create(uint16_t element_size, uint16_t length, handle_t *deque);
/**
 * @fn can_pop(handle_t deque)
 * return true if there is an item on the queue
 * @param   queue to pop
 * @return  s_true if an item available, s_false if not
 */
extern result_t can_pop(handle_t deque);
/**
 * @fn count(handle_t deque, uint16_t *value)
 * Number of items in the deque
 * @param deque  deque to query
 * @param value  number of items in the deque
 * @result s_ok if a valid queue
 */
extern result_t count(handle_t deque, uint16_t *value);
/**
 * @fn push_back(handle_t deque, const void *item, uint32_t max_wait)
 * Push an item onto the deque.  will block if no space
 * @param deque   queue to push onto
 * @param item    item to copy onto queue
 */
extern result_t push_back(handle_t deque, const void *item, uint32_t max_wait);
/**
 * @fn pop_front(handle_t deque, void *item, uint32_t max_wait)
 * Remove an item from a queue
 * @param deque deque to pop from
 * @param item  if non 0 copy of item
 */
extern result_t pop_front(handle_t deque, void *item, uint32_t max_wait);
/**
 * @fn capacity(handle_t deque, uint16_t *value)
 * Return the number of items that can be in a deque
 * @param deque   Queue to query
 * @param value   number of items that can be added to the queue
 * @return s_ok if a valid deque
 */
extern result_t capacity(handle_t deque, uint16_t *value);

///////////////////////////////////////////////////////////////////////////////
//
//    Registry functions
//
#define REG_NAME_MAX (16)
/**
 * @struct lla_t
 * Lat/Lng altitude parameter type
 * @param lat Lattitude
 * @param lng Longitude
 * @param alt Altitude
 */
typedef struct _lla_t {
  float lat;
  float lng;
  float alt;
  } lla_t;

/**
 * @struct xyz_t
 * Generic Euclidean point
 * @param x X
 * @param y Y
 * @param z Z
 */
typedef struct _xyz_t {
  float x;
  float y;
  float z;
  } xyz_t;

/** @struct qtn_t
 * Quarternion type
 */
typedef struct _qtn_t {
  float w;
  float x;
  float y;
  float z;
  } qtn_t;

/** @struct matrix_t
 * Generic matrix parameter
 * @param v Values
 */
typedef struct _matrix_t {
  float v[3][3];
  } matrix_t;

extern const matrix_t identity_matrix;

extern const xyz_t *matrix_dot(const matrix_t *m, const xyz_t *v, xyz_t *r);
extern const matrix_t *matrix_add(const matrix_t *m, const matrix_t *v, matrix_t *r);
extern const matrix_t *matrix_subtract(const matrix_t *m, const matrix_t *v, matrix_t *r);
extern const matrix_t *matrix_multiply(const matrix_t *m, const matrix_t *v, matrix_t *r);
extern const matrix_t *matrix_divide(const matrix_t *m, const matrix_t *v, matrix_t *r);
extern const matrix_t *matrix_transpose(const matrix_t *m, matrix_t *r);
extern const matrix_t *matrix_exponetiation(const matrix_t *m, matrix_t *r);
extern const matrix_t *matrix_copy(const matrix_t *m, matrix_t *r);

/**
 * @fn reg_create_key(memid_t parent, const char *name, memid_t *key)
 * Create a registry key
 * @param parent    Parent key to create within, 0 for root
 * @param name      Path to the key
 * @param key       Key created
 * @return s_ok if created ok
 * @remarks If the key exists, then is equivalent to an open call
 */
extern result_t reg_create_key(memid_t parent, const char *name, memid_t *key);
/**
 * @fn reg_open_key(memid_t parent, const char *name, memid_t *key)
 * Open a registry key
 * @param parent    parent to open relative to
 * @param name      path to a key
 * @param key       resulting key
 * @return
 */
extern result_t reg_open_key(memid_t parent, const char *name, memid_t *key);

// these are the hdr.data_type values for a field
typedef enum {
  field_none,                     // no definition
  field_key,											// a key is a parent of other fields
  field_bool,                     // a boolean field (see notes)
  field_int8,
  field_uint8,
  field_int16,
  field_uint16,
  field_int32,
  field_uint32,
  field_float,
  field_xyz,
  field_matrix,
  field_string,
  field_qtn,
  field_lla,
  field_stream,                   // a stream field.  needs stream functions
  } field_datatype;
/**
 * @fn reg_enum_key(memid_t key, field_datatype *type, uint16_t *length, void *data, uint16_t len, char *name, memid_t *child)
 * Enumerate the children of a key
 * @param key     Key to enumerate
 * @param type    Type of child key, if not field_none then will restrict result
 * @param length  Length of enumerated key, when called sets the size of the data area if non-0
 * @param data    Data buffer for result if child is not field_key
 * @param len     length of name buffer
 * @param name    buffer for name
 * @param child   Resulting child key, when called if non-0 then the next key is used
 * @return s_ok if key found ok
 *
 * @example
 * // example to enumerate keys:
 * char name[17];
 * field_datatype type = 0;
 * // must be 0 on first call
 * memid_t child = 0;
 * result_t result;
 *
 * while(succeeded(result = reg_enum_key(key, &type(0, 17, name, &child)))
 *    {
 *    // field_datatype has the field type, name is the child name
 *    field_datatype = 0;
 *    }
 *
 */
extern result_t reg_enum_key(memid_t key, field_datatype *type, uint16_t *length, void *data, 
  uint16_t len, char *name, memid_t *child);
/**
 * @fn reg_delete_key(memid_t key)
 * Remove a key, all sub-keys and values
 * @param key Key to remove
 * @return s_ok if completed ok
 */
extern result_t reg_delete_key(memid_t key);
/**
 * @fn reg_delete_value(memid_t parent, const char *name)
 * Remove a value
 * @param parent  Parent key to remove from
 * @param name    Name of the value to remove
 * @return
 */
extern result_t reg_delete_value(memid_t parent, const char *name);
/**
 * @fn reg_query_memid(memid_t entry, field_datatype *type, char *name, uint16_t *length, memid_t *parent)
 * Return the information about a memid
 * @param entry   memid to query
 * @param type    Returned type of the entry
 * @param name    Name of the entry must be REG_NAME_MAX length
 * @param length  Length of the entry
 * @param parent  Memid of the parent of the key
 * @return s_ok if a valid memid.
 */
extern result_t reg_query_memid(memid_t entry, field_datatype *type, char *name, uint16_t *length, memid_t *parent);
/**
 * @fn reg_query_child(memid_t key, const char *entry, memid_t *memid, field_datatype *type, uint16_t *length)
* Return the information about a child
* @param key     parent key
* @param entry   name of entry to query
* @param type    Returned type of the entry
* @param name    Name of the entry
* @param length  Length of the entry
* @param memid  Memid of the child
* @return s_ok if a valid memid.
*/
extern result_t reg_query_child(memid_t key, const char *entry, memid_t *memid, field_datatype *type, uint16_t *length);
/**
 * @fn reg_rename_value(memid_t parent, const char *name, const char *new_name)
 * Rename a value
 * @param parent      Parent key
 * @param name        Value name
 * @param new_name    New Name
 * @return s_ok if the value was renamed
 */
extern result_t reg_rename_value(memid_t parent, const char *name, const char *new_name);
/**
 * @fn reg_rename_value(memid_t key, const char *new_name)
 * Rename a key
 * @param parent      Key to rename
 * @param new_name    New Name
 * @return s_ok if the value was renamed
 */
extern result_t reg_rename_key(memid_t key, const char *new_name);
  /**
 * @fn reg_get_int8(memid_t parent, const char *name, int8_t *result)
 * Read a int8_t setting from the registry
 * @param parent        ID of the setting
 * @param result        Where to put the word read
 * @return true if the setting can be read
 */
extern result_t reg_get_int8(memid_t parent, const char *name, int8_t *result);
/**
 * @fn reg_set_int8(memid_t parent, const char *name, int8_t value)
 * Write a int8_t setting
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_int8(memid_t parent, const char *name, int8_t value);
/**
 * @fn reg_get_uint8(memid_t parent, const char *name, uint8_t *result)
 * Read a uint8_t setting from the registry
 * @param memid        ID of the setting
 * @param result        Where to put the word read
 * @return true if the setting can be read
 */
  extern result_t reg_get_uint8(memid_t parent, const char *name, uint8_t *result);
/**
 * @fn reg_set_uint8(memid_t parent, const char *name, uint8_t value)
 * Write a uint8_t setting
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_uint8(memid_t parent, const char *name, uint8_t value);
/**
 * @fn  reg_get_int16(memid_t parent, const char *name, int16_t *result)
 * Read a int16_t setting from the registry
 * @param parent        ID of the setting
 * @param result        Where to put the word read
 * @return true if the setting can be read
 */
extern result_t reg_get_int16(memid_t parent, const char *name, int16_t *result);
/**
 * @fn reg_set_int16(memid_t parent, const char *name, int16_t value)
 * Write a int16_t setting
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_int16(memid_t parent, const char *name, int16_t value);
/**
 * @fn reg_get_uint16(memid_t parent, const char *name, uint16_t *result)
 * Read a uint16_t setting from the registry
 * @param memid        ID of the setting
 * @param result        Where to put the word read
 * @return true if the setting can be read
 */
extern result_t reg_get_uint16(memid_t parent, const char *name, uint16_t *result);
/**
 * @fn reg_set_uint16(memid_t parent, const char *name, uint16_t value)
 * Write a uint16_t setting
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_uint16(memid_t parent, const char *name, uint16_t value);
/**
 * @fn reg_get_int32(memid_t parent, const char *name, int32_t *result)
 * Read a int32_t setting from the registry
 * @param memid        ID of the setting
 * @param result        Where to put the word read
 * @return true if the setting can be read
 */
extern result_t reg_get_int32(memid_t parent, const char *name, int32_t *result);
/**
 * @fn reg_set_int32(memid_t parent, const char *name, int32_t value)
 * Write a int32_t setting
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_int32(memid_t parent, const char *name, int32_t value);
/**
 * @fn reg_get_uint32(memid_t parent, const char *name, uint32_t *result)
 * Read a uint32_t setting from the registry
 * @param memid        ID of the setting
 * @param result        Where to put the word read
 * @return true if the setting can be read
 */
extern result_t reg_get_uint32(memid_t parent, const char *name, uint32_t *result);
/**
 * @fn reg_set_uint32(memid_t parent, const char *name, uint32_t value)
 * Write a uint32_t setting
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_uint32(memid_t parent, const char *name, uint32_t value);
/**
 * @fn reg_get_lla(memid_t parent, const char *name, lla_t *result)
 * Read a lla_t setting from the registry
 * @param memid        ID of the setting
 * @param result        Where to put the word read
 * @return true if the setting can be read
 */
extern result_t reg_get_lla(memid_t parent, const char *name, lla_t *result);
/**
 * @fn reg_set_lla(memid_t parent, const char *name, const lla_t *value)
 * Write a lla_t setting
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_lla(memid_t parent, const char *name, const lla_t *value);
/**
 * @fn reg_get_xyz(memid_t parent, const char *name, xyz_t *result)
 * Read a xyz_t setting from the registry
 * @param memid        ID of the setting
 * @param result        Where to put the word read
 * @return true if the setting can be read
 */
extern result_t reg_get_xyz(memid_t parent, const char *name, xyz_t *result);
/**
 * @fn reg_set_xyz(memid_t parent, const char *name, const xyz_t *value)
 * Write a xyz_t setting
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_xyz(memid_t parent, const char *name, const xyz_t *value);
/**
 * @fn reg_get_matrix(memid_t parent, const char *name, matrix_t *result)
 * Read a matrix_t setting from the registry
 * @param memid        ID of the setting
 * @param result        Where to put the word read
 * @return true if the setting can be read
 */
extern result_t reg_get_matrix(memid_t parent, const char *name, matrix_t *result);
/**
 * @fn reg_set_matrix(memid_t parent, const char *name, matrix_t *value)
 * Write a matrix_t setting
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_matrix(memid_t parent, const char *name, matrix_t *value);
/**
 * @fn reg_get_qtn(memid_t parent, const char *name, qtn_t *result)
 * Read a qtn_t setting from the registry
 * @param memid        ID of the setting
 * @param result        Where to put the word read
 * @return true if the setting can be read
 */
extern result_t reg_get_qtn(memid_t parent, const char *name, qtn_t *result);
/**
 * @fn reg_set_qtn(memid_t parent, const char *name, const qtn_t *value)
 * Write a qtn_t setting
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_qtn(memid_t parent, const char *name, const qtn_t *value);

// all registry strings are quite small, but fast
#define REG_STRING_MAX 36
/**
 * @fn reg_get_string(memid_t parent, const char *name, char *value, uint16_t *length)
 * Read a string from the registry
 * @param parent          Parent key
 * @param name            name of the setting
 * @param value           buffer with the string, if 0 then actual length returned in length
 * @param length          length of buffer if defined, otherwise where length returned
 * @return s_ok if string read
 */
extern result_t reg_get_string(memid_t parent, const char *name, char *value, uint16_t *length);
/**
 * @fn reg_set_string(memid_t parent, const char *name, const char *value)
 * Write a string setting
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_string(memid_t parent, const char *name, const char *value);
/**
 * @fn reg_get_float(memid_t parent, const char *name, float *result)
 * Read a float from the registry
 * @param memid          ID of the setting
 * @param result          value to read
 * @return true if the setting can be read
 */
extern result_t reg_get_float(memid_t parent, const char *name, float *result);
/**
 * @fn reg_set_float(memid_t parent, const char *name, float value)
 * Write a float to the registry
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_float(memid_t parent, const char *name, float value);
/**
 * @fn reg_get_bool(memid_t parent, const char *name, bool *result)
 * Read a bool from the registry
 * @param memid          ID of the setting
 * @param result          value to read
 * @return true if the setting can be read
 */
extern result_t reg_get_bool(memid_t parent, const char *name, bool *result);
/**
 * @fn reg_set_bool(memid_t parent, const char *name, bool value)
 * Write a bool to the registry
 * @param memid          ID of the setting
 * @param value           value to write
 * @return true if the setting can be written
 */
extern result_t reg_set_bool(memid_t parent, const char *name, bool value);
///////////////////////////////////////////////////////////////////////////////
//
//  Serial port communications functions
//
/**
 * @fn comm_create_device(memid_t key, handle_t *device)
 * Open a new comms device
 * @param key       Registry key that holds the setup information
 * @param worker    Semaphore used to block calling thread on
 * @param device    resulting comms device
 * @return s_ok if device is opened ok
 */
extern result_t comm_create_device(memid_t key, handle_t *device);
/**
 * @fn comm_write(handle_t device, const uint8_t *data, uint16_t len, uint32_t timeout)
 * Write a string to the comms device
 * @param device  Device to write to
 * @param data    Character string to write
 * @param len     length of the buffer to write
 * @param timeout max milliseconds to wait to send each byte
 * @return s_ok if written ok
 * @remark this is a blocking call and will only return when the timeout is reached or
 * all characters are sent
 */
extern result_t comm_write(handle_t device, const uint8_t *data, uint16_t len, uint32_t timeout);
/**
 * @fn comm_read(handle_t device, uint8_t *data, uint16_t len, uint16_t *bytes_read, uint32_t timeout)
 * Read bytes from the comms device
 * @param device      Device to read from
 * @param data        Data to read
 * @param len         Length of read buffer
 * @param bytes_read  How many bytes read
 * @param timeout     Max milliseconds to wait for each character
 * @return s_ok if bytes were read
 */
extern result_t comm_read(handle_t device, uint8_t *data, uint16_t len, uint16_t *bytes_read, uint32_t timeout);

typedef enum _ioctl_type
  {
  set_device_ctl,
  get_device_ctl
  } ioctl_type;
/**
 * @fn comm_ioctl(handle_t device, ioctl_type type, const void *in_buffer,
    uint16_t in_buffer_size, void *out_buffer, uint16_t out_buffer_size,
               uint16_t *size_returned)
 * Perform an IOCTL on the device
 * @param device            Device to control
 * @param type              Type of ioctl
 * @param in_buffer         Data to be assigned
 * @param in_buffer_size    Size of data
 * @param out_buffer        Data to be received
 * @param out_buffer_size   Size of out buffer
 * @param size_returned     Actual result size
 * @return s_ok if IOCTL performed.
 */
extern result_t comm_ioctl(handle_t device, ioctl_type type, const void *in_buffer,
  uint16_t in_buffer_size, void *out_buffer, uint16_t out_buffer_size,
  uint16_t *size_returned);

///////////////////////////////////////////////////////////////////////////////
//
//  Generic serial port device ioctls.

typedef enum
  {
  comms_state_ioctl,
  comms_error_ioctl,
  comms_escape_ioctl,
  comms_break_ioctl,
  comms_event_mask_ioctl,
  comms_modem_status_ioctl,
  comms_timeouts_ioctl,
  } comms_ioctl_type_t;

typedef enum
  {
  cbr_110 = 110,
  cbr_300 = 300,
  cbr_600 = 600,
  cbr_1200 = 1200,
  cbr_2400 = 2400,
  cbr_4800 = 4800,
  cbr_9600 = 9600,
  cbr_14400 = 14400,
  cbr_19200 = 19200,
  cbr_38400 = 38400,
  cbr_57600 = 57600,
  cbr_115200 = 115200,
  cbr_128000 = 128000,
  cbr_240000 = 240000,
  cbr_256000 = 256000
  } baud_rate_t;

typedef enum
  {
  no_parity,
  odd_parity,
  even_parity,
  mark_parity,
  space_parity
  } parity_mode_t;

typedef enum
  {
  dtr_disabled,
  dtr_enabled,
  dtr_flowcontrol
  } dtr_mode_t;

typedef enum
  {
  rts_control_disabled,
  rts_control_enabled,
  rts_control_handshake,
  rts_control_toggle
  } rts_control_t;

typedef enum
  {
  one_stop_bit = 0,
  one_five_stop_bit = 1,
  two_stop_bit = 2
  } stop_bits_t;

typedef struct _comms_ioctl_t
  {
  uint16_t version;
  comms_ioctl_type_t ioctl_type;
  } comms_ioctl_t;

typedef struct _comms_state_ioctl_t
  {
  comms_ioctl_t ioctl;
  baud_rate_t baud_rate;
  bool binary_mode;
  parity_mode_t parity;
  bool cts_out_enabled;
  bool dsr_out_enabled;
  bool dsr_in_enabled;
  dtr_mode_t dtr_mode;
  rts_control_t rts_control;
  bool xmit_continue_on_xoff;
  bool xoff_out_enabled;
  bool xoff_in_enabled;
  bool parity_character_enabled;
  bool ignore_null_characters;
  bool abort_on_error;
  bool parity_checking_enabled;
  int16_t xon_limit;
  int16_t xoff_limit;
  uint16_t byte_size;
  stop_bits_t stop_bits;
  char xon_char;
  char xoff_char;
  char error_character;
  char parity_character;
  char eof_char;
  char event_char;
  } comms_state_ioctl_t;

typedef struct _comms_error_ioctl_t
  {
  bool overflow;
  bool overrun;
  bool parity;
  bool framing_error;
  bool break_char;
  } comms_error_ioctl_t;

typedef enum
  {
  escape_set_xoff = 1,
  escape_set_xon = 2,
  escape_set_rts = 3,
  escape_clear_rts = 4,
  escape_set_dtr = 5,
  escape_clear_dtr = 6,
  escape_set_break = 8,
  escape_clear_break = 9
  } escape_character_type;

typedef struct _comms_escape_ioctl_t
  {
  comms_ioctl_t ioctl;
  escape_character_type escape_type;
  } comms_escape_ioctl_t;

typedef struct _comms_break_ioctl_t
  {
  comms_ioctl_t ioctl;
  bool set_break;
  } comms_break_ioctl_t;

typedef uint16_t event_mask_t;
// types of events that can be waited on

#define ev_break 0x0040
#define ev_cts 0x0008
#define ev_dsr 0x0010
#define ev_err 0x0080
#define ev_event1 0x0800
#define ev_event2 0x1000
#define ev_perr 0x0200
#define ev_ring 0x0100
#define ev_rlsd 0x0020
#define ev_rxfull 0x0400
#define ev_rxchar 0x0001
#define ev_rxflag 0x0002
#define ev_txempty 0x0004

typedef struct _comms_event_mask_ioctl_t
  {
  comms_ioctl_t ioctl;

  handle_t notification_event;
  event_mask_t mask;
  } comms_event_mask_ioctl_t;

typedef struct _comms_modem_status_ioctl_t
  {
  comms_ioctl_t ioctl;
  bool cts_on;
  bool dsr_on;
  bool ring_on;
  bool rlsd_on;
  } comms_modem_status_ioctl_t;

typedef struct _comms_timeouts_ioctl_t
  {
  comms_ioctl_t ioctl;
  /*
   The maximum time allowed to elapse between the arrival of two bytes on the communications line,
   in milliseconds. During a ReadFile operation, the time period begins when the first uint8_t is received.
   If the interval between the arrival of any two bytes exceeds this amount, the ReadFile operation is
   completed and any buffered data is returned. A value of zero indicates that interval time-outs are not used.

   A value of limit<unsigned int>::max(), combined with zero values for both the ReadTotalTimeoutConstant and
   ReadTotalTimeoutMultiplier members, specifies that the read operation is to return immediately with the bytes
   that have already been received, even if no bytes have been received.
   */
  unsigned int read_interval_timeout;
  /*
   The multiplier used to calculate the total time-out period for read operations, in milliseconds.
   For each read operation, this value is multiplied by the requested number of bytes to be read.
   */
  unsigned int read_total_timeout_multiplier;
  /*
   A constant used to calculate the total time-out period for read operations, in milliseconds.
   For each read operation, this value is added to the product of the ReadTotalTimeoutMultiplier member
   and the requested number of bytes.

   A value of zero for both the ReadTotalTimeoutMultiplier and ReadTotalTimeoutConstant members indicates
   that total time-outs are not used for read operations.
   */
  unsigned int read_total_timeout_constant;
  /*
   The multiplier used to calculate the total time-out period for write operations, in milliseconds.
   For each write operation, this value is multiplied by the number of bytes to be written.
   */
  unsigned int write_total_timeout_multiplier;
  /*
   A constant used to calculate the total time-out period for write operations, in milliseconds.
   For each write operation, this value is added to the product of the WriteTotalTimeoutMultiplier
   member and the number of bytes to be written.

   A value of zero for both the WriteTotalTimeoutMultiplier and WriteTotalTimeoutConstant members
   indicates that total time-outs are not used for write operations.
   */
  unsigned int write_total_timeout_constant;
  } comms_timeouts_ioctl_t;

///////////////////////////////////////////////////////////////////////////////
//
// Stream functions.
//
// The registry supports a stream object which can be treated as a bufferred
// stream.  The size of a stream is limited to
// 32 kBytes only
/**
 * @fn reg_stream_open(memid_t parent, const char *path, handle_t *stream)
 * Open a stream given a path.
 * @param parent    registry key the path is relative to, 0 for root
 * @param path      path to the stream
 * @param stream    opened stream
 * @return s_ok if the stream exists.
 */
extern result_t reg_stream_open(memid_t parent, const char *path, uint32_t flags, handle_t *stream);
/**
 * @fn reg_stream_create(memid_t parent, const char *path, handle_t *stream)
 * Create a new stream.
 * @param parent    registry key the path is relative to.  The key path must exist
 * @param path      path to the stream.
 * @param stream    newly created stream
 * @return s_ok if the stream created ok.
 */
extern result_t reg_stream_create(memid_t parent, const char *path, uint32_t flags, handle_t *stream);
/**
 * @fn manifest_open(const char *path, handle_t *stream)
 * Open a manifest stream.
 * @param key       Key to open resource from
 * @param path      Path to the stream. see remarks
 * @param stream    opened stream
 * @return s_ok if the stream was opened ok
 * @remark A manifest stream is a file that is stored as a base64
 * encoded file.  This allows for streams to be created using
 * the CLI to store resources (images and fonts) in the registry
 * or in code.  The resource is read-only
 */
extern result_t manifest_open(memid_t key, const char *path, handle_t *stream);
/**
 * @fn manifest_create(const char *path, handle_t *stream)
 * Open a manifest stream.
 * @param literal   Base64 encoded binary image
 * @param stream    opened stream
 * @return s_ok if the stream was opened ok
 * @remark A manifest stream is a file that is stored as a base64
 * encoded file.  This allows for streams to be created using
 * the CLI to store resources (images and fonts) in the registry
 * or in code.  The resource is read only
 */
extern result_t manifest_create(const char *literal, handle_t *stream);
#ifndef NAME_MAX
#define NAME_MAX	256
#endif
/**
* @fn stream_open(const char *path, handle_t *stream)
* Open a stream in the file system
* @param path path to the stream.  Is unix style "/dir/<file>
* @param flags open/create/truncate/append flags
* @param stream resulting stream
* @return s_ok if file is found
*/
extern result_t stream_open(const char *path, uint32_t flags,  handle_t *stream);
/**
* @fn stream_rename(handle_t stream, const char *new_filename)
* Rename a stream
* @param stream already opened stream
* @param new_filename the name of the new file
* @return s_ok if renamed ok
*/
extern result_t stream_rename(const char *old_filename, const char *new_filename);
/**
 * @brief Remove a stream
 * @param path Path to the stream
 * @return s_ok if removed
*/
extern result_t stream_delete(const char *path);
/**
* @fn create_directory(const char *path)
* Create a directory
* @param path path to the new directory
* @return s_ok if path created
*/
extern result_t create_directory(const char *path);
/**
* @fn remove_directory(const char *path)
* Remoce an empty directory
* @param path path to the empty directory
* @return s_ok if the path can be removed
*/
extern result_t remove_directory(const char *path);
/**
* @fn open_directory(const char *dirname, handle_t *dirp)
* Open a directory to enumerate the contents
* @param dirname path to the directory
* @param dirp handle to a directory enumeration
* @return s_ok if opened
*/
extern result_t open_directory(const char *dirname, handle_t *dirp);

typedef enum _dir_entry_type {
  et_file,
  et_directory
  } dir_entry_type;
/**
* @fn read_directory(handle_t dirp, dir_entry_type *et, char *buffer, size_t len)
* Read a directory entry and advance the directory pointer
* @param dirp opened directory pointer
* @param stat details of the entry
* @return s_ok if opened ok
*/
extern result_t read_directory(handle_t dirp, stat_t *stat);
/**
* @fn rewind_directory(handle_t dirp)
* Skip to the previous directory entry
* @param dirp directory entry pointer
* @return s_ok if rewound
*/
extern result_t rewind_directory(handle_t dirp);
/**
* @fn freespace(const char *path, off_t *space)
* return how much space is available on the device
* @param path path to a device
* @param space resulting free space
* @return s_ok if the path is valid
*/
extern result_t freespace(const char *path, uint32_t *space);
/**
* @fn totalspace(const char *path, off_t *space)
* return the total space available on a device
* @param path path to the mounted file system
* @param space resulting space available
* @return s_ok if a valid path
*/
extern result_t totalspace(const char *path, uint32_t *space);
/**
 * @brief Return information about a file/directory
 * @param path  path to query
 * @param st    result
 * @return s_ok if entry found
*/
extern result_t stream_stat(const char *path, stat_t *st);
/**
 * @fn stream_eof(handle_t stream)
 * Detect end of read pointer on stream
 * @param stream    stream to check
 * @return s_ok if at eof, s_false if not.
 */
extern result_t stream_eof(handle_t stream);
/**
 * @fn stream_read(handle_t stream, void *buffer, uint16_t size, uint16_t *read)
 * Read from a stream
 * @param stream    Stream to read from
 * @param buffer    Buffer to read bytes into
 * @param size      Size of read buffer
 * @param read      Number of bytes read into the buffer
 * @return s_ok if read ok.
 */
extern result_t stream_read(handle_t stream, void *buffer, uint32_t size, uint32_t *read);
/**
 * @fn stream_write(handle_t stream, const void *buffer, uint16_t size)
 * Write bytes to a stream
 * @param stream    Stream to write to
 * @param buffer    Buffer of bytes to write
 * @param size      Number of bytes to write
 * @return s_ok if the buffer was written to the stream
 */
extern result_t stream_write(handle_t stream, const void *buffer, uint32_t size);
/**
 * @fn stream_getpos(handle_t stream, uint16_t *pos)
 * Get the position of the stream for the next operation
 * @param stream    Stream to query
 * @param pos       Position of the stream for the next read or write
 * @return s_ok if a valid stream
 */
extern result_t stream_getpos(handle_t stream, uint32_t *pos);
/**
 * @fn stream_setpos(handle_t stream, uint16_t pos)
 * Move the stream offset
 * @param stream    Stream to query
 * @param pos       Position to set
 * @param whence    How to set position
 * @return s_ok if the position is valid.  This can at the end of the stream
 * or within the stream.  setting to UINT16_MAX will move the pointer to the
 * end of the stream.
 */
extern result_t stream_setpos(handle_t stream, int32_t pos, uint32_t whence);
/**
 * @fn stream_length(handle_t stream, uint16_t *length)
 * Return the number of bytes in a stream
 * @param stream    Stream to query
 * @param length    Length of the stream, max is 32768
 * @return s+ok if the handle is a valid stream
 */
extern result_t stream_length(handle_t stream, uint32_t *length);
/**
 * @fn stream_truncate(handle_t stream, uint16_t length)
 * Set the stream length
 * @param stream    Stream to set
 * @param length    Length to truncate to.  This must be at or less than the
 * stream length
 * @return s_ok if truncated ok
 */
extern result_t stream_truncate(handle_t stream, uint32_t length);
/**
 * @fn stream_copy(handle_t from, handle_t to)
 * Copy betrween two streams
 * @param from      Stream to copy from
 * @param to        Stream to copy to
 * @return s_ok if truncated ok
 */
extern result_t stream_copy(handle_t from, handle_t to);
/**
 * @fn strstream_create(const char *lit, handle_t *stream)
 * Create an in-memory stream.  Should be deleted when done.
 * @param buffer  Optional literal to copy into the stream
 * @param len     Length of the literal
 * @param read_only If set then the stream will be created read-only and the
 * buffer will not be copied into a dynamic memory
 * @param stream  Resulting stream
 * @return s_ok if created ok
 */
extern result_t strstream_create(const uint8_t *buffer, uint32_t len, bool read_only, handle_t *stream);
/**
 * @fn stream_vprintf(handle_t stream, const char *fmt, va_list ap)
 * print a formatted stream
 * @param stream
 * @param fmt
 * @param ap
 * @return
 */
extern result_t stream_vprintf(handle_t stream, const char *fmt, va_list ap);
/**
 * @fn handle_trintf(handle_t stream, const char *format, ...)
 * Print a formatted string.
 * @param stream    Stream to print to
 * @param format    Format to print as
 * @param cb        Callback to get an argument
 * @param argv      Pointer to arguments
 * @return s_ok if printed ok
 */
static inline result_t stream_printf(handle_t stream, const char *format, ...) // __attribute__((format(printf, 2, 3)));
  {
  va_list list;
  va_start(list, format);

  return stream_vprintf(stream, format, list);
  }
/**
 * @fn stream_vscanf(handle_t stream, const char *fmt, va_list ap)
 * Scan arguments from a stream
 * @param stream    Stream to scan from
 * @param fmt       Format of the arguments
 * @param ap        Varargs list
 * @return s_ok if scanned ok
 */
extern result_t stream_vscanf(handle_t stream, const char *fmt, va_list ap);
/**
 * @fn stream_scanf(handle_t stream, const char *format, ...)
 * Scan arguments from a stream.
 * @param stream    Stream to scan from
 * @param format    Format of the arguments
 * @return s_ok if scanned ok
 */
static inline result_t stream_scanf(handle_t stream, const char *format, ...) //  __attribute__((format(scanf, 2, 3)));
{
va_list list;
va_start(list, format);

return stream_vscanf(stream, format, list);
}
  /**
 * @fn stream_getc(handle_t stream, char *ch)
 * Read one character from the stream
 * @param stream    Stream to read from
 * @param ch        character read
 * @return s_ok if character available, or e_end_of_file if no characters
 */
extern result_t stream_getc(handle_t stream, char *ch);
/**
 * @fn handle_tutc(handle_t stream, char ch)
 * Send one character to the stream
 * @param stream    Stream to write to
 * @param ch        character to write
 * @return s_ok if written ok
 */
extern result_t stream_putc(handle_t stream, char ch);
/**
 * @fn stream_ungetc(handle_t stream, char ch
 * Push the character ch onto the end of the stream.
 * @param stream    Stream to write to
 * @param ch        character to append
 * @return s_ok if the stream position was rewound
 */
extern result_t stream_ungetc(handle_t stream, char ch);
/**
 * @fn stream_gets(handle_t stream, char *buffer, uint16_t len)
 * Read an ascii line of characters from the stream.
 * @param stream    Stream to read from
 * @param buffer    Buffer to read into
 * @param len       Length of read buffer
 * @return s_ok if read ok
 * This routine will wait for an EOL character or a buffer full, or EOF before
 * returning.  The end of line character is a \n character.  Depending on the
 * stream implementation this may be translated to a \r character
 * the bytes read will always be len-1 so the trailing \0 can be appended
 */
extern result_t stream_gets(handle_t stream, char *buffer, uint16_t len);
/**
 * @fn handle_tuts(handle_t stream, const char *str)
 * Write a string to a stream
 * @param stream    Stream to write to
 * @param str       string to send
 * @return s_ok if written ok.
 */
extern result_t stream_puts(handle_t stream, const char *str);

///////////////////////////////////////////////////////////////////////////////
//
// Compute a CRC
//
/**
* @fn compute_crc(const void *buffer, uint16_t length, uint32_t seed, uint32_t *value)
* Compute a CRC, uses hardware if possible
* @param buffer   Buffer to compute CRC for
* @param length   Number of bytes to iterate over
* @param seed     Seed CRC, set 0 if not used
* @param value    Resulting CRC
* @return s_ok if computed ok
*/
extern result_t compute_crc(const void *buffer, uint16_t length, uint32_t seed, uint32_t *value);
/**
 * @brief release all resources for a handle
 * @param hndl handle to release
 * @return s_ok if a valid handle and all resources released
*/
extern result_t close_handle(handle_t hndl);
///////////////////////////////////////////////////////////////////////////////
//
// low level message hooks
/**
 * @function bool (*msg_hook_fn)(const canmsg_t *msg, void *parg)
 * @param msg   Can message to process
 * @param parg  Argument used when hook function registered
 * @return true if the message was handled.  Only used when a service callback
 * is used.
 */
typedef bool (*msg_hook_fn)(const canmsg_t *, void *parg);
/**
 * @function subscribe(msg_hook_t *handler)
 * register a call-back function to handle messages.
 * @param handler Handler to add to the hook chain
 * @return s_ok if the handler can be added
 */
extern result_t subscribe(msg_hook_fn handler, void *parg, handle_t *hndl);
/**
 * @function unsubscribe(msg_hook_t *handler)
 * Remove the msg hook function
 * @param hndl Handler to remove from the hook chain
 * @return s_ok if the handler can be removed
 */
extern result_t unsubscribe(handle_t hndl);

////////////////////////////////////////////////////////////////////////////////
//
// Memory allocation routines.  Do not use malloc/free which are not thread safe
//
/**
 * @function neutron_malloc(size_t size)
 * Allocate a block of memory, thread-safe
 * @param size  size to allocate
 * @return pointer to memory, 0 if failed
 */
extern result_t neutron_malloc(size_t size, void **mem);
/**
 * @function neutron_calloc(size_t size,size_t elem_size, void** mem))
 * Allocate a block of memory and set its contents to all 0, thread-safe
 * @param size  number to allocate
 * @param elem_size size of each element
 * @return pointer to memory, 0 if failed
 */
extern result_t neutron_calloc(size_t size, size_t elem_size, void** mem);
/**
 * @function  neutron_free(void *mem)
 * Free a kernel allocated block of memory, thread-safe
 * @param mem memory to free
 */
extern result_t neutron_free(void *mem);
/**
 * @function neutron_realloc(void *mem, size_t new_size)
 * Resize the buffer to be new_size, thread-safe
 * @param mem       pointer to the existing buffer
 * @param new_size  new size of the buffer
 * @return pointer to the new buffer
 */
extern result_t neutron_realloc(size_t new_size, void **mem);
/**
 * @function neutron_strdup(const char *str)
 * Thread safe string duplicate
 * @param str string to copy
 * @param len duplicate at most len chars (0 = no limit)
 * @return the duplicated string, can be freed with neutron_free
 */
extern result_t neutron_strdup(const char *str, size_t len, char **mem);

///////////////////////////////////////////////////////////////////////////////
//
// Decompression of a stream.
//
// Functions to take a stream of bytes that is compressed using the DEFLATE
// algorithm.
//
/**
 * @function result_t (*get_byte_fn)(handle_t parg, uint32_t offset, uint8_t *data)
 * Get a byte from the decompressed buffer
 * @param parg    Opaque argument for the accessor
 * @param offset  Offset into the buffer
 * @param data    Data returned
 * @return s_ok if the offset is within bounds of the buffer and the data is available
 */
typedef result_t(*get_byte_fn)(handle_t parg, uint32_t offset, uint8_t *data);
/**
* @function result_t (*get_byte_fn)(handle_t parg, uint32_t offset, uint8_t *data)
* Get a byte from the decompressed buffer
* @param parg    Opaque argument for the accessor
* @param offset  Offset into the buffer
* @param data    Data to de assigned
* @return s_ok if the offset is within bounds of the buffer and the data was written
*/
typedef result_t(*set_byte_fn)(handle_t parg, uint32_t offset, uint8_t data);
/**
 * @function result_t decompress(handle_t stream, handle_t parg, get_byte_fn getter, set_byte_fn setter, uint32_t *length)
 * Decompress a stream into a user defined buffer
 * @param stream    Source stream to read from.  Must be a valid DEFLATE format stream
 * @param parg      User defined callback argument
 * @param getter    Function to read a byte from the user buffer
 * @param setter    Function to set a byte into the user buffer
 * @param length    Optional value that counts the number of bytes decompressed
 * @return s_ok if the stream was decoded ok.
 */
extern result_t decompress(handle_t stream, handle_t parg, get_byte_fn getter, set_byte_fn setter, uint32_t *length);

typedef struct _typeid_t typeid_t;

typedef result_t(*is_valid_fn)(const handle_t hndl);
/**
 * @brief optional function to call when the object is deleted
*/
typedef result_t(*etheralize_fn)(handle_t hndl);
/**
 * @brief create a stringified version of an object
 * @param obj    The handle to be stringified
 * @param fmt    Optional format
 * @param str    String, must be released with neutron_free
*/
typedef result_t(*to_string_fn)(handle_t hndl, const char *fmt, char *str);

typedef struct _typeid_t {
  const char *name;               // name of the type
  is_valid_fn is_valid;           // called to check an object is valid
  etheralize_fn etherealize;      // called to destroy an object
  to_string_fn to_string;         // called to convert an object to a string
  const typeid_t *base;           // base class (0 = no base)
  } typeid_t;

// base of all neutron types.
typedef struct _base_t {
  const typeid_t *type;
  } base_t;

/**
 * @brief Check for an coerce a handle to a type
 * @param handle          handle to be checked
 * @param type            type to coerce to
 * @param coerced_type    if != 0 pointer to the type
 * @return s_ok if coercion ok
*/
extern result_t is_typeof(const handle_t handle, const typeid_t *type, void **coerced_type);
/**
 * @brief Check all types given a pointer
 * @param handle handle to call check functions on
 * @return s_ok if a valid handle
*/
extern result_t check_handle(const handle_t handle);

#ifdef min
#undef min
#endif
#define min(a, b) ((a) < (b) ? (a) : b)


#ifdef max
#undef max
#endif
#define max(a, b) ((a) > (b) ? (a) : b)

extern void enter_critical();
extern void exit_critical();
/**
* @function rtc_hook()
* Update the real-time clock
* @remark if more frequent time updates are required, other than the gps position
* fix time update the framework should call this every second, or as often as the
* current_time should be updated.  The exact tick time will be used to increment the real-time clock
*/
extern void rtc_hook();
//extern result_t stream_printf(handle_t stream, char const *fmt, ...);

typedef enum _scan_type
  {
  s_int8,       // char
  s_uint8,      // unsigned char
  s_int16,      // int
  s_uint16,     // unsigned int
  s_int32,      // long
  s_uint32,     // unsigned long
  s_str,        // char *
  s_float,      // float, float
  } scan_type;
/**
 * @function get_arg_fn(uint16_t arg, void *argv, scan_type dt, void *value)
 * Callback function for scanf/printf
 * @param arg     Argument number 0..n
 * @param argv    Callback parameter
 * @param dt      Datatype scanned or requested
 * @param value   Pointer to pointer to the value passed-on or returned
 * @return s_ok if a valid param
 */
typedef result_t(*get_arg_fn)(uint16_t arg, void *argv, scan_type dt, void *value);
/**
 * @function handle_printf_cb(handle_t stream, const char *format, get_arg_fn cb, void *argv)
 * Print a formatted string.
 * @param stream    Stream to print to
 * @param format    Format to print as
 * @param cb        Callback to get an argument
 * @param argv      Pointer to arguments
 * @return s_ok if printed ok
 */
extern result_t stream_printf_cb(handle_t stream, const char *format, get_arg_fn cb, void *argv);
/**
 * @function stream_scanf_cb(handle_t stream, const char *format, get_arg_fn cb, void *argv)
 * Scan arguments from a string.
 * @param stream    Stream to scan from
 * @param format    Format of the arguments
 * @param cb        Callback to get an argument
 * @param argv      Pointer to arguments
 * @return s_ok if scanned ok
 */
extern result_t stream_scanf_cb(handle_t stream, const char *format, get_arg_fn cb, void *argv);

////////////////////////////////////////////////////////////////////////////////
//
// Kernel start routines
/**
 * @function neutron_run(const char *name, uint16_t stack_size, task_callback callback, void *parg, uint8_t priority, handle_t *task)
 * Initialize memory, create a root task and dispatch it.
 * Never returns
 * @param name          name of the task
 * @param stack_size    size of stack to allocate (words)
 * @param callback      task main routine
 * @param parg          argument to pass to task
 * @param priority      initial priority
 * @param task          created task handle
 */
extern void neutron_run(const char *name,
  uint16_t stack_size,
  task_callback callback,
  void *parg,
  uint8_t priority,
  handle_t *task);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// CanFly/Neutron startup
///
/// Only call this is there is a GDI required and a framebuffer is available. 
/// The device needs to be linked with the photon library.
/// 
/// If a c# runtime is required then ion and nano will be required

/**
 * Open the registry functions.  Must be called from the low level code
 * @param factory_reset  true if reset device back to an empty eeprom
 * @param size	Number of memid's
 * @param row_size	Maximum size of row that can be read/written
 * @param task_callback	Syncronisation semaphore to use if needed
 * @return s_ok if opened ok
 */
extern result_t bsp_reg_init(bool factory_reset, uint16_t size, uint16_t row_size);
/**
 * Read blocks from the registry memory
 * @param memid			Starting memid
 * @param bytes_to_read	Total bytes to read.  Note this will always be multiples of the row size
 * @param buffer		Data buffer to read into
 * @param task_callback	Syncronisation semaphore to use if needed
 * @return s_ok if opened ok
 */
extern result_t bsp_reg_read_block(uint32_t memid, uint16_t bytes_to_read, void *buffer);
/**
 * Read blocks to the registry memory
 * @param memid			Starting memid
 * @param bytes_to_write	Total bytes to write.  Note this will always be multiples of the row size
 * @param buffer		Data buffer to write from
 * @param task_callback	Syncronisation semaphore to use if needed
 * @return s_ok if opened ok
 */
extern result_t bsp_reg_write_block(uint32_t memid, uint16_t bytes_to_write, const void *buffer);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// CanFly/Neutron startup
typedef union _can_filter_t {
  uint16_t id : 10;
  uint16_t mask : 2;
  uint16_t enabled : 1;
  } can_filter_t;
  
#define FILTER_256 3
#define FILTER_64  2
#define FILTER_32  1
#define FILTER_16  0

// callback to set the node id  
typedef void (*set_node_id_fn)(uint8_t new_id, uint8_t new_subtype);

typedef struct _neutron_parameters_t
  {
  // name of the node reported in the nis message
  const char *node_name;
  // node identifier.  Published in id msg
  // NOTE: this is used to set the masks for the service channel
  // and bearer channel filters
  uint8_t node_id;
  // optional node_setter
  set_node_id_fn set_id;
  uint8_t hardware_id;
  uint8_t hardware_version;
  // size of can transmit buffer, if 0 default size used
  uint16_t tx_length;
  // size of can tx worker task stack, if 0 default size used
  uint16_t tx_stack_length;
  // size of can receive buffer, if 0 default size used
  uint16_t rx_length;
  // size of can rx worker task stack, if 0 default size used
  uint16_t rx_stack_length;
  // rate to operate the can bus as
  uint32_t bitrate;
  // clock supplied to the can controller
  uint32_t fsys;
  // used to generate can packets.  Most low level devices use this format
  uint32_t sjw;
  uint32_t prseg;
  uint32_t seg1ph;
  uint32_t seg2ph;
  
  // Filters to apply to the CAN bus to speed up
  // message filters
  can_filter_t can_filters[30];   // ranges of filters
  
  //
  // Example device provisioning code
  // NTQ = sjw + prseg + seg1ph + seg2ph
  //
  // for 50mhz clock:
  //
  //  sjw = 1
  //  prseg = 1
  //  seg1ph = 5
  //  seg2ph = 3
  //
  // ntq = 10
  // bitrate = 250 kbps
  // fsys = 50 mHz
  //
  // divisor = (fsys / (2* ntq * bitrate))-1;
  // divisor = (50000 /(2 * 10 * 250))-1;
  // divisor = 9
  
  // this is the size of the config db in bytes
  uint32_t config_db_size;
  uint16_t eeprom_channel;        // which i2c port to use
  bool init_config_db;
  uint32_t status_interval;     // status interval check
  } neutron_parameters_t;

/**
 * @brief cached startup parameters
*/
extern const neutron_parameters_t *neutron_params;

/**
 * Initialize the can-aerospace subsystem, then initialize neutron
 * @param params
 * @param create_publish_task
 * @return
 */
extern result_t canfly_init(const neutron_parameters_t *params, bool create_publish_task);

/**
 * Initialize Neutron
 * @param params      setup and memory parameters
 * @param init_mode   true if a factory reset
 * @return s_ok if started ok
 */
extern result_t neutron_init(const neutron_parameters_t *params, bool create_worker);
/**
 * @brief update the status message send
 * @param status  Status to send
 * @param force_update True if send update immediately
 * @return s_ok if message updated ok
 */
extern result_t update_board_status(e_board_status status, bool force_update);

/////////////////////////////////////////////////////////////////////////////
//
// These are calls to the implementation of the hardware
/**
 * Initialize the hardware
 * @param rx_queue  Receive message queue.  The hardware needs to make an ISR safe call to push data onto queue
 */
extern result_t bsp_can_init(handle_t rx_queue, const neutron_parameters_t *params);
/**
 * Send a message
 * @param msg
 */
extern result_t bsp_send_can(const canmsg_t *msg);

extern const typeid_t filesystem_type;

// this defines a plugable file system for neutron
// the bsp has the api to register a file system
typedef struct _filesystem_t {
  base_t base;
  result_t(*mount)(filesystem_t *fs);
  result_t(*unmount)(filesystem_t *fs);
  result_t(*fssync)(filesystem_t *fs);
  result_t(*open)(filesystem_t *fs, const char *path, uint32_t flags, handle_t *stream);
  result_t(*remove)(filesystem_t *fs, const char *path);
  result_t(*rename)(filesystem_t *fs, const char *old_path, const char *new_path);
  result_t(*stat)(filesystem_t *fs, const char *path, stat_t *st);
  result_t(*mkdir)(filesystem_t *fs, const char *path);
  result_t(*rmdir)(filesystem_t *fs, const char *path);
  result_t(*opendir)(filesystem_t *fs, const char *dirname, handle_t *dirp);
  result_t(*readdir)(filesystem_t *fs, handle_t dirp, stat_t* info);
  result_t(*seekdir)(filesystem_t *fs, handle_t dirp, uint32_t offs);
  result_t(*telldir)(filesystem_t *fs, handle_t dirp, uint32_t *offs);
  result_t(*rewinddir)(filesystem_t *fs, handle_t dirp);
  result_t(*sync)(filesystem_t *fs, handle_t fshndl);
  result_t(*freespace)(filesystem_t *fs, const char *path, uint32_t *space);
  result_t(*totalspace)(filesystem_t *fs, const char *path, uint32_t *space);
  } filesystem_t;

/**
  * @function  mount(const char *mount_point, const filesystem_t *file_type, handle_t *fshndl)
  * Mount a flash file system
  * @param drive_letter
  * @param filesystem  type of filesystem
  * @return s_ok if file system mounted
  * @remark These form the prefix for all paths (A:, B: C: etc)
  */
result_t mount(char drive_letter, filesystem_t *filesystem);

/**
 * @function umount(handle_t fshndl)
 * Unmount a file system.  This is very sim
 * @param fshndl
 * @return
 */
extern result_t umount(handle_t fshndl);
/**
* @function fs_sync(handle_t fshndl)
* Force a sync of any data in the filesystem
* @param fshndl hanldle to filesystem, 0 will sync all loaded filesystems
* @return s_ok if all data flushed ok
*/
extern result_t fs_sync(handle_t fshndl);
/**
 * @brief Create a filesystem that uses the registry as a backing store
 * @param root    memid that is the root
 * @param handle  created filesystem
 * @return s_ok if created
*/
extern result_t create_registry_fs(memid_t root, filesystem_t** handle);

/////////////////////////////////////////////////////////////////////////////
//
// User mode application functions.
//
// this is a special file stystem that will be mounted at /app and allows
// for access to the program flash arena for storing applications

typedef struct _flash_disk_t flash_disk_t;

/**
* @brief Erase a block ready for an image to be written
* @param fs      file system holding flash params
* @param base    base address of image
* @param length  length of image
* @return s_ok if the memory is erased.
*/
typedef result_t(*flash_erase_fn)(const flash_disk_t *fs, uint32_t block);
typedef result_t(*flash_write_fn)(const flash_disk_t *fs, uint32_t block, uint32_t page, uint32_t size, const void *buffer);
typedef result_t(*flash_read_fn)(const flash_disk_t *fs, uint32_t block, uint32_t page, uint32_t size, void *buffer);
typedef result_t(*flash_write_oob_fn)(const flash_disk_t *fs, uint32_t block, uint32_t page, uint32_t size, const void *buffer);
typedef result_t(*flash_read_oob_fn)(const flash_disk_t* fs, uint32_t block, uint32_t page, uint32_t size, void* buffer);
typedef result_t(*flash_sync_fn)(const flash_disk_t *fs);
/**
 * @brief copy a page and optionally update data on the page
 * @param fs   The filesystem
 * @param src_page  Page to update
 * @paran dest_page Page to write
 * @param offset    offset to update data with
 * @param size      length of data (if 0 then no update done)
 * @param buffer    If data modification happening, then this is the data
*/
typedef result_t(*flash_copy_page_fn)(const flash_disk_t *fs, uint32_t src_block, uint32_t src_page,
 uint32_t dst_block, uint32_t dest_page, uint32_t offset, uint16_t size, const void *buffer);

typedef struct _flash_params_t {

  uint32_t page_size;       // minimum page size that can be written
  // Size of an erasable block in bytes. This does not impact ram consumption
  // and may be larger than the physical erase size. However, non-inlined
  // files take up at minimum one block. Must be a multiple of the read and
  // program sizes.
  uint32_t block_size;
  // Number of erasable blocks on the device.
  uint32_t block_count;
  // Number of erase cycles before littlefs evicts metadata logs and moves
  // the metadata to another block. Suggested values are in the
  // range 100-1000, with large values having better performance at the cost
  // of less consistent wear distribution.
  //
  // Set to -1 to disable block-level wear-leveling.
  int32_t block_cycles;
  // Optional upper limit on length of file names in bytes. No downside for
  // larger names except the size of the info struct which is controlled by
  // the LFS_NAME_MAX define. Defaults to LFS_NAME_MAX when zero. Stored in
  // superblock and must be respected by other drivers.
  uint32_t name_max;
  // Optional upper limit on files in bytes. No downside for larger files
  // but must be <= LFS_FILE_MAX. Defaults to LFS_FILE_MAX when zero. Stored
  // in superblock and must be respected by other drivers.
  uint32_t file_max;
  // Optional upper limit on custom attributes in bytes. No downside for
  // larger attributes size but must be <= LFS_ATTR_MAX. Defaults to
  // LFS_ATTR_MAX when zero.
  uint32_t attr_max;
  // Optional upper limit on total space given to metadata pairs in bytes. On
  // devices with large blocks (e.g. 128kB) setting this to a low size (2-8kB)
  // can help bound the metadata compaction time. Must be <= block_size.
  // Defaults to block_size when zero.
  uint32_t metadata_max;
  uint32_t page_size;       // minimum page size that can be written
  // Size of an erasable block in bytes. This does not impact ram consumption
  // and may be larger than the physical erase size. However, non-inlined
  // files take up at minimum one block. Must be a multiple of the read and
  // program sizes.
  uint32_t block_size;
  // Number of erasable blocks on the device.
  uint32_t block_count;

  uint32_t oob_size;        // size of oob data per page

  // Minimum size of a block read in bytes. All read operations will be a
  // multiple of this value.
  uint32_t read_size;
  // Minimum size of a block program in bytes. All program operations will be
  // a multiple of this value.
  uint32_t prog_size;

  flash_erase_fn erase;
  flash_write_fn write;
  flash_read_fn read;
  flash_write_oob_fn write_oob;
  flash_read_oob_fn read_oob;
  flash_copy_page_fn copy_page;
  flash_sync_fn sync;
  } flash_disk_t;


/**
 * @brief Open an existing flash file system
 * @param disk  Disk to use for the file system
 * @param cache_size  Size of block caches in bytes. Each cache buffers a portion of a block in
 * RAM. The flash filesystem needs a read cache, a program cache, and one additional
 * cache per file. Larger caches can improve performance by storing more
 * data and reducing the number of disk accesses. Must be a multiple of the
 * read and program sizes, and a factor of the block size.
 * @param lookahead_size  Size of the lookahead buffer in bytes. A larger lookahead buffer
 * increases the number of blocks found during an allocation pass. The
 * lookahead buffer is stored as a compact bitmap, so each byte of RAM
 * can track 8 blocks. Must be a multiple of 8.
 * @param fs    Handle to the filesystem
 * @return s_ok if opened ok
*/
extern result_t open_flash_filesystem(const flash_disk_t *disk,
                                      uint32_t cache_size,
                                      uint32_t lookahead_size,
                                      filesystem_t **fs);
/**
 * @brief Initialize a new filesystem on a disk
 * @param disk      Disk to allocate on
 * @param cache_size  Size of block caches in bytes. Each cache buffers a portion of a block in
 * RAM. The flash filesystem needs a read cache, a program cache, and one additional
 * cache per file. Larger caches can improve performance by storing more
 * data and reducing the number of disk accesses. Must be a multiple of the
 * read and program sizes, and a factor of the block size.
 * @param lookahead_size  Size of the lookahead buffer in bytes. A larger lookahead buffer
 * increases the number of blocks found during an allocation pass. The
 * lookahead buffer is stored as a compact bitmap, so each byte of RAM
 * can track 8 blocks. Must be a multiple of 8.
 * @param params    Superblock params
 * @param wipe_fs   If set all blocks are erased
 * @param fs        New filesystem
 * @return 
*/
extern result_t create_flash_filesystem(const flash_disk_t *disk,
                                        uint32_t cache_size,
                                        uint32_t lookahead_size,
                                        bool wipe_fs,
                                        filesystem_t **fs);
/**
 * @brief create an image update file
 * @param name    Name of the image
 * @param size    Size of the image
 * @param fd      Generated file
 * @return s_ok if image created ok
 */
extern result_t create_image(const char *name, uint32_t size, handle_t *fd);
/**
 * @brief start the neutron services running
 * @return 
 */
extern result_t neutron__services_init(const neutron_parameters_t *params);

/////////////////////////////////////////////////////////////////////////////
//
// Random number and system information
//
/**
* @function bsp_random(uint32_t seed, uint32_t *rand)
* @param seed seed to use
* @param rand random number generated
* @returns_ok if the random number generated
*/
extern result_t bsp_random(uint32_t seed, uint32_t *rand);
/**
* @param id         unique id
* @param node_id    0..31 node id of this node
* @param build      Release number of the OS.
* @return s_ok if a unique cpu id is available.
*/
extern result_t bsp_cpu_id(uint32_t *id, uint32_t *build, const char **image_name);
/**
 * @brief Queue a request to have the device reset itself
 * @paran msec_delay  number of milliseconds to delay before the reset occurs
 * @return s_ok if reset supported
*/
extern result_t bsp_queue_reset(uint32_t msec_delay);

//////////////////////////////////////////////////////////////////////
// this structure describes an enumeration
// MUST be sorted in name order or the completions won't work
// the values can be any order
typedef struct _enum_t {
  const char *name;
  uint16_t value;
  } enum_t;

//////////////////////////////////////////////////////////////////////
//
//    Diagnostics and tracing

#ifdef __assert
#undef __assert
#endif

extern result_t debug_break();

#if defined(_DEBUG) && defined(_WIN32)
extern result_t __assert(const char *file, int line, bool check, const char *msg);
#endif

#ifdef assert
#undef assert
#endif

#if defined(_DEBUG) && defined(_WIN32)
#define assert(chk) __assert(__FILE__, __LINE__, chk, 0)
#define assert_s(chk, s) __assert(__FILE__, __LINE__, chk, s)
#else
#define assert(chk)
#endif

#ifdef trace
#undef trace
#endif

// platform trace
extern result_t platform_trace(uint16_t level, const char *msg, va_list va);

// trace writes messages to a stream in the registry that is limited to 4kbytes.
// used for startup logging
//
// can be accessed using shell
//    cat dmesg
// similar to the linux varsion
static inline void trace(uint16_t level, const char *msg, ...)
  {
  va_list list;
  va_start(list, msg);

  platform_trace(level, msg, list);
  }

#ifndef TRACE_LEVEL
#ifdef _WIN32
#define TRACE_LEVEL 7
#else
#define TRACE_LEVEL 3
#endif
#endif

#define trace_emergency(...)  trace(0, __VA_ARGS__)

#if TRACE_LEVEL > 0
#define trace_alert(...)      trace(1, __VA_ARGS__)
#else
#define trace_alert(...)
#endif

#if TRACE_LEVEL > 1
#define trace_critical(...)   trace(2, __VA_ARGS__)
#else
#define trace_critical(...)
#endif

#if TRACE_LEVEL > 2
#define trace_error(...)      trace(3, __VA_ARGS__)
#else
#define trace_error(...)
#endif

#if TRACE_LEVEL > 3
#define trace_warning(...)    trace(4, __VA_ARGS__)
#else
#define trace_warning(...)
#endif

#if TRACE_LEVEL > 4
#define trace_notice(...)     trace(5, __VA_ARGS__)
#else
#define trace_notice(...)
#endif

#if TRACE_LEVEL > 5
#define trace_info(...)       trace(6, __VA_ARGS__)
#else
#define trace_info(...)
#endif

#if TRACE_LEVEL > 6
#define trace_debug(...)      trace(7, __VA_ARGS__)
#else
#define trace_debug(...)
#endif

#ifdef _WIN32
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif


#endif


