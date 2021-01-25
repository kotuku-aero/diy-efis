//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_TYPES_H_
#define _NANOCLR_TYPES_H_

#include "nanoCLR_PlatformDef.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

#define FAULT_ON_NULL_HR(ptr, hr)                                                                                      \
    if (!(ptr))                                                                                                        \
    NANOCLR_SET_AND_LEAVE((hr))
#define CHECK_ALLOCATION(ptr)                                                                                          \
    if (!(ptr))                                                                                                        \
    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY)
#define CHECK_SIZE_RANGE(len)                                                                                          \
    if (len > CLR_RT_HeapBlock::HB_MaxSize)                                                                            \
    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE)

#define FIMPLIES(x, y)        ((!(x)) || (y))
#define FINRANGE(val, lo, hi) (((val) >= (lo)) && ((val) <= (hi)))

///////////////////////////////////////////////////////////////////////////////////////////////////

#define NANOCLR_INTEROP_STUB_RETURN(stack) return stack.NotImplementedStub()
#define NANOCLR_SYSTEM_STUB_RETURN()       return S_OK
#define NANOCLR_FEATURE_STUB_RETURN()      return CLR_E_NOTIMPL

//////////////////////////////////////////////////

#define NANOCLR_PARAMCHECK_BEGIN()                                                                                     \
    {                                                                                                                  \
        HRESULT hrInner = S_OK;

#define NANOCLR_PARAMCHECK_POINTER(ptr)                                                                                \
    {                                                                                                                  \
        if (ptr == NULL)                                                                                               \
        {                                                                                                              \
            hrInner = CLR_E_NULL_REFERENCE;                                                                            \
        }                                                                                                              \
    }

#define NANOCLR_PARAMCHECK_POINTER_AND_SET(ptr, val)                                                                   \
    {                                                                                                                  \
        if (ptr == NULL)                                                                                               \
        {                                                                                                              \
            hrInner = CLR_E_NULL_REFERENCE;                                                                            \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            *ptr = val;                                                                                                \
        }                                                                                                              \
    }

#define NANOCLR_PARAMCHECK_NOTNULL(ptr)                                                                                \
    {                                                                                                                  \
        if (ptr == NULL)                                                                                               \
        {                                                                                                              \
            hrInner = CLR_E_INVALID_PARAMETER;                                                                         \
        }                                                                                                              \
    }

#define NANOCLR_PARAMCHECK_STRING_NOT_EMPTY(ptr)                                                                       \
    {                                                                                                                  \
        if (ptr == NULL || ptr[0] == 0)                                                                                \
        {                                                                                                              \
            hrInner = CLR_E_INVALID_PARAMETER;                                                                         \
        }                                                                                                              \
    }

#define NANOCLR_PARAMCHECK_END()                                                                                       \
    {                                                                                                                  \
        NANOCLR_CHECK_HRESULT(hrInner);                                                                                \
    }                                                                                                                  \
    }

///////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma pack(push, NANOCLR_TYPES_H, 4)
#endif

enum CLR_OPCODE
  {
#define OPDEF(c, s, pop, push, args, type, l, s1, s2, ctrl) c,
#include "opcode.def"
#undef OPDEF
  CEE_COUNT, /* number of instructions and macros pre-defined */
  };

#if defined(__GNUC__)
#define __int64 long long
#endif

typedef const uint8_t* CLR_PMETADATA;

//--//
// may need to change later
typedef int64_t CLR_INT64_TEMP_CAST;
typedef uint64_t CLR_UINT64_TEMP_CAST;
typedef double CLR_DOUBLE_TEMP_CAST;

#define CLR_SIG_INVALID 0xFFFF

enum CLR_LOGICAL_OPCODE
  {
  LO_Not = 0x00,
  LO_And = 0x01,
  LO_Or = 0x02,
  LO_Xor = 0x03,
  LO_Shl = 0x04,
  LO_Shr = 0x05,

  LO_Neg = 0x06,
  LO_Add = 0x07,
  LO_Sub = 0x08,
  LO_Mul = 0x09,
  LO_Div = 0x0A,
  LO_Rem = 0x0B,

  LO_Box = 0x0C,
  LO_Unbox = 0x0D,

  LO_Branch = 0x0E,
  LO_Set = 0x0F,
  LO_Switch = 0x10,

  LO_LoadFunction = 0x11,
  LO_LoadVirtFunction = 0x12,

  LO_Call = 0x13,
  LO_CallVirt = 0x14,
  LO_Ret = 0x15,

  LO_NewObject = 0x16,
  LO_CastClass = 0x17,
  LO_IsInst = 0x18,

  LO_Dup = 0x19,
  LO_Pop = 0x1A,

  LO_Throw = 0x1B,
  LO_Rethrow = 0x1C,
  LO_Leave = 0x1D,
  LO_EndFinally = 0x1E,

  LO_Convert = 0x1F,

  LO_StoreArgument = 0x20,
  LO_LoadArgument = 0x21,
  LO_LoadArgumentAddress = 0x22,

  LO_StoreLocal = 0x23,
  LO_LoadLocal = 0x24,
  LO_LoadLocalAddress = 0x25,

  LO_LoadConstant_I4 = 0x26,
  LO_LoadConstant_I8 = 0x27,
  LO_LoadConstant_R4 = 0x28,
  LO_LoadConstant_R8 = 0x29,

  LO_LoadNull = 0x2A,
  LO_LoadString = 0x2B,
  LO_LoadToken = 0x2C,

  LO_NewArray = 0x2D,
  LO_LoadLength = 0x2E,

  LO_StoreElement = 0x2F,
  LO_LoadElement = 0x30,
  LO_LoadElementAddress = 0x31,

  LO_StoreField = 0x32,
  LO_LoadField = 0x33,
  LO_LoadFieldAddress = 0x34,

  LO_StoreStaticField = 0x35,
  LO_LoadStaticField = 0x36,
  LO_LoadStaticFieldAddress = 0x37,

  LO_StoreIndirect = 0x38,
  LO_LoadIndirect = 0x39,

  LO_InitObject = 0x3A,
  LO_LoadObject = 0x3B,
  LO_CopyObject = 0x3C,
  LO_StoreObject = 0x3D,

  LO_Nop = 0x3E,

  LO_EndFilter = 0x3F,

  LO_Unsupported = 0x40,

  LO_FIRST = LO_Not,
  LO_LAST = LO_EndFilter,
  };

///////////////////////////////////////////////////////////////////////////////////////////////////

static const uint16_t CLR_EmptyIndex = 0xFFFF;
static const uint32_t CLR_EmptyToken = 0xFFFFFFFF;
static const size_t CLR_MaxStreamSize_AssemblyRef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_TypeRef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_FieldRef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_MethodRef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_TypeDef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_FieldDef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_MethodDef = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_Attributes = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_TypeSpec = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_Resources = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_ResourcesData = 0xFFFFFFFF;
static const size_t CLR_MaxStreamSize_Strings = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_Signatures = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_ByteCode = 0x0000FFFF;
static const size_t CLR_MaxStreamSize_ResourcesFiles = 0x0000FFFF;

enum CLR_OpcodeParam
  {
  CLR_OpcodeParam_Field = 0,
  CLR_OpcodeParam_Method = 1,
  CLR_OpcodeParam_Type = 2,
  CLR_OpcodeParam_String = 3,
  CLR_OpcodeParam_Tok = 4,
  CLR_OpcodeParam_Sig = 5,
  CLR_OpcodeParam_BrTarget = 6,
  CLR_OpcodeParam_ShortBrTarget = 7,
  CLR_OpcodeParam_I = 8,
  CLR_OpcodeParam_I8 = 9,
  CLR_OpcodeParam_None = 10,
  CLR_OpcodeParam_R = 11,
  CLR_OpcodeParam_Switch = 12,
  CLR_OpcodeParam_Var = 13,
  CLR_OpcodeParam_ShortI = 14,
  CLR_OpcodeParam_ShortR = 15,
  CLR_OpcodeParam_ShortVar = 16,
  };

#define CanCompressOpParamToken(opParam) (opParam >= CLR_OpcodeParam_Field && opParam <= CLR_OpcodeParam_String)
#define IsOpParamToken(opParam)          (opParam >= CLR_OpcodeParam_Field && opParam <= CLR_OpcodeParam_Sig)

//--//

enum CLR_FlowControl
  {
  CLR_FlowControl_NEXT = 0,
  CLR_FlowControl_CALL = 1,
  CLR_FlowControl_RETURN = 2,
  CLR_FlowControl_BRANCH = 3,
  CLR_FlowControl_COND_BRANCH = 4,
  CLR_FlowControl_THROW = 5,
  CLR_FlowControl_BREAK = 6,
  CLR_FlowControl_META = 7,
  };

//--//

#define c_CLR_StringTable_Version 1

///////////////////////////////////////////////////////////////////////////////////////////////////

enum CLR_TABLESENUM
  {
  TBL_AssemblyRef = 0x00000000,
  TBL_TypeRef = 0x00000001,
  TBL_FieldRef = 0x00000002,
  TBL_MethodRef = 0x00000003,
  TBL_TypeDef = 0x00000004,
  TBL_FieldDef = 0x00000005,
  TBL_MethodDef = 0x00000006,
  TBL_Attributes = 0x00000007,
  TBL_TypeSpec = 0x00000008,
  TBL_Resources = 0x00000009,
  TBL_ResourcesData = 0x0000000A,
  TBL_Strings = 0x0000000B,
  TBL_Signatures = 0x0000000C,
  TBL_ByteCode = 0x0000000D,
  TBL_ResourcesFiles = 0x0000000E,
  TBL_EndOfAssembly = 0x0000000F,
  TBL_Max = 0x00000010,
  };

enum CLR_CorCallingConvention
  {
  /////////////////////////////////////////////////////////////////////////////////////////////
  //
  // This is based on CorCallingConvention.
  //
  PIMAGE_CEE_CS_CALLCONV_DEFAULT = 0x0,

  PIMAGE_CEE_CS_CALLCONV_VARARG = 0x5,
  PIMAGE_CEE_CS_CALLCONV_FIELD = 0x6,
  PIMAGE_CEE_CS_CALLCONV_LOCAL_SIG = 0x7,
  PIMAGE_CEE_CS_CALLCONV_PROPERTY = 0x8,
  PIMAGE_CEE_CS_CALLCONV_UNMGD = 0x9,
  PIMAGE_CEE_CS_CALLCONV_GENERICINST = 0xa,  // generic method instantiation
  PIMAGE_CEE_CS_CALLCONV_NATIVEVARARG = 0xb, // used ONLY for 64bit vararg PInvoke calls
  PIMAGE_CEE_CS_CALLCONV_MAX = 0xc,          // first invalid calling convention

  // The high bits of the calling convention convey additional info
  PIMAGE_CEE_CS_CALLCONV_MASK = 0x0f,         // Calling convention is bottom 4 bits
  PIMAGE_CEE_CS_CALLCONV_HASTHIS = 0x20,      // Top bit indicates a 'this' parameter
  PIMAGE_CEE_CS_CALLCONV_EXPLICITTHIS = 0x40, // This parameter is explicitly in the signature
  PIMAGE_CEE_CS_CALLCONV_GENERIC =
  0x10, // Generic method sig with explicit number of type arguments (precedes ordinary parameter count)
        //
        // End of overlap with CorCallingConvention.
        //
        /////////////////////////////////////////////////////////////////////////////////////////////
  };

enum CLR_DataType // KEEP IN SYNC WITH Microsoft.SPOT.DataType!!
  {
  DATATYPE_VOID, // 0 bytes

  DATATYPE_BOOLEAN, // 1 byte
  DATATYPE_I1,      // 1 byte
  DATATYPE_U1,      // 1 byte

  DATATYPE_CHAR, // 2 bytes
  DATATYPE_I2,   // 2 bytes
  DATATYPE_U2,   // 2 bytes

  DATATYPE_I4, // 4 bytes
  DATATYPE_U4, // 4 bytes
  DATATYPE_R4, // 4 bytes

  DATATYPE_I8,       // 8 bytes
  DATATYPE_U8,       // 8 bytes
  DATATYPE_R8,       // 8 bytes
  DATATYPE_DATETIME, // 8 bytes     // Shortcut for System.DateTime
  DATATYPE_TIMESPAN, // 8 bytes     // Shortcut for System.TimeSpan
  DATATYPE_STRING,

  DATATYPE_LAST_NONPOINTER = DATATYPE_TIMESPAN,      // This is the last type that doesn't need to be relocated.
  DATATYPE_LAST_PRIMITIVE_TO_PRESERVE = DATATYPE_R8, // All the above types don't need fix-up on assignment.
  DATATYPE_LAST_PRIMITIVE_TO_MARSHAL = DATATYPE_STRING, // All the above types can be marshaled by assignment.
  DATATYPE_LAST_PRIMITIVE = DATATYPE_STRING, // All the above types don't need fix-up on assignment.

  DATATYPE_OBJECT,    // Shortcut for System.Object
  DATATYPE_CLASS,     // CLASS <class Token>
  DATATYPE_VALUETYPE, // VALUETYPE <class Token>
  DATATYPE_SZARRAY,   // Shortcut for single dimension zero lower bound array SZARRAY <type>
  DATATYPE_BYREF,     // BYREF <type>

  ////////////////////////////////////////

  DATATYPE_FREEBLOCK,
  DATATYPE_CACHEDBLOCK,
  DATATYPE_ASSEMBLY,
  DATATYPE_WEAKCLASS,
  DATATYPE_REFLECTION,
  DATATYPE_ARRAY_BYREF,
  DATATYPE_DELEGATE_HEAD,
  DATATYPE_DELEGATELIST_HEAD,
  DATATYPE_OBJECT_TO_EVENT,
  DATATYPE_BINARY_BLOB_HEAD,

  DATATYPE_THREAD,
  DATATYPE_SUBTHREAD,
  DATATYPE_STACK_FRAME,
  DATATYPE_TIMER_HEAD,
  DATATYPE_LOCK_HEAD,
  DATATYPE_LOCK_OWNER_HEAD,
  DATATYPE_LOCK_REQUEST_HEAD,
  DATATYPE_WAIT_FOR_OBJECT_HEAD,
  DATATYPE_FINALIZER_HEAD,
  DATATYPE_MEMORY_STREAM_HEAD, // SubDataType?
  DATATYPE_MEMORY_STREAM_DATA, // SubDataType?

  DATATYPE_SERIALIZER_HEAD,      // SubDataType?
  DATATYPE_SERIALIZER_DUPLICATE, // SubDataType?
  DATATYPE_SERIALIZER_STATE,     // SubDataType?

  DATATYPE_ENDPOINT_HEAD,

  // These constants are shared by Debugger.dll, and cannot be conditionally compiled away.
  // This adds a couple extra bytes to the lookup table.  But frankly, the lookup table should probably
  // be shrunk to begin with.  Most of the datatypes are used just to tag memory.
  // For those datatypes, perhaps we should use a subDataType instead (probably what the comments above are about).

  DATATYPE_RADIO_LAST = DATATYPE_ENDPOINT_HEAD + 3,

  DATATYPE_IO_PORT,
  DATATYPE_IO_PORT_LAST = DATATYPE_RADIO_LAST + 1,

  DATATYPE_VTU_PORT_LAST = DATATYPE_IO_PORT_LAST + 1,

#if defined(NANOCLR_APPDOMAINS)
  DATATYPE_APPDOMAIN_HEAD,
  DATATYPE_TRANSPARENT_PROXY,
  DATATYPE_APPDOMAIN_ASSEMBLY,
#endif
  DATATYPE_APPDOMAIN_LAST = DATATYPE_VTU_PORT_LAST + 3,

  DATATYPE_FIRST_INVALID,

  // Type modifies. This is exact copy of VALUES ELEMENT_TYPE_* from CorHdr.h
  //

  DATATYPE_TYPE_MODIFIER = 0x40,
  DATATYPE_TYPE_SENTINEL = 0x01 | DATATYPE_TYPE_MODIFIER, // sentinel for varargs
  DATATYPE_TYPE_PINNED = 0x05 | DATATYPE_TYPE_MODIFIER,
  DATATYPE_TYPE_R4_HFA = 0x06 | DATATYPE_TYPE_MODIFIER, // used only internally for R4 HFA types
  DATATYPE_TYPE_R8_HFA = 0x07 | DATATYPE_TYPE_MODIFIER, // used only internally for R8 HFA types
  };

enum CLR_ReflectionType
  {
  REFLECTION_INVALID = 0x00,
  REFLECTION_ASSEMBLY = 0x01,
  REFLECTION_TYPE = 0x02,
  REFLECTION_TYPE_DELAYED = 0x03,
  REFLECTION_CONSTRUCTOR = 0x04,
  REFLECTION_METHOD = 0x05,
  REFLECTION_FIELD = 0x06,
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
inline uint32_t CLR_DataFromTk(uint32_t tk)
  {
  return tk & 0x00FFFFFF;
  }
inline CLR_TABLESENUM CLR_TypeFromTk(uint32_t tk)
  {
  return (CLR_TABLESENUM)(tk >> 24);
  }
inline uint32_t CLR_TkFromType(CLR_TABLESENUM tbl, uint32_t data)
  {
  return ((((uint32_t)tbl) << 24) & 0xFF000000) | (data & 0x00FFFFFF);
  }
#if 0
// Used on LE host to target BE
inline uint32_t     CLR_TkFromType(CLR_TABLESENUM tbl, uint32_t data) { return (((uint32_t)(tbl) & 0xFF) | (data & 0xFFFFFF00)); }
inline uint32_t     CLR_DataFromTk(uint32_t tk) { return                  tk & 0xFFFFFF00; }
inline CLR_TABLESENUM CLR_TypeFromTk(uint32_t tk) { return (CLR_TABLESENUM)(tk & 0xFF); }
#endif
//--//

inline uint32_t CLR_UncompressStringToken(uint32_t tk)
  {
  return CLR_TkFromType(TBL_Strings, tk);
  }

inline uint32_t CLR_UncompressTypeToken(uint32_t tk)
  {
  static const CLR_TABLESENUM c_lookup[3] = { TBL_TypeDef, TBL_TypeRef, TBL_TypeSpec };
  return CLR_TkFromType(c_lookup[(tk >> 14) & 3], 0x3fff & tk);
  }

inline uint32_t CLR_UncompressFieldToken(uint32_t tk)
  {
  static const CLR_TABLESENUM c_lookup[2] = { TBL_FieldDef, TBL_FieldRef };
  return CLR_TkFromType(c_lookup[(tk >> 15) & 1], 0x7fff & tk);
  }

inline uint32_t CLR_UncompressMethodToken(uint32_t tk)
  {
  static const CLR_TABLESENUM c_lookup[2] = { TBL_MethodDef, TBL_MethodRef };
  return CLR_TkFromType(c_lookup[(tk >> 15) & 1], 0x7fff & tk);
  }

#if defined(_WIN32)

extern uint32_t CLR_ReadTokenCompressed(CLR_PMETADATA& ip, CLR_OPCODE opcode);

#endif

//--//

HRESULT CLR_CompressTokenHelper(const CLR_TABLESENUM* tables, uint16_t cTables, uint32_t& tk);

inline HRESULT CLR_CompressStringToken(uint32_t& tk)
  {
  static const CLR_TABLESENUM c_lookup[1] = { TBL_Strings };

  return CLR_CompressTokenHelper(c_lookup, ARRAYSIZE(c_lookup), tk);
  }

inline HRESULT CLR_CompressTypeToken(uint32_t& tk)
  {
  static const CLR_TABLESENUM c_lookup[3] = { TBL_TypeDef, TBL_TypeRef, TBL_TypeSpec };

  return CLR_CompressTokenHelper(c_lookup, ARRAYSIZE(c_lookup), tk);
  }

inline HRESULT CLR_CompressFieldToken(uint32_t& tk)
  {
  static const CLR_TABLESENUM c_lookup[2] = { TBL_FieldDef, TBL_FieldRef };

  return CLR_CompressTokenHelper(c_lookup, ARRAYSIZE(c_lookup), tk);
  }

inline HRESULT CLR_CompressMethodToken(uint32_t& tk)
  {
  static const CLR_TABLESENUM c_lookup[2] = { TBL_MethodDef, TBL_MethodRef };

  return CLR_CompressTokenHelper(c_lookup, ARRAYSIZE(c_lookup), tk);
  }

//--//

inline bool CLR_CompressData(uint32_t val, uint8_t*& p)
  {
  uint8_t* ptr = p;

  if (val <= 0x7F)
    {
    *ptr++ = (uint8_t)(val);
    }
  else if (val <= 0x3FFF)
    {
    *ptr++ = (uint8_t)((val >> 8) | 0x80);
    *ptr++ = (uint8_t)((val));
    }
  else if (val <= 0x1FFFFFFF)
    {
    *ptr++ = (uint8_t)((val >> 24) | 0xC0);
    *ptr++ = (uint8_t)((val >> 16));
    *ptr++ = (uint8_t)((val >> 8));
    *ptr++ = (uint8_t)((val));
    }
  else
    {
    return false;
    }

  p = ptr;

  return true;
  }

inline uint32_t CLR_UncompressData(const uint8_t*& p)
  {
  CLR_PMETADATA ptr = p;
  uint32_t val = *ptr++;
  // Handle smallest data inline.
  if ((val & 0x80) == 0x00) // 0??? ????
    {
    }
  else if ((val & 0xC0) == 0x80) // 10?? ????
    {
    val = (val & 0x3F) << 8;
    val |= (uint32_t)*ptr++;
    }
  else // 110? ????
    {
    val = (val & 0x1F) << 24;
    val |= (uint32_t)*ptr++ << 16;
    val |= (uint32_t)*ptr++ << 8;
    val |= (uint32_t)*ptr++ << 0;
    }
#if 0
  // Handle smallest data inline.
  if ((val & 0x80) == 0x00)        // 0??? ????
    {
    }
  else if ((val & 0xC0) == 0x80)  // 10?? ????
    {
    val = (val & 0x3F);
    val |= ((uint32_t)*ptr++ << 8);
    }
  else // 110? ????
    {
    val = (val & 0x1F);
    val |= (uint32_t)*ptr++ << 8;
    val |= (uint32_t)*ptr++ << 16;
    val |= (uint32_t)*ptr++ << 24;
    }

#endif

  p = ptr;

  return val;
  }

inline CLR_DataType CLR_UncompressElementType(const uint8_t*& p)
  {
  return (CLR_DataType)*p++;
  }

inline uint32_t CLR_TkFromStream(const uint8_t*& p)
  {
  static const CLR_TABLESENUM c_lookup[4] = { TBL_TypeDef, TBL_TypeRef, TBL_TypeSpec, TBL_Max };

  uint32_t data = CLR_UncompressData(p);

  return CLR_TkFromType(c_lookup[data & 3], data >> 2);
  }

//--//--//--//

#if defined(__GNUC__)

#define NANOCLR_READ_UNALIGNED_UINT8(arg, ip)                                                                          \
    arg = *(const uint8_t *)ip;                                                                                      \
    ip += sizeof(uint8_t)
template <typename T> __inline void NANOCLR_READ_UNALIGNED_UINT16(T& arg, CLR_PMETADATA& ip)
  {
  arg = (uint16_t)(*(const uint8_t*)ip);
  ip += sizeof(uint8_t);
  arg |= (uint16_t)(*(const uint8_t*)ip) << 8;
  ip += sizeof(uint8_t);
  }
template <typename T> __inline void NANOCLR_READ_UNALIGNED_UINT32(T& arg, CLR_PMETADATA& ip)
  {
  arg = (uint32_t)(*(const uint8_t*)ip);
  ip += sizeof(uint8_t);
  arg |= (uint32_t)(*(const uint8_t*)ip) << 8;
  ip += sizeof(uint8_t);
  arg |= (uint32_t)(*(const uint8_t*)ip) << 16;
  ip += sizeof(uint8_t);
  arg |= (uint32_t)(*(const uint8_t*)ip) << 24;
  ip += sizeof(uint8_t);
  }
template <typename T> __inline void NANOCLR_READ_UNALIGNED_UINT64(T& arg, CLR_PMETADATA& ip)
  {
  arg = (uint64_t)(*(const uint8_t*)ip);
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 8;
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 16;
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 24;
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 32;
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 40;
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 48;
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 56;
  ip += sizeof(uint8_t);
  }

#define NANOCLR_READ_UNALIGNED_INT8(arg, ip)                                                                           \
    arg = *(const int8_t *)ip;                                                                                       \
    ip += sizeof(int8_t)
template <typename T> __inline void NANOCLR_READ_UNALIGNED_INT16(T& arg, CLR_PMETADATA& ip)
  {
  arg = (uint16_t)(*(const uint8_t*)ip);
  ip += sizeof(uint8_t);
  arg |= (uint16_t)(*(const uint8_t*)ip) << 8;
  ip += sizeof(uint8_t);
  arg = (int16_t)arg;
  }
template <typename T> __inline void NANOCLR_READ_UNALIGNED_INT32(T& arg, CLR_PMETADATA& ip)
  {
  arg = (uint32_t)(*(const uint8_t*)ip);
  ip += sizeof(uint8_t);
  arg |= (uint32_t)(*(const uint8_t*)ip) << 8;
  ip += sizeof(uint8_t);
  arg |= (uint32_t)(*(const uint8_t*)ip) << 16;
  ip += sizeof(uint8_t);
  arg |= (uint32_t)(*(const uint8_t*)ip) << 24;
  ip += sizeof(uint8_t);
  arg = (int32_t)arg;
  }
template <typename T> __inline void NANOCLR_READ_UNALIGNED_INT64(T& arg, CLR_PMETADATA& ip)
  {
  arg = (uint64_t)(*(const uint8_t*)ip);
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 8;
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 16;
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 24;
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 32;
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 40;
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 48;
  ip += sizeof(uint8_t);
  arg |= (uint64_t)(*(const uint8_t*)ip) << 56;
  ip += sizeof(uint8_t);
  arg = (int64_t)arg;
  }

#elif defined(_MSC_VER)

#define NANOCLR_READ_UNALIGNED_UINT8(arg, ip)                                                                          \
    arg = *(__declspec(align(1)) const uint8_t *)ip;                                                                 \
    ip += sizeof(uint8_t)
#define NANOCLR_READ_UNALIGNED_UINT16(arg, ip)                                                                         \
    arg = *(__declspec(align(1)) const uint16_t *)ip;                                                                \
    ip += sizeof(uint16_t)
#define NANOCLR_READ_UNALIGNED_UINT32(arg, ip)                                                                         \
    arg = *(__declspec(align(1)) const uint32_t *)ip;                                                                \
    ip += sizeof(uint32_t)
#define NANOCLR_READ_UNALIGNED_UINT64(arg, ip)                                                                         \
    arg = *(__declspec(align(1)) const uint64_t *)ip;                                                                \
    ip += sizeof(uint64_t)

#define NANOCLR_READ_UNALIGNED_INT8(arg, ip)                                                                           \
    arg = *(__declspec(align(1)) const int8_t *)ip;                                                                  \
    ip += sizeof(int8_t)
#define NANOCLR_READ_UNALIGNED_INT16(arg, ip)                                                                          \
    arg = *(__declspec(align(1)) const int16_t *)ip;                                                                 \
    ip += sizeof(int16_t)
#define NANOCLR_READ_UNALIGNED_INT32(arg, ip)                                                                          \
    arg = *(__declspec(align(1)) const int32_t *)ip;                                                                 \
    ip += sizeof(int32_t)
#define NANOCLR_READ_UNALIGNED_INT64(arg, ip)                                                                          \
    arg = *(__declspec(align(1)) const int64_t *)ip;                                                                 \
    ip += sizeof(int64_t)

//--//

#define NANOCLR_WRITE_UNALIGNED_UINT8(ip, arg)                                                                         \
    *(__declspec(align(1)) uint8_t *)ip = arg;                                                                       \
    ip += sizeof(uint8_t)
#define NANOCLR_WRITE_UNALIGNED_UINT16(ip, arg)                                                                        \
    *(__declspec(align(1)) uint16_t *)ip = arg;                                                                      \
    ip += sizeof(uint16_t)
#define NANOCLR_WRITE_UNALIGNED_UINT32(ip, arg)                                                                        \
    *(__declspec(align(1)) uint32_t *)ip = arg;                                                                      \
    ip += sizeof(uint32_t)
#define NANOCLR_WRITE_UNALIGNED_UINT64(ip, arg)                                                                        \
    *(__declspec(align(1)) uint64_t *)ip = arg;                                                                      \
    ip += sizeof(uint64_t)

#define NANOCLR_WRITE_UNALIGNED_INT8(ip, arg)                                                                          \
    *(__declspec(align(1)) int8_t *)ip = arg;                                                                        \
    ip += sizeof(int8_t)
#define NANOCLR_WRITE_UNALIGNED_INT16(ip, arg)                                                                         \
    *(__declspec(align(1)) int16_t *)ip = arg;                                                                       \
    ip += sizeof(int16_t)
#define NANOCLR_WRITE_UNALIGNED_INT32(ip, arg)                                                                         \
    *(__declspec(align(1)) int32_t *)ip = arg;                                                                       \
    ip += sizeof(int32_t)
#define NANOCLR_WRITE_UNALIGNED_INT64(ip, arg)                                                                         \
    *(__declspec(align(1)) int64_t *)ip = arg;                                                                       \
    ip += sizeof(int64_t)

#else // TODO: __packed is compiler specific... Which compiler is this for?

#define NANOCLR_READ_UNALIGNED_UINT8(arg, ip)                                                                          \
    arg = *(__packed uint8_t *)ip;                                                                                   \
    ip += sizeof(uint8_t)
#define NANOCLR_READ_UNALIGNED_UINT16(arg, ip)                                                                         \
    arg = *(__packed uint16_t *)ip;                                                                                  \
    ip += sizeof(uint16_t)
#define NANOCLR_READ_UNALIGNED_UINT32(arg, ip)                                                                         \
    arg = *(__packed uint32_t *)ip;                                                                                  \
    ip += sizeof(uint32_t)
#define NANOCLR_READ_UNALIGNED_UINT64(arg, ip)                                                                         \
    arg = *(__packed uint64_t *)ip;                                                                                  \
    ip += sizeof(uint64_t)
#define NANOCLR_READ_UNALIGNED_INT8(arg, ip)                                                                           \
    arg = *(__packed int8_t *)ip;                                                                                    \
    ip += sizeof(int8_t)
#define NANOCLR_READ_UNALIGNED_INT16(arg, ip)                                                                          \
    arg = *(__packed int16_t *)ip;                                                                                   \
    ip += sizeof(int16_t)
#define NANOCLR_READ_UNALIGNED_INT32(arg, ip)                                                                          \
    arg = *(__packed int32_t *)ip;                                                                                   \
    ip += sizeof(int32_t)
#define NANOCLR_READ_UNALIGNED_INT64(arg, ip)                                                                          \
    arg = *(__packed int64_t *)ip;                                                                                   \
    ip += sizeof(int64_t)

//--//

#define NANOCLR_WRITE_UNALIGNED_UINT8(ip, arg)                                                                         \
    *(__packed uint8_t *)ip = arg;                                                                                   \
    ip += sizeof(uint8_t)
#define NANOCLR_WRITE_UNALIGNED_UINT16(ip, arg)                                                                        \
    *(__packed uint16_t *)ip = arg;                                                                                  \
    ip += sizeof(uint16_t)
#define NANOCLR_WRITE_UNALIGNED_UINT32(ip, arg)                                                                        \
    *(__packed uint32_t *)ip = arg;                                                                                  \
    ip += sizeof(uint32_t)
#define NANOCLR_WRITE_UNALIGNED_UINT64(ip, arg)                                                                        \
    *(__packed uint64_t *)ip = arg;                                                                                  \
    ip += sizeof(uint64_t)

#define NANOCLR_WRITE_UNALIGNED_INT8(ip, arg)                                                                          \
    *(__packed int8_t *)ip = arg;                                                                                    \
    ip += sizeof(int8_t)
#define NANOCLR_WRITE_UNALIGNED_INT16(ip, arg)                                                                         \
    *(__packed int16_t *)ip = arg;                                                                                   \
    ip += sizeof(int16_t)
#define NANOCLR_WRITE_UNALIGNED_INT32(ip, arg)                                                                         \
    *(__packed int32_t *)ip = arg;                                                                                   \
    ip += sizeof(int32_t)
#define NANOCLR_WRITE_UNALIGNED_INT64(ip, arg)                                                                         \
    *(__packed int64_t *)ip = arg;                                                                                   \
    ip += sizeof(int64_t)

#endif

//--//
#define NANOCLR_READ_UNALIGNED_OPCODE(op, ip)                                                                          \
    op = CLR_OPCODE(*ip++);                                                                                            \
    if (op == CEE_PREFIX1)                                                                                             \
    {                                                                                                                  \
        opcode = CLR_OPCODE(*ip++ + 256);                                                                              \
    }

#define NANOCLR_READ_UNALIGNED_COMPRESSED_FIELDTOKEN(arg, ip)                                                          \
    NANOCLR_READ_UNALIGNED_UINT16(arg, ip);                                                                            \
    arg = CLR_UncompressFieldToken(arg)
#define NANOCLR_READ_UNALIGNED_COMPRESSED_METHODTOKEN(arg, ip)                                                         \
    NANOCLR_READ_UNALIGNED_UINT16(arg, ip);                                                                            \
    arg = CLR_UncompressMethodToken(arg)
#define NANOCLR_READ_UNALIGNED_COMPRESSED_TYPETOKEN(arg, ip)                                                           \
    NANOCLR_READ_UNALIGNED_UINT16(arg, ip);                                                                            \
    arg = CLR_UncompressTypeToken(arg)
#define NANOCLR_READ_UNALIGNED_COMPRESSED_STRINGTOKEN(arg, ip)                                                         \
    NANOCLR_READ_UNALIGNED_UINT16(arg, ip);                                                                            \
    arg = CLR_UncompressStringToken(arg)

//--//

inline CLR_OPCODE CLR_ReadNextOpcode(CLR_PMETADATA& ip)
  {
  CLR_PMETADATA ptr = ip;
  CLR_OPCODE opcode = CLR_OPCODE(*ptr++);

  if (opcode == CEE_PREFIX1)
    {
    opcode = CLR_OPCODE(*ptr++ + 256);
    }

  ip = ptr;

  return opcode;
  }

inline CLR_OPCODE CLR_ReadNextOpcodeCompressed(CLR_PMETADATA& ip)
  {
  CLR_PMETADATA ptr = ip;
  CLR_OPCODE opcode = CLR_OPCODE(*ptr++);

  if (opcode == CEE_PREFIX1)
    {
    opcode = CLR_OPCODE(*ptr++ + 256);
    }

  ip = ptr;

  return opcode;
  }

//--//

#define FETCH_ARG_UINT8(arg, ip)                                                                                       \
    uint32_t arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_UINT8(arg, ip)
#define FETCH_ARG_UINT16(arg, ip)                                                                                      \
    uint32_t arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_UINT16(arg, ip)
#define FETCH_ARG_UINT32(arg, ip)                                                                                      \
    uint32_t arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_UINT32(arg, ip)
#define FETCH_ARG_UINT64(arg, ip)                                                                                      \
    uint64_t arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_UINT64(arg, ip)

#define FETCH_ARG_INT8(arg, ip)                                                                                        \
    int32_t arg;                                                                                                     \
    NANOCLR_READ_UNALIGNED_INT8(arg, ip)
#define FETCH_ARG_INT16(arg, ip)                                                                                       \
    int32_t arg;                                                                                                     \
    NANOCLR_READ_UNALIGNED_INT16(arg, ip)
#define FETCH_ARG_INT32(arg, ip)                                                                                       \
    int32_t arg;                                                                                                     \
    NANOCLR_READ_UNALIGNED_INT32(arg, ip)
#define FETCH_ARG_INT64(arg, ip)                                                                                       \
    int64_t arg;                                                                                                     \
    NANOCLR_READ_UNALIGNED_INT64(arg, ip)

#define FETCH_ARG_COMPRESSED_STRINGTOKEN(arg, ip)                                                                      \
    uint32_t arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_COMPRESSED_STRINGTOKEN(arg, ip)
#define FETCH_ARG_COMPRESSED_FIELDTOKEN(arg, ip)                                                                       \
    uint32_t arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_COMPRESSED_FIELDTOKEN(arg, ip)
#define FETCH_ARG_COMPRESSED_TYPETOKEN(arg, ip)                                                                        \
    uint32_t arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_COMPRESSED_TYPETOKEN(arg, ip)
#define FETCH_ARG_COMPRESSED_METHODTOKEN(arg, ip)                                                                      \
    uint32_t arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_COMPRESSED_METHODTOKEN(arg, ip)
#define FETCH_ARG_TOKEN(arg, ip)                                                                                       \
    uint32_t arg;                                                                                                    \
    NANOCLR_READ_UNALIGNED_UINT32(arg, ip)

//--//

#if defined(_WIN32)

CLR_PMETADATA CLR_SkipBodyOfOpcode(CLR_PMETADATA ip, CLR_OPCODE opcode);
CLR_PMETADATA CLR_SkipBodyOfOpcodeCompressed(CLR_PMETADATA ip, CLR_OPCODE opcode);

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

extern bool CLR_SafeSprintfV(char*& szBuffer, size_t& iBuffer, const char* format, va_list arg);
extern bool CLR_SafeSprintf(char*& szBuffer, size_t& iBuffer, const char* format, ...);

#if !defined(BUILD_RTM)

#define NANOCLR_DEBUG_STOP() _ASSERTE(false)

#else

#define NANOCLR_DEBUG_STOP() CPU_Reset()

#endif

//--//

struct CLR_Debug
  {
  static int PrintfV(const char* format, va_list arg);
  static int Printf(const char* format, ...);
  static void Emit(const char* text, int len);
  static void Flush();

  //--//

  typedef int (*OutputHandler)(const char* format, ...);

#if defined(_WIN32)
  static void RedirectToString(std::string* str);
  static void SaveMessage(std::string str);
#endif
  };

////////////////////////////////////////////////////////////////////////////////

struct CLR_RECORD_VERSION
  {
  uint16_t iMajorVersion;
  uint16_t iMinorVersion;
  uint16_t iBuildNumber;
  uint16_t iRevisionNumber;
  };

struct CLR_RECORD_ASSEMBLY
  {
  static const uint32_t c_Flags_NeedReboot = 0x00000001;

  uint8_t marker[8];
  //
  uint32_t headerCRC;
  uint32_t assemblyCRC;
  uint32_t flags;
  //
  uint32_t nativeMethodsChecksum;
  uint32_t patchEntryOffset;
  //
  CLR_RECORD_VERSION version;
  //
  uint16_t assemblyName; // TBL_Strings
  uint16_t stringTableVersion;
  //
  uint32_t startOfTables[TBL_Max];
  uint32_t numOfPatchedMethods;
  //
  // For every table, a number of bytes that were padded to the end of the table
  // to align to unsigned long.  Each table starts at a unsigned long boundary, and ends
  // at a unsigned long boundary.  Some of these tables will, by construction, have
  // no padding, and all will have values in the range [0-3].  This isn't the most
  // compact form to hold this information, but it only costs 16 bytes/assembly.
  // Trying to only align some of the tables is just much more hassle than it's worth.
  // And, of course, this field also has to be unsigned long-aligned.
  uint8_t paddingOfTables[((TBL_Max - 1) + 3) / 4 * 4];
  //--//

  bool GoodHeader() const;
  bool GoodAssembly() const;

#if defined(_WIN32)
  void ComputeCRC();
#endif

  uint32_t SizeOfTable(CLR_TABLESENUM tbl) const
    {
    return startOfTables[tbl + 1] - startOfTables[tbl] - paddingOfTables[tbl];
    }

  uint32_t TotalSize() const
    {
    return startOfTables[TBL_EndOfAssembly];
    }

  //--//

  static uint32_t ComputeAssemblyHash(const char* name, const CLR_RECORD_VERSION& ver);
  };

struct CLR_RECORD_ASSEMBLYREF
  {
  uint16_t name; // TBL_Strings
  uint16_t pad;
  //
  CLR_RECORD_VERSION version;
  };

struct CLR_RECORD_TYPEREF
  {
  uint16_t name;      // TBL_Strings
  uint16_t nameSpace; // TBL_Strings
  //
  uint16_t scope; // TBL_AssemblyRef | TBL_TypeRef // 0x8000
  uint16_t pad;
  };

struct CLR_RECORD_FIELDREF
  {
  uint16_t name;   // TBL_Strings
  uint16_t container; // TBL_TypeRef
  //
  uint16_t sig; // TBL_Signatures
  uint16_t pad;
  };

struct CLR_RECORD_METHODREF
  {
  uint16_t name;   // TBL_Strings
  uint16_t container; // TBL_TypeRef
  //
  uint16_t sig; // TBL_Signatures
  uint16_t pad;
  };

struct CLR_RECORD_TYPEDEF
  {
  static const uint16_t TD_Scope_Mask = 0x0007;
  static const uint16_t TD_Scope_NotPublic = 0x0000;         // Class is not public scope.
  static const uint16_t TD_Scope_Public = 0x0001;            // Class is public scope.
  static const uint16_t TD_Scope_NestedPublic = 0x0002;      // Class is nested with public visibility.
  static const uint16_t TD_Scope_NestedPrivate = 0x0003;     // Class is nested with private visibility.
  static const uint16_t TD_Scope_NestedFamily = 0x0004;      // Class is nested with family visibility.
  static const uint16_t TD_Scope_NestedAssembly = 0x0005;    // Class is nested with assembly visibility.
  static const uint16_t TD_Scope_NestedFamANDAssem = 0x0006; // Class is nested with family and assembly visibility.
  static const uint16_t TD_Scope_NestedFamORAssem = 0x0007;  // Class is nested with family or assembly visibility.

  static const uint16_t TD_Serializable = 0x0008;

  static const uint16_t TD_Semantics_Mask = 0x0030;
  static const uint16_t TD_Semantics_Class = 0x0000;
  static const uint16_t TD_Semantics_ValueType = 0x0010;
  static const uint16_t TD_Semantics_Interface = 0x0020;
  static const uint16_t TD_Semantics_Enum = 0x0030;

  static const uint16_t TD_Abstract = 0x0040;
  static const uint16_t TD_Sealed = 0x0080;

  static const uint16_t TD_SpecialName = 0x0100;
  static const uint16_t TD_Delegate = 0x0200;
  static const uint16_t TD_MulticastDelegate = 0x0400;

  static const uint16_t TD_Patched = 0x0800;

  static const uint16_t TD_BeforeFieldInit = 0x1000;
  static const uint16_t TD_HasSecurity = 0x2000;
  static const uint16_t TD_HasFinalizer = 0x4000;
  static const uint16_t TD_HasAttributes = 0x8000;

  uint16_t name;      // TBL_Strings
  uint16_t nameSpace; // TBL_Strings
  //
  uint16_t extends;       // TBL_TypeDef | TBL_TypeRef // 0x8000
  uint16_t enclosingType; // TBL_TypeDef
  //
  uint16_t interfaces;    // TBL_Signatures
  uint16_t methods_First; // TBL_MethodDef
  //
  uint8_t vMethods_Num;
  uint8_t iMethods_Num;
  uint8_t sMethods_Num;
  uint8_t dataType;
  //
  uint16_t sFields_First; // TBL_FieldDef
  uint16_t iFields_First; // TBL_FieldDef
  //
  uint8_t sFields_Num;
  uint8_t iFields_Num;
  uint16_t flags;

  //--//

  bool IsEnum() const
    {
    return (flags & (TD_Semantics_Mask)) == TD_Semantics_Enum;
    }
  bool IsDelegate() const
    {
    return (flags & (TD_Delegate | TD_MulticastDelegate)) != 0;
    }
  };

struct CLR_RECORD_FIELDDEF
  {
  static const uint16_t FD_Scope_Mask = 0x0007;
  static const uint16_t FD_Scope_PrivateScope = 0x0000; // Member not referenceable.
  static const uint16_t FD_Scope_Private = 0x0001;      // Accessible only by the parent type.
  static const uint16_t FD_Scope_FamANDAssem = 0x0002;  // Accessible by sub-types only in this Assembly.
  static const uint16_t FD_Scope_Assembly = 0x0003;     // Accessibly by anyone in the Assembly.
  static const uint16_t FD_Scope_Family = 0x0004;       // Accessible only by type and sub-types.
  static const uint16_t FD_Scope_FamORAssem = 0x0005; // Accessibly by sub-types anywhere, plus anyone in assembly.
  static const uint16_t FD_Scope_Public = 0x0006;     // Accessibly by anyone who has visibility to this scope.

  static const uint16_t FD_NotSerialized = 0x0008; // Field does not have to be serialized when type is remoted.

  static const uint16_t FD_Static = 0x0010;   // Defined on type, else per instance.
  static const uint16_t FD_InitOnly = 0x0020; // Field may only be initialized, not written to after init.
  static const uint16_t FD_Literal = 0x0040;  // Value is compile time constant.

  static const uint16_t FD_SpecialName = 0x0100; // field is special.  Name describes how.
  static const uint16_t FD_HasDefault = 0x0200;  // Field has default.
  static const uint16_t FD_HasFieldRVA = 0x0400; // Field has RVA.

  static const uint16_t FD_NoReflection = 0x0800; // field does not allow reflection

  static const uint16_t FD_HasAttributes = 0x8000;

  uint16_t name; // TBL_Strings
  uint16_t sig;     // TBL_Signatures
  //
  uint16_t defaultValue; // TBL_Signatures
  uint16_t flags;
  };

struct CLR_RECORD_METHODDEF
  {
  static const uint32_t MD_Scope_Mask = 0x00000007;
  static const uint32_t MD_Scope_PrivateScope = 0x00000000; // Member not referenceable.
  static const uint32_t MD_Scope_Private = 0x00000001;      // Accessible only by the parent type.
  static const uint32_t MD_Scope_FamANDAssem = 0x00000002;  // Accessible by sub-types only in this Assembly.
  static const uint32_t MD_Scope_Assem = 0x00000003;        // Accessibly by anyone in the Assembly.
  static const uint32_t MD_Scope_Family = 0x00000004;       // Accessible only by type and sub-types.
  static const uint32_t MD_Scope_FamORAssem =
    0x00000005;                                       // Accessibly by sub-types anywhere, plus anyone in assembly.
  static const uint32_t MD_Scope_Public = 0x00000006; // Accessibly by anyone who has visibility to this scope.

  static const uint32_t MD_Static = 0x00000010;    // Defined on type, else per instance.
  static const uint32_t MD_Final = 0x00000020;     // Method may not be overridden.
  static const uint32_t MD_Virtual = 0x00000040;   // Method virtual.
  static const uint32_t MD_HideBySig = 0x00000080; // Method hides by name+sig, else just by name.

  static const uint32_t MD_VtableLayoutMask = 0x00000100;
  static const uint32_t MD_ReuseSlot = 0x00000000;   // The default.
  static const uint32_t MD_NewSlot = 0x00000100;     // Method always gets a new slot in the vtable.
  static const uint32_t MD_Abstract = 0x00000200;    // Method does not provide an implementation.
  static const uint32_t MD_SpecialName = 0x00000400; // Method is special.  Name describes how.
  static const uint32_t MD_NativeProfiled = 0x00000800;

  static const uint32_t MD_Constructor = 0x00001000;
  static const uint32_t MD_StaticConstructor = 0x00002000;
  static const uint32_t MD_Finalizer = 0x00004000;

  static const uint32_t MD_DelegateConstructor = 0x00010000;
  static const uint32_t MD_DelegateInvoke = 0x00020000;
  static const uint32_t MD_DelegateBeginInvoke = 0x00040000;
  static const uint32_t MD_DelegateEndInvoke = 0x00080000;

  static const uint32_t MD_Synchronized = 0x01000000;
  static const uint32_t MD_GloballySynchronized = 0x02000000;
  static const uint32_t MD_Patched = 0x04000000;
  static const uint32_t MD_EntryPoint = 0x08000000;
  static const uint32_t MD_RequireSecObject = 0x10000000; // Method calls another method containing security code.
  static const uint32_t MD_HasSecurity = 0x20000000;      // Method has security associate with it.
  static const uint32_t MD_HasExceptionHandlers = 0x40000000;
  static const uint32_t MD_HasAttributes = 0x80000000;

  uint16_t name; // TBL_Strings
  uint16_t RVA;
  //
  uint32_t flags;
  //
  uint8_t retVal;
  uint8_t numArgs;
  uint8_t numLocals;
  uint8_t lengthEvalStack;
  //
  uint16_t locals; // TBL_Signatures
  uint16_t sig;    // TBL_Signatures
  };

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
// pragma required here because compiler is not too happy with the cast to uint32_t* from a uint16_t variable

struct CLR_RECORD_ATTRIBUTE
  {
  uint16_t ownerType; // one of TBL_TypeDef, TBL_MethodDef, or TBL_FieldDef.
  uint16_t ownerIdx;  // TBL_TypeDef | TBL_MethodDef | TBL_FielfDef
  uint16_t constructor;
  uint16_t data; // TBL_Signatures

  uint32_t Key() const
    {
    return *(uint32_t*)&ownerType;
    }
  };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

struct CLR_RECORD_TYPESPEC
  {
  uint16_t sig; // TBL_Signatures
  uint16_t pad;
  };

struct CLR_RECORD_EH
  {
  static const uint16_t EH_Catch = 0x0000;
  static const uint16_t EH_CatchAll = 0x0001;
  static const uint16_t EH_Finally = 0x0002;
  static const uint16_t EH_Filter = 0x0003;

  //--//

  uint16_t mode;
  union {
    uint16_t classToken; // TBL_TypeDef | TBL_TypeRef
    uint16_t filterStart;
    };
  uint16_t tryStart;
  uint16_t tryEnd;
  uint16_t handlerStart;
  uint16_t handlerEnd;

  //--//

  static CLR_PMETADATA ExtractEhFromByteCode(CLR_PMETADATA ipEnd, const CLR_RECORD_EH*& ptrEh, uint32_t& numEh);

  uint32_t GetToken() const;
  };

CT_ASSERT_UNIQUE_NAME(sizeof(CLR_RECORD_EH) == 12, CLR_RECORD_EH)

struct CLR_RECORD_RESOURCE_FILE
  {
  static const uint32_t CURRENT_VERSION = 2;

  uint32_t version;
  uint32_t sizeOfHeader;
  uint32_t sizeOfResourceHeader;
  uint32_t numberOfResources;
  uint16_t name; // TBL_Strings
  uint16_t pad;
  uint32_t offset; // TBL_Resource
  };

struct CLR_RECORD_RESOURCE
  {
  static const uint8_t RESOURCE_Invalid = 0x00;
  static const uint8_t RESOURCE_Bitmap = 0x01;
  static const uint8_t RESOURCE_Font = 0x02;
  static const uint8_t RESOURCE_String = 0x03;
  static const uint8_t RESOURCE_Binary = 0x04;

  static const uint8_t FLAGS_PaddingMask = 0x03;
  static const int16_t SENTINEL_ID = 0x7FFF;

  //
  // Sorted on id
  //
  int16_t id;
  uint8_t kind;
  uint8_t flags;
  uint32_t offset;
  };

#if defined(_MSC_VER)
#pragma pack(pop, NANOCLR_TYPES_H)
#endif

#endif // _NANOCLR_TYPES_H_
