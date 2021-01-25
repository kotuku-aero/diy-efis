//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_RUNTIME__HEAPBLOCK_H_
#define _NANOCLR_RUNTIME__HEAPBLOCK_H_

////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLR_RT_HEAPBLOCK_RAW_ID(dataType, flags, size)                                                                 \
    ((dataType & 0x000000FF) | ((flags & 0x000000FF) << 8) | ((size & 0x0000FFFF) << 16))
#define CLR_RT_HEAPBLOCK_ASSIGN_INTEGER32_SIGNED(dataType, num)                                                        \
    {                                                                                                                  \
        m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(dataType, 0, 1);                                                            \
        m_data.numeric.s4 = (int32_t)num;                                                                            \
    }
#define CLR_RT_HEAPBLOCK_ASSIGN_INTEGER64_SIGNED(dataType, num)                                                        \
    {                                                                                                                  \
        m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(dataType, 0, 1);                                                            \
        m_data.numeric.s8 = num;                                                                                       \
    }

#define CLR_RT_HEAPBLOCK_ASSIGN_INTEGER32_UNSIGNED(dataType, num)                                                      \
    {                                                                                                                  \
        m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(dataType, 0, 1);                                                            \
        m_data.numeric.u4 = (uint32_t)num;                                                                           \
    }
#define CLR_RT_HEAPBLOCK_ASSIGN_INTEGER64_UNSIGNED(dataType, num)                                                      \
    {                                                                                                                  \
        m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(dataType, 0, 1);                                                            \
        m_data.numeric.u8 = num;                                                                                       \
    }

#define CLR_RT_HEAPBLOCK_ASSIGN_FLOAT32(dataType, num)                                                                 \
    {                                                                                                                  \
        m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(dataType, 0, 1);                                                            \
        m_data.numeric.r4 = num;                                                                                       \
    }
#define CLR_RT_HEAPBLOCK_ASSIGN_FLOAT64(dataType, num)                                                                 \
    {                                                                                                                  \
        m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(dataType, 0, 1);                                                            \
        m_data.numeric.r8 = num;                                                                                       \
    }

////////////////////////////////////////////////////////////

#define CLR_RT_HEAPBLOCK_RELOCATE(ptr)                                                                                 \
    {                                                                                                                  \
        CLR_DataType dt = ptr->DataType();                                                                             \
                                                                                                                       \
        if (dt > DATATYPE_LAST_NONPOINTER && dt < DATATYPE_FIRST_INVALID)                                              \
        {                                                                                                              \
            CLR_RT_HeapBlockRelocate rel = c_CLR_RT_DataTypeLookup[dt].m_relocate;                                     \
            if (rel)                                                                                                   \
            {                                                                                                          \
                (ptr->*rel)();                                                                                         \
            }                                                                                                          \
        }                                                                                                              \
    }

////////////////////////////////////////////////////////////

//
// This is used in memory move operations.
//
struct CLR_RT_HeapBlock_Raw
  {
  uint32_t data[3];
  };

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

struct CLR_RT_HeapBlock
  {
  friend struct CLR_RT_HeapBlock_Node;
  friend struct CLR_RT_DblLinkedList;

  friend struct MethodCompiler;
  //--//

  static const uint32_t HB_Alive = 0x01;
  static const uint32_t HB_KeepAlive = 0x02;
  static const uint32_t HB_Event = 0x04;
  static const uint32_t HB_Pinned = 0x08;
  static const uint32_t HB_Boxed = 0x10;
  static const uint32_t HB_Unused20 = 0x20;
  // If more bits are needed, HB_Signaled and HB_SignalAutoReset can be freed for use with a little work.
  // It is not necessary that any heapblock can be waited upon.  Currently, only Threads (Thread.Join),
  // ManualResetEvent, and AutoResetEvent are waitable objects.
  static const uint32_t HB_Signaled = 0x40;
  static const uint32_t HB_SignalAutoReset = 0x80;

  static const uint32_t HB_Unmovable = HB_Pinned | HB_Event;

  //
  // These are special flags used to control allocation.
  //
  static const uint32_t HB_InitializeToZero = 0x00100;
  static const uint32_t HB_NoGcOnFailure = 0x00200;
  static const uint32_t HB_SpecialGCAllocation = 0x00400;
  static const uint32_t HB_CompactOnFailure = 0x00800;

  static const uint32_t HB_NoGcOnFailedAllocation = HB_SpecialGCAllocation | HB_NoGcOnFailure;
  static const uint32_t HB_MaxSize = 0x0000FFFF;

  static const uint32_t HB_Object_Fields_Offset = 1;

  //--//

  private:
    union CLR_RT_HeapBlock_Id {
      struct Type
        {
        uint8_t dataType; // CLR_DataType
        uint8_t flags;    // HB_*
        uint16_t size;
        } type;

      uint32_t raw;
      } m_id;

    union CLR_RT_HeapBlock_AtomicData {
      struct NodeLink
        {
        CLR_RT_HeapBlock_Node *nextBlock;
        CLR_RT_HeapBlock_Node *prevBlock;
        } nodeLink;

      union Numeric {
        uint8_t u1;
        uint16_t u2;
        uint32_t u4;

        struct U8
          {
          uint32_t LL;
          uint32_t HH;

          operator uint64_t() const
            {
            return ((uint64_t)HH << 32 | (uint64_t)LL);
            }

          U8 &operator=(const uint64_t num)
            {
            LL = (uint32_t)((ULONGLONGCONSTANT(0x00000000FFFFFFFF) & num));
            HH = (uint32_t)((ULONGLONGCONSTANT(0xFFFFFFFF00000000) & num) >> 32);
            return *this;
            }
          U8 &operator+=(const U8 &num)
            {
            uint64_t value = (uint64_t)*this; // uses conversion
            value += (uint64_t)num;               // uses conversion and then built-in type __int64
            *this = value;                          // uses assignment operator (operator=)
            return *this;
            }

          U8 &operator-=(const U8 &num)
            {
            uint64_t value = (uint64_t)*this; // uses conversion
            value -= (uint64_t)num;               // uses conversion and then built-in type __int64
            *this = value;                          // uses assignment operator (operator=)
            return *this;
            }

          U8 operator*(const U8 &num)
            {
            uint64_t value = (uint64_t)*this;
            U8 ret_value;
            value *= (uint64_t)num; // uses conversion and then built-in type __int64
            ret_value = value;
            return ret_value;
            }

          U8 operator/(const U8 &num)
            {
            uint64_t value = (uint64_t)*this;
            U8 ret_value;
            value /= (uint64_t)num; // uses conversion and then built-in type __int64
            ret_value = value;
            return ret_value;
            }

          U8 operator~()
            {
            U8 ret_value;
            ret_value.LL = ~LL;
            ret_value.HH = ~HH;
            return ret_value;
            }

          U8 &operator%=(const U8 &num)
            {
            uint64_t value = (uint64_t)*this; // uses conversion
            value %= (uint64_t)num;               // uses conversion and then built-in type __int64
            *this = value;                          // uses assignment operator (operator=)
            return *this;
            }

          U8 &operator&=(const U8 &num)
            {
            LL &= num.LL;
            HH &= num.HH;
            return *this;
            }

          U8 &operator|=(const U8 &num)
            {
            LL |= num.LL;
            HH |= num.HH;
            return *this;
            }

          U8 &operator^=(const U8 &num)
            {
            LL ^= num.LL;
            HH ^= num.HH;
            return *this;
            }

          U8 &operator>>=(const uint32_t num)
            {

            uint64_t value = (uint64_t)*this; // uses conversion
            value >>= num;                          // uses conversion and then built-in type __int64
            *this = value;                          // uses assignment operator (operator=)
            return *this;
            }

          U8 &operator<<=(const uint32_t num)
            {
            uint64_t value = (uint64_t)*this; // uses conversion
            value <<= num;                          // uses conversion and then built-in type __int64
            *this = value;                          // uses assignment operator (operator=)
            return *this;
            }

          bool operator<(const U8 &num)
            {
            uint64_t value = (uint64_t)*this; // uses conversion
            return (value < (uint64_t)num);
            }

          bool operator>(const U8 &num)
            {
            uint64_t value = (uint64_t)*this; // uses conversion
            return (value > (uint64_t)num);
            }

          bool operator==(const U8 &num)
            {
            uint64_t value = (uint64_t)*this; // uses conversion
            return (value == (uint64_t)num);
            }

          bool operator==(const uint64_t num)
            {
            uint64_t value = (uint64_t)*this; // uses conversion
            return (value == num);
            }

          } u8;
        //
        int8_t s1;
        int16_t s2;
        int32_t s4;

        struct S8
          {
          uint32_t LL;
          uint32_t HH;

          operator int64_t() const
            {
            return (((uint64_t)HH) << 32 | (uint64_t)LL);
            }

          S8 &operator=(const int64_t num)
            {
            LL = (uint32_t)((ULONGLONGCONSTANT(0x00000000FFFFFFFF) & num));
            HH = (uint32_t)((ULONGLONGCONSTANT(0xFFFFFFFF00000000) & num) >> 32);
            return *this;
            }

          S8 &operator+=(const S8 &num)
            {
            int64_t value = (int64_t)*this; // uses conversion
            value += (int64_t)num;              // uses conversion and then built-in type __int64
            *this = value;                        // uses assignment operator (operator=)
            return *this;
            }

          S8 &operator-=(const S8 &num)
            {
            int64_t value = (int64_t)*this; // uses conversion
            value -= (int64_t)num;              // uses conversion and then built-in type __int64
            *this = value;                        // uses assignment operator (operator=)
            return *this;
            }

          S8 operator*(const S8 &num)
            {
            int64_t value = (int64_t)*this;
            S8 ret_value;
            value *= (int64_t)num; // uses conversion and then built-in type __int64
            ret_value = value;
            return ret_value;
            }

          S8 operator/(const S8 &num)
            {
            int64_t value = (int64_t)*this;
            S8 ret_value;
            value /= (int64_t)num; // uses conversion and then built-in type __int64
            ret_value = value;
            return ret_value;
            }

          S8 operator~()
            {
            S8 ret_value;
            ret_value.LL = ~LL;
            ret_value.HH = ~HH;
            return ret_value;
            }

          S8 &operator%=(const S8 &num)
            {
            int64_t value = (int64_t)*this; // uses conversion
            value %= (int64_t)num;              // uses conversion and then built-in type __int64
            *this = value;                        // uses assignment operator (operator=)
            return *this;
            }

          S8 &operator&=(const S8 &num)
            {
            LL &= num.LL;
            HH &= num.HH;
            return *this;
            }

          S8 &operator|=(const S8 &num)
            {
            LL |= num.LL;
            HH |= num.HH;
            return *this;
            }

          S8 &operator^=(const S8 &num)
            {
            LL ^= num.LL;
            HH ^= num.HH;
            return *this;
            }

          S8 &operator>>=(const uint32_t num)
            {
            int64_t value = (int64_t)*this; // uses conversion
            value >>= num;                        // uses conversion and then built-in type __int64
            *this = value;                        // uses assignment operator (operator=)
            return *this;
            }

          S8 &operator<<=(const uint32_t num)
            {
            int64_t value = (int64_t)*this; // uses conversion
            value <<= num;                        // uses conversion and then built-in type __int64
            *this = value;                        // uses assignment operator (operator=)
            return *this;
            }

          bool operator<(const S8 &num)
            {
            int64_t value = (int64_t)*this; // uses conversion
            return (value < (int64_t)num);
            }

          bool operator>(const S8 &num)
            {
            int64_t value = (int64_t)*this; // uses conversion
            return (value > (int64_t)num);
            }

          bool operator==(const S8 &num)
            {
            int64_t value = (int64_t)*this; // uses conversion
            return (value == (int64_t)num);
            }

          bool operator==(const int64_t num)
            {
            int64_t value = (int64_t)*this; // uses conversion
            return (value == num);
            }

          } s8;
        //

        float r4;

        struct R8
          {
          uint32_t LL;
          uint32_t HH;

          operator double() const
            {
            double ret_val;

#if defined(__GNUC__)
            ///
            /// UNDONE: FIXME: This code fixes an optimization problem with the gcc compiler.
            /// When the optimization level is greater than zero, the gcc compiler
            /// code will not work with the unsigned int* conversion, it requires you
            /// to copy byte by byte.
            ///
            uint8_t *tmp = (uint8_t *)&ret_val;
            uint8_t *src = (uint8_t *)&LL;
            uint32_t i;

            for (i = 0; i < sizeof(uint32_t); i++)
              {
              *tmp++ = *src++;
              }

            src = (uint8_t *)&HH;
            for (i = 0; i < sizeof(uint32_t); i++)
              {
              *tmp++ = *src++;
              }
#else
            uint32_t *tmp = (uint32_t *)&ret_val;
            tmp[0] = LL;
            tmp[1] = HH;
#endif // defined(__GNUC__)

            return ret_val;
            }

          R8 &operator=(const double num)
            {
#if defined(__GNUC__)
            ///
            /// UNDONE: FIXME: This code fixes an optimization problem with the gcc compiler.
            /// When the optimization level is greater than zero, the gcc compiler
            /// code will not work with the unsigned int* conversion, it requires you
            /// to copy byte by byte.
            ///
            uint8_t *src = (uint8_t *)&num;
            uint8_t *dst = (uint8_t *)&LL;
            uint32_t i;

            for (i = 0; i < sizeof(uint32_t); i++)
              {
              *dst++ = *src++;
              }

            dst = (uint8_t *)&HH;
            for (i = 0; i < sizeof(uint32_t); i++)
              {
              *dst++ = *src++;
              }
#else
            uint32_t *tmp = (uint32_t *)&num;
            LL = (uint32_t)tmp[0];
            HH = (uint32_t)tmp[1];
#endif

            return *this;
            }

          R8 &operator+=(const R8 &num)
            {
            double value = (double)*this; // uses conversion
            value += (double)num;         // uses conversion and then built-in type double
            *this = value;                // uses assignment operator (operator=)
            return *this;
            }

          R8 &operator-=(const R8 &num)
            {
            double value = (double)*this; // uses conversion
            value -= (double)num;         // uses conversion and then built-in type double
            *this = value;                // uses assignment operator (operator=)
            return *this;
            }

          R8 operator*(const R8 &num)
            {
            double value = (double)*this; // uses conversion
            R8 ret_value;
            value *= (double)num; // uses conversion and then built-in type __int64
            ret_value = value;    // uses assignment operator (operator=)
            return ret_value;
            }

          R8 operator/(const R8 &num)
            {
            double value = (double)*this; // uses conversion
            R8 ret_value;
            value /= (double)num; // uses conversion and then built-in type __int64
            ret_value = value;    // uses assignment operator (operator=)
            return ret_value;
            }

          bool operator<(const R8 &num)
            {
            double value = (double)*this; // uses conversion
            return (value < (double)num);
            }

          bool operator>(const R8 &num)
            {
            double value = (double)*this; // uses conversion
            return (value > (double)num);
            }

          bool operator==(const R8 &num)
            {
            double value = (double)*this; // uses conversion
            return (value == (double)num);
            }

          bool operator==(const double num)
            {
            double value = (double)*this; // uses conversion
            return (value == num);
            }

          } r8;

        } numeric;

      // The macro CT_ASSERT is used to validate that members of Numeric union start at zero offset in union.
      // This presumption is used in CRL_RT_Interop code.
      // This macro does not add any code or data member, it is pure compiler time validation.
      // This is not a valid check for Big Endian.
      // Addr:  0011223344556677  Number: 0xAABBCCDD
      // MemBE: AABBCCDD
      // MemLE: DDCCBBAA
      // So for LE DD,Addr 00 is the low byte.
      // For BE it is certainly not true, hence this check will not work. Will Interop? FIXME GJS - verify interop
      CT_ASSERT_UNIQUE_NAME(offsetof(Numeric, s1) == 0, s1)
        CT_ASSERT_UNIQUE_NAME(offsetof(Numeric, s2) == 0, s2)
        CT_ASSERT_UNIQUE_NAME(offsetof(Numeric, s4) == 0, s4)
        CT_ASSERT_UNIQUE_NAME(offsetof(Numeric, u1) == 0, u1)
        CT_ASSERT_UNIQUE_NAME(offsetof(Numeric, u2) == 0, u2)
        CT_ASSERT_UNIQUE_NAME(offsetof(Numeric, u4) == 0, u4)
        CT_ASSERT_UNIQUE_NAME(offsetof(Numeric, r4) == 0, r4)
        CT_ASSERT_UNIQUE_NAME(offsetof(Numeric, s8) == 0, s8)
        CT_ASSERT_UNIQUE_NAME(offsetof(Numeric, u8) == 0, u8)
        CT_ASSERT_UNIQUE_NAME(offsetof(Numeric, r8) == 0, r8)

        struct String
        {
        const char *m_text;
#if !defined(NANOCLR_NO_ASSEMBLY_STRINGS)
        CLR_RT_Assembly *m_assm;
#endif
        } string;

      //--//

      struct ObjectReference
        {
        CLR_RT_HeapBlock *ptr;
        } objectReference;

      struct ArrayReference
        {
        CLR_RT_HeapBlock_Array *array;
        uint32_t index;
        } arrayReference;

      struct ObjectHeader
        {
        uint32_t cls;
        CLR_RT_HeapBlock_Lock *lock;
        } objectHeader;

#if defined(NANOCLR_APPDOMAINS)
      struct TransparentProxy
        {
        CLR_RT_HeapBlock *ptr;       // points to a DATATYPE_CLASS derived from MBRO
        CLR_RT_AppDomain *appDomain; // EVENT HEAP -- NO RELOCATION
        } transparentProxy;
#endif

      //--//

      CLR_RT_ReflectionDef_Index reflection;

      //--//

      struct BinaryBlob
        {
        CLR_RT_MarkingHandler m_mark;
        CLR_RT_RelocationHandler m_relocate;
        } binaryBlob;

      } m_data;

  public:
    //--//

    void InitializeToZero();

    //--//

    CLR_DataType DataType() const
      {
      return (CLR_DataType)m_id.type.dataType;
      }
    uint8_t DataFlags() const
      {
      return m_id.type.flags;
      }
    uint16_t DataSize() const
      {
      return m_id.type.size;
      }

    // Returns number of bytes actually used in CLR_RT_HeapBlock_AtomicData
    uint32_t GetAtomicDataUsedBytes() const;

    void SetDataId(uint32_t id)
      {
      m_id.raw = id;
      }
    void ChangeDataType(uint32_t dataType)
      {
      m_id.type.dataType = dataType;
      }
    void ChangeDataFlags(uint32_t flags)
      {
      m_id.type.flags = flags;
      }

    void ClearData()
      {
      m_data.nodeLink.nextBlock = NULL;
      m_data.nodeLink.prevBlock = NULL;
      }

    void SetFlags(uint8_t flags)
      {
      m_id.type.flags |= flags;
      }
    void ResetFlags(uint8_t flags)
      {
      m_id.type.flags &= ~flags;
      }
    bool IsFlagSet(uint8_t flags) const
      {
      return (m_id.type.flags & flags) != 0;
      }

    //--//

#if defined(NANOCLR_FILL_MEMORY_WITH_DIRTY_PATTERN)

    void Debug_ClearBlock(int data);
    void Debug_CheckPointer() const;
    static void Debug_CheckPointer(void *ptr);

#else

    void Debug_ClearBlock(int data)
      {
      }
    void Debug_CheckPointer() const
      {
      }
    static void Debug_CheckPointer(void *ptr)
      {
      }

#endif

    //--//

    bool IsAlive() const
      {
      return IsFlagSet(HB_Alive);
      }
    bool IsEvent() const
      {
      return IsFlagSet(HB_Event);
      }
    bool IsForcedAlive() const
      {
      return IsFlagSet(HB_KeepAlive);
      }

    bool IsPinned() const
      {
      return IsFlagSet(HB_Pinned);
      }
    void Pin()
      {
      SetFlags(HB_Pinned);
      }
    void Unpin()
      {
      ResetFlags(HB_Pinned);
      }

    void MarkDead()
      {
      ResetFlags(HB_Alive);
      }
    void MarkAlive()
      {
      SetFlags(HB_Alive);
      }

    void MarkForcedAlive()
      {
      SetFlags(HB_KeepAlive);
      }
    void UnmarkForcedAlive()
      {
      ResetFlags(HB_KeepAlive);
      }

    bool IsBoxed() const
      {
      return IsFlagSet(HB_Boxed);
      }
    void Box()
      {
      SetFlags(HB_Boxed);
      }
    void Unbox()
      {
      ResetFlags(HB_Boxed);
      }

    //--//

    void SetBoolean(bool val)
      {
      CLR_RT_HEAPBLOCK_ASSIGN_INTEGER32_SIGNED(DATATYPE_BOOLEAN, val ? 1 : 0);
      }
    void SetInteger(const int32_t num, uint8_t dataType)
      {
      CLR_RT_HEAPBLOCK_ASSIGN_INTEGER32_SIGNED(dataType, num);
      }

    void SetInteger(const int8_t num)
      {
      CLR_RT_HEAPBLOCK_ASSIGN_INTEGER32_SIGNED(DATATYPE_I1, num);
      }
    void SetInteger(const int16_t num)
      {
      CLR_RT_HEAPBLOCK_ASSIGN_INTEGER32_SIGNED(DATATYPE_I2, num);
      }
    void SetInteger(const int32_t num)
      {
      CLR_RT_HEAPBLOCK_ASSIGN_INTEGER32_SIGNED(DATATYPE_I4, num);
      }
    void SetInteger(const int64_t &num)
      {
      CLR_RT_HEAPBLOCK_ASSIGN_INTEGER64_SIGNED(DATATYPE_I8, num);
      }
    void SetInteger(const uint8_t num)
      {
      CLR_RT_HEAPBLOCK_ASSIGN_INTEGER32_UNSIGNED(DATATYPE_U1, num);
      }
    void SetInteger(const uint16_t num)
      {
      CLR_RT_HEAPBLOCK_ASSIGN_INTEGER32_UNSIGNED(DATATYPE_U2, num);
      }
    void SetInteger(const uint32_t num)
      {
      CLR_RT_HEAPBLOCK_ASSIGN_INTEGER32_UNSIGNED(DATATYPE_U4, num);
      }
    void SetInteger(const uint64_t &num)
      {
      CLR_RT_HEAPBLOCK_ASSIGN_INTEGER64_UNSIGNED(DATATYPE_U8, num);
      }

    void SetFloat(const float num)
      {
      CLR_RT_HEAPBLOCK_ASSIGN_FLOAT32(DATATYPE_R4, num);
      }
    void SetDouble(const double num)
      {
      CLR_RT_HEAPBLOCK_ASSIGN_FLOAT64(DATATYPE_R8, num);
      }

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

    void SetFloatFromBits(const uint32_t num)
      {
      SetFloat(*(const float *)&num);
      }

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

    void SetDoubleFromBits(const uint64_t &num)
      {
      SetDouble(*(const double *)&num);
      }

    void SetObjectReference(const CLR_RT_HeapBlock *ptr)
      {
      m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_OBJECT, 0, 1);
      m_data.objectReference.ptr = (CLR_RT_HeapBlock *)ptr;
      }

    void SetReference(CLR_RT_HeapBlock &dst)
      {
      CLR_RT_HeapBlock *obj;

      //
      // ValueTypes are implemented as pointers to objects,
      // so getting a reference to a ValueType has to be treated like getting a reference to object, not to its
      // holder!
      //
      if (dst.IsAValueType())
        {
        obj = dst.Dereference();
        }
      else
        {
        obj = &dst;
        }

      m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_BYREF, 0, 1);
      m_data.objectReference.ptr = obj;
      }

    bool IsAReferenceOfThisType(CLR_DataType dataType) const
      {
      if (DataType() == DATATYPE_OBJECT)
        {
        CLR_RT_HeapBlock *obj = Dereference();

        if (obj && obj->DataType() == dataType)
          return true;
        }

      return false;
      }

    bool IsAValueType() const
      {
      if (DataType() == DATATYPE_OBJECT)
        {
        CLR_RT_HeapBlock *obj = Dereference();

        if (obj && obj->DataType() == DATATYPE_VALUETYPE && obj->IsBoxed() == false)
          return true;
        }

      return false;
      }

    bool SameHeader(const CLR_RT_HeapBlock &right) const
      {
      return this->m_data.numeric.u8 == right.m_data.numeric.u8;
      }

    //--//

    CLR_RT_HeapBlock_Array *RecoverArrayHeader() const
      {
      return (DataType() == DATATYPE_ARRAY_BYREF) ? m_data.arrayReference.array : NULL;
      }

    //--//

    const char *RecoverString() const
      {
      if (DataType() == DATATYPE_OBJECT)
        {
        const CLR_RT_HeapBlock *ptr = m_data.objectReference.ptr;

        if (ptr)
          {
          return ptr->StringText();
          }
        }

      return NULL;
      }

    //--//
    const CLR_RT_HeapBlock_AtomicData &DataByRefConst() const
      {
      return m_data;
      }

    CLR_RT_HeapBlock_AtomicData::Numeric &NumericByRef()
      {
      return m_data.numeric;
      }
    const CLR_RT_HeapBlock_AtomicData::Numeric &NumericByRefConst() const
      {
      return m_data.numeric;
      }

    //--//

    const char *StringText() const
      {
      return m_data.string.m_text;
      }

    void SetStringText(const char *szText)
      {
      m_data.string.m_text = szText;
      }

    //--//

    uint32_t ObjectCls() const
      {
      return m_data.objectHeader.cls;
      }
    CLR_RT_HeapBlock_Lock *ObjectLock() const
      {
      return m_data.objectHeader.lock;
      }
    void SetObjectLock(CLR_RT_HeapBlock_Lock *lock)
      {
      m_data.objectHeader.lock = lock;
      }

    HRESULT SetObjectCls(uint32_t cls);

    //--//

    const CLR_RT_ReflectionDef_Index &ReflectionDataConst() const
      {
      return m_data.reflection;
      }
    CLR_RT_ReflectionDef_Index &ReflectionData()
      {
      return m_data.reflection;
      }

    //--//

    CLR_RT_HeapBlock_Array *Array() const
      {
      return m_data.arrayReference.array;
      }
    uint32_t ArrayIndex() const
      {
      return m_data.arrayReference.index;
      }
    void ArrayIndexIncrement()
      {
      m_data.arrayReference.index++;
      }

    CLR_RT_MarkingHandler BinaryBlobMarkingHandler() const
      {
      return m_data.binaryBlob.m_mark;
      }
    CLR_RT_RelocationHandler BinaryBlobRelocationHandler() const
      {
      return m_data.binaryBlob.m_relocate;
      }

    void SetBinaryBlobHandlers(CLR_RT_MarkingHandler mark, CLR_RT_RelocationHandler relocate)
      {
      m_data.binaryBlob.m_mark = mark;
      m_data.binaryBlob.m_relocate = relocate;
      }

    //--//
    //--//

    CLR_RT_HeapBlock *Dereference() const
      {
      return m_data.objectReference.ptr;
      }

    CLR_RT_HeapBlock_WeakReference *DereferenceWeakReference() const
      {
      return (CLR_RT_HeapBlock_WeakReference *)Dereference();
      }
    CLR_RT_HeapBlock_String *DereferenceString() const
      {
      return (CLR_RT_HeapBlock_String *)Dereference();
      }
    CLR_RT_HeapBlock_Array *DereferenceArray() const
      {
      return (CLR_RT_HeapBlock_Array *)Dereference();
      }
    CLR_RT_HeapBlock_Delegate *DereferenceDelegate() const
      {
      return (CLR_RT_HeapBlock_Delegate *)Dereference();
      }
    CLR_RT_HeapBlock_Delegate_List *DereferenceDelegateList() const
      {
      return (CLR_RT_HeapBlock_Delegate_List *)Dereference();
      }
    CLR_RT_HeapBlock_BinaryBlob *DereferenceBinaryBlob() const
      {
      return (CLR_RT_HeapBlock_BinaryBlob *)Dereference();
      }

    //--//

    void AssignId(const CLR_RT_HeapBlock &value)
      {
      _ASSERTE(value.DataSize() == 1);

      m_id = value.m_id;
      }

    void AssignData(const CLR_RT_HeapBlock &value)
      {
      _ASSERTE(value.DataSize() == 1);

      m_data = value.m_data;
      }

    void Assign(const CLR_RT_HeapBlock &value)
      {
      _ASSERTE(value.DataSize() == 1);

      value.Debug_CheckPointer();

      CLR_RT_HeapBlock_Raw *src = (CLR_RT_HeapBlock_Raw *)this;
      CLR_RT_HeapBlock_Raw *dst = (CLR_RT_HeapBlock_Raw *)&value;

      *src = *dst;
      }

    void AssignAndPreserveType(const CLR_RT_HeapBlock &value)
      {
      _ASSERTE(value.DataSize() == 1);

      this->m_data = value.m_data;

      if (this->DataType() > DATATYPE_LAST_PRIMITIVE_TO_PRESERVE)
        this->m_id = value.m_id;
      }

    void AssignPreserveTypeCheckPinned(const CLR_RT_HeapBlock &value)
      {
      _ASSERTE(value.DataSize() == 1);

      { // If local variable does not have pinned type - move source data into it
      if (!IsPinned())
        {
        AssignAndPreserveType(value);
        }
      else // IsPinned() is true;
        {
        AssignAndPinReferencedObject(value);
        }
      }
      }

    // This function is called for assigning to "pinned" reference variables.
    // Since it is rare case, the code is not inlined to save code size.
    void AssignAndPinReferencedObject(const CLR_RT_HeapBlock &value);

    HRESULT Convert(CLR_DataType et, bool fOverflow, bool fUnsigned)
      {
      //
      // For V1, we don't throw on overflow.
      //
      return Convert_Internal(et);
      }

    bool InitObject();

    //--//

    static int32_t Compare_Unsigned_Values(const CLR_RT_HeapBlock &left, const CLR_RT_HeapBlock &right)
      {
      return Compare_Values(left, right, false);
      }
    static int32_t Compare_Signed_Values(const CLR_RT_HeapBlock &left, const CLR_RT_HeapBlock &right)
      {
      return Compare_Values(left, right, true);
      }

    //--//

    HRESULT SetReflection(const CLR_RT_ReflectionDef_Index &reflex);
    HRESULT SetReflectionAssembly(uint32_t assm);
    HRESULT SetReflection(uint32_t sig);
    HRESULT SetReflectionClass(uint32_t cls);
    HRESULT SetReflectionField(uint32_t fd);
    HRESULT SetReflectionMethod(uint32_t md);

    HRESULT InitializeArrayReference(CLR_RT_HeapBlock &ref, int index);
    void InitializeArrayReferenceDirect(CLR_RT_HeapBlock_Array &array, int index);
    void FixArrayReferenceForValueTypes();

    HRESULT LoadFromReference(CLR_RT_HeapBlock &ref);
    HRESULT StoreToReference(CLR_RT_HeapBlock &ref, int size);
    HRESULT Reassign(const CLR_RT_HeapBlock &value);
    HRESULT PerformBoxingIfNeeded();
    HRESULT PerformBoxing(const CLR_RT_TypeDef_Instance &cls);
    HRESULT PerformUnboxing(const CLR_RT_TypeDef_Instance &cls);
    CLR_RT_HeapBlock *FixBoxingReference();
    HRESULT EnsureObjectReference(CLR_RT_HeapBlock *&obj);

    //--//

    bool IsZero() const;
    void Promote();

    static uint32_t GetHashCode(CLR_RT_HeapBlock *ptr, bool fRecurse, uint32_t crc);
    static bool ObjectsEqual(const CLR_RT_HeapBlock &left, const CLR_RT_HeapBlock &right, bool fSameReference);

    static int32_t Compare_Values(const CLR_RT_HeapBlock &left, const CLR_RT_HeapBlock &right, bool fSigned);

    HRESULT Convert_Internal(CLR_DataType et);
    HRESULT NumericAdd(const CLR_RT_HeapBlock &right);
    HRESULT NumericSub(const CLR_RT_HeapBlock &right);
    HRESULT NumericMul(const CLR_RT_HeapBlock &right);
    HRESULT NumericDiv(const CLR_RT_HeapBlock &right);
    HRESULT NumericDivUn(const CLR_RT_HeapBlock &right);
    HRESULT NumericRem(const CLR_RT_HeapBlock &right);
    HRESULT NumericRemUn(const CLR_RT_HeapBlock &right);
    HRESULT NumericShl(const CLR_RT_HeapBlock &right);
    HRESULT NumericShr(const CLR_RT_HeapBlock &right);
    HRESULT NumericShrUn(const CLR_RT_HeapBlock &right);
    HRESULT NumericNeg();

    CLR_RT_HeapBlock *ExtractValueBlock(int offset);
    void ReadValue(int64_t &val, int offset);
    void WriteValue(const int64_t &val, int offset);

    void Relocate__HeapBlock();
    void Relocate_String();
    void Relocate_Obj();
    void Relocate_Cls();
    void Relocate_Ref();
    void Relocate_ArrayRef();

#if defined(NANOCLR_APPDOMAINS)
    void Relocate_TransparentProxy();
#endif

  private:
    CLR_RT_HeapBlock &operator=(const CLR_RT_HeapBlock &);
  };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

//--//

#define NANOCLR_FOREACH_NODE(cls, ptr, lst)                                                                            \
    {                                                                                                                  \
        cls *ptr;                                                                                                      \
        cls *ptr##Next;                                                                                                \
                                                                                                                       \
        for (ptr = (cls *)(lst).FirstNode(); (ptr##Next = (cls *)ptr->Next()) != NULL; ptr = ptr##Next)                \
        {                                                                                                              \
            NANOCLR_FAULT_ON_EARLY_COLLECTION(ptr##Next);

#define NANOCLR_FOREACH_NODE__NODECL(cls, ptr, lst)                                                                    \
    {                                                                                                                  \
        cls *ptr##Next;                                                                                                \
                                                                                                                       \
        for (ptr = (cls *)(lst).FirstNode(); (ptr##Next = (cls *)ptr->Next()) != NULL; ptr = ptr##Next)                \
        {                                                                                                              \
            NANOCLR_FAULT_ON_EARLY_COLLECTION(ptr##Next);

#define NANOCLR_FOREACH_NODE__DIRECT(cls, ptr, startNode)                                                              \
    {                                                                                                                  \
        cls *ptr;                                                                                                      \
        cls *ptr##Next;                                                                                                \
                                                                                                                       \
        for (ptr = (cls *)(startNode); (ptr##Next = (cls *)ptr->Next()) != NULL; ptr = ptr##Next)                      \
        {                                                                                                              \
            NANOCLR_FAULT_ON_EARLY_COLLECTION(ptr##Next);

#define NANOCLR_FOREACH_NODE_PREPARE_RELOAD(ptr) NANOCLR_CANCEL_EARLY_COLLECTION(ptr##Next)

#define NANOCLR_FOREACH_NODE_RELOAD(cls, ptr) ptr##Next = (cls *)ptr->Next()

#define NANOCLR_FOREACH_NODE_RESTART(cls, ptr, lst)                                                                    \
    ptr##Next = (cls *)(lst).FirstNode();                                                                              \
    continue

#define NANOCLR_FOREACH_NODE_END()                                                                                     \
    }                                                                                                                  \
    }

#define NANOCLR_FOREACH_NODE_BACKWARD(cls, ptr, lst)                                                                   \
    {                                                                                                                  \
        cls *ptr;                                                                                                      \
        cls *ptr##Prev;                                                                                                \
                                                                                                                       \
        for (ptr = (cls *)(lst).LastNode(); (ptr##Prev = (cls *)ptr->Prev()) != NULL; ptr = ptr##Prev)                 \
        {                                                                                                              \
            NANOCLR_FAULT_ON_EARLY_COLLECTION(ptr##Prev);

#define NANOCLR_FOREACH_NODE_BACKWARD__NODECL(cls, ptr, lst)                                                           \
    {                                                                                                                  \
        cls *ptr##Prev;                                                                                                \
                                                                                                                       \
        for (ptr = (cls *)(lst).LastNode(); (ptr##Prev = (cls *)ptr->Prev()) != NULL; ptr = ptr##Prev)                 \
        {                                                                                                              \
            NANOCLR_FAULT_ON_EARLY_COLLECTION(ptr##Prev);

#define NANOCLR_FOREACH_NODE_BACKWARD__DIRECT(cls, ptr, startNode)                                                     \
    {                                                                                                                  \
        cls *ptr;                                                                                                      \
        cls *ptr##Prev;                                                                                                \
                                                                                                                       \
        for (ptr = (cls *)(startNode); (ptr##Prev = (cls *)ptr->Prev()) != NULL; ptr = ptr##Prev)                      \
        {                                                                                                              \
            NANOCLR_FAULT_ON_EARLY_COLLECTION(ptr##Prev);

#define NANOCLR_FOREACH_NODE_BACKWARD_RESTART(cls, ptr, lst)                                                           \
    ptr##Prev = (cls *)(lst).LastNode();                                                                               \
    continue

#define NANOCLR_FOREACH_NODE_BACKWARD_PREPARE_RELOAD(ptr) NANOCLR_CANCEL_EARLY_COLLECTION(ptr##Prev)

#define NANOCLR_FOREACH_NODE_BACKWARD_RELOAD(cls, ptr) ptr##Prev = (cls *)ptr->Prev()

#define NANOCLR_FOREACH_NODE_BACKWARD_END()                                                                            \
    }                                                                                                                  \
    }

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

struct CLR_RT_HeapBlock_Node : public CLR_RT_HeapBlock
  {
  friend struct CLR_RT_DblLinkedList;

  void GenericNode_Initialize()
    {
    ClearData();
    }

  CLR_RT_HeapBlock_Node *Next() const
    {
    return m_data.nodeLink.nextBlock;
    }
  CLR_RT_HeapBlock_Node *Prev() const
    {
    return m_data.nodeLink.prevBlock;
    }

  void SetNext(CLR_RT_HeapBlock_Node *next)
    {
    m_data.nodeLink.nextBlock = next;
    }
  void SetPrev(CLR_RT_HeapBlock_Node *prev)
    {
    m_data.nodeLink.prevBlock = prev;
    }

  //
  // The ARM compiler cannot inline these methods,
  // it keeps injecting a call to a 4 instruction-long function (arg!!).
  //
  // So we'll have to use the explicit check...
  //
  // bool IsValidForward () const { return Next() != NULL; }
  // bool IsValidBackward() const { return Prev() != NULL; }

  //--//

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_1_HeapBlocksAndUnlink
  void ConsistencyCheck_Nodes(CLR_RT_HeapBlock_Node *prev, CLR_RT_HeapBlock_Node *next)
    {
    if (prev && next)
      return;
    if (!prev && !next)
      return;

    trace_debug("Bad node!!\r\n");

    NANOCLR_DEBUG_STOP();
    }

#else

  void ConsistencyCheck_Nodes(CLR_RT_HeapBlock_Node *prev, CLR_RT_HeapBlock_Node *next)
    {
    }

#endif

  void RemoveFromList()
    {
    CLR_RT_HeapBlock_Node *prev = m_data.nodeLink.prevBlock;
    CLR_RT_HeapBlock_Node *next = m_data.nodeLink.nextBlock;

    ConsistencyCheck_Nodes(prev, next);

    if (prev)
      prev->m_data.nodeLink.nextBlock = next;
    if (next)
      next->m_data.nodeLink.prevBlock = prev;
    }

  void Unlink()
    {
    CLR_RT_HeapBlock_Node *prev = m_data.nodeLink.prevBlock;
    CLR_RT_HeapBlock_Node *next = m_data.nodeLink.nextBlock;

    ConsistencyCheck_Nodes(prev, next);

    if (prev)
      prev->m_data.nodeLink.nextBlock = next;
    if (next)
      next->m_data.nodeLink.prevBlock = prev;

    m_data.nodeLink.prevBlock = NULL;
    m_data.nodeLink.nextBlock = NULL;
    }

  //--//

  void Relocate();
  };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

struct CLR_RT_DblLinkedList
  {
  private:
    //
    // Logically, a list starts with a CLR_RT_HeapBlock_Node with only the Next() set and ends with a node with only
    // Prev() set. This can be collapsed to have the two nodes overlap.
    //
    CLR_RT_HeapBlock_Node *m_first; // ANY HEAP - DO RELOCATION -
    CLR_RT_HeapBlock_Node *m_null;
    CLR_RT_HeapBlock_Node *m_last; // ANY HEAP - DO RELOCATION -

    //--//

  public:
    void DblLinkedList_Initialize();
    void DblLinkedList_PushToCache();
    void DblLinkedList_Release();

    int NumOfNodes();

    void Relocate();

    //--//

    CLR_RT_HeapBlock_Node *FirstNode() const
      {
      return m_first;
      }
    CLR_RT_HeapBlock_Node *LastNode() const
      {
      return m_last;
      }
    bool IsEmpty() const
      {
      return m_first == Tail();
      }

    CLR_RT_HeapBlock_Node *FirstValidNode() const
      {
      CLR_RT_HeapBlock_Node *res = m_first;
      return res->Next() ? res : NULL;
      }
    CLR_RT_HeapBlock_Node *LastValidNode() const
      {
      CLR_RT_HeapBlock_Node *res = m_last;
      return res->Prev() ? res : NULL;
      }

    // Check that node pNode is not "dummy" tail or head node.
    static bool IsValidListNode(CLR_RT_HeapBlock_Node *pNode)
      {
      return pNode->m_data.nodeLink.nextBlock != NULL && pNode->m_data.nodeLink.prevBlock != NULL;
      }

    CLR_RT_HeapBlock_Node *Head() const
      {
      return (CLR_RT_HeapBlock_Node *)((size_t)&m_first - offsetof(CLR_RT_HeapBlock, m_data.nodeLink.nextBlock));
      }
    CLR_RT_HeapBlock_Node *Tail() const
      {
      return (CLR_RT_HeapBlock_Node *)((size_t)&m_last - offsetof(CLR_RT_HeapBlock, m_data.nodeLink.prevBlock));
      }

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_2_DblLinkedList
    void ValidateList();
#else
    void ValidateList() {};
#endif

    //--//

  private:
    void Insert(CLR_RT_HeapBlock_Node *prev, CLR_RT_HeapBlock_Node *next, CLR_RT_HeapBlock_Node *node)
      {
      node->m_data.nodeLink.nextBlock = next;
      node->m_data.nodeLink.prevBlock = prev;

      next->m_data.nodeLink.prevBlock = node;
      prev->m_data.nodeLink.nextBlock = node;
      }

  public:
    void InsertBeforeNode(CLR_RT_HeapBlock_Node *node, CLR_RT_HeapBlock_Node *nodeNew)
      {
      ValidateList();

      if (node && nodeNew && node != nodeNew)
        {
        nodeNew->RemoveFromList();

        Insert(node->Prev(), node, nodeNew);
        }
      }

    void InsertAfterNode(CLR_RT_HeapBlock_Node *node, CLR_RT_HeapBlock_Node *nodeNew)
      {
      ValidateList();

      if (node && nodeNew && node != nodeNew)
        {
        nodeNew->RemoveFromList();

        Insert(node, node->Next(), nodeNew);
        }
      }

    void LinkAtFront(CLR_RT_HeapBlock_Node *node)
      {
      InsertAfterNode(Head(), node);
      }

    void LinkAtBack(CLR_RT_HeapBlock_Node *node)
      {
      InsertBeforeNode(Tail(), node);
      }

    CLR_RT_HeapBlock_Node *ExtractFirstNode()
      {
      ValidateList();

      CLR_RT_HeapBlock_Node *node = FirstValidNode();

      if (node)
        node->Unlink();

      return node;
      }

    CLR_RT_HeapBlock_Node *ExtractLastNode()
      {
      ValidateList();

      CLR_RT_HeapBlock_Node *node = LastValidNode();

      if (node)
        node->Unlink();

      return node;
      }

    //--//

    PROHIBIT_COPY_CONSTRUCTORS(CLR_RT_DblLinkedList);
  };

struct CLR_RT_AVLTree
  {
  struct Owner;

  enum Skew
    {
    SKEW_LEFT = -1,
    SKEW_NONE = 0,
    SKEW_RIGHT = 1,
    };

  enum Result
    {
    RES_OK = 0,
    RES_BALANCE = 1,
    RES_NOTFOUND = 2,
    RES_DUPLICATE = 3,
    RES_ERROR = 4,
    };

  struct Entry : public CLR_RT_HeapBlock_Node
    {
    Entry *m_left;
    Entry *m_right;
    Skew m_skew;
    };

  typedef int (*ComparerFtn)(void *state, Entry *left, Entry *right);
  typedef Entry *(*NewNodeFtn)(void *state, Entry *payload);
  typedef void (*FreeNodeFtn)(void *state, Entry *node);
  typedef void (*ReassignFtn)(void *state, Entry *from, Entry *to);

  struct OwnerInfo
    {
    ComparerFtn m_ftn_compare;
    NewNodeFtn m_ftn_newNode;
    FreeNodeFtn m_ftn_freeNode;
    ReassignFtn m_ftn_reassignNode;
    void *m_state;
    };

  //--//

  Entry *m_root;
  OwnerInfo m_owner;

  //--//

  void Initialize();

  Result Insert(Entry *newDatum);
  Result Remove(Entry *oldDatum);
  Entry *Find(Entry *srcDatum);

  private:
    static void RotateLeft(Entry *&n);
    static void RotateRight(Entry *&n);

    static Result LeftGrown(Entry *&n);
    static Result RightGrown(Entry *&n);

    static Result LeftShrunk(Entry *&n);
    static Result RightShrunk(Entry *&n);

    bool FindHighest(Entry *&n, Entry *target, Result &res);
    bool FindLowest(Entry *&n, Entry *target, Result &res);

    Result Insert(Entry *&n, Entry *newDatum);
    Result Remove(Entry *&n, Entry *oldDatum);
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CLR_RT_HeapBlock_String : public CLR_RT_HeapBlock
  {
  static CLR_RT_HeapBlock_String *CreateInstance(CLR_RT_HeapBlock &reference, uint32_t length);

  static HRESULT CreateInstance(CLR_RT_HeapBlock &reference, const char *szText);
  static HRESULT CreateInstance(CLR_RT_HeapBlock &reference, const char *szText, uint32_t length);
  static HRESULT CreateInstance(CLR_RT_HeapBlock &reference, uint32_t token, CLR_RT_Assembly *assm);
  static HRESULT CreateInstance(CLR_RT_HeapBlock &reference, const char *szText, CLR_RT_Assembly *assm);
  static HRESULT CreateInstance(CLR_RT_HeapBlock &reference, uint16_t *szText, uint32_t length);

  static CLR_RT_HeapBlock_String *GetStringEmpty();
  };

struct CLR_RT_HeapBlock_Array : public CLR_RT_HeapBlock
  {
  uint32_t m_numOfElements;
  uint8_t m_typeOfElement;
  uint8_t m_sizeOfElement;
  uint8_t m_fReference;
  uint8_t m_pad;

  //--//

  static HRESULT CreateInstance(
    CLR_RT_HeapBlock &reference,
    uint32_t length,
    const CLR_RT_ReflectionDef_Index &reflex);
  static HRESULT CreateInstance(CLR_RT_HeapBlock &reference, uint32_t length, uint32_t cls);
  static HRESULT CreateInstance(CLR_RT_HeapBlock &reference, uint32_t length, CLR_RT_Assembly *assm, uint32_t tk);

  uint8_t *GetFirstElement()
    {
    return ((uint8_t *)&this[1]);
    }

  uint8_t *GetElement(uint32_t index)
    {
    return GetFirstElement() + m_sizeOfElement * index;
    }

  uint16_t *GetFirstElementUInt16()
    {
    return ((uint16_t *)&this[1]);
    }

  uint16_t *GetElementUInt16(uint32_t index)
    {
    return GetFirstElementUInt16() + m_sizeOfElement * index;
    }

  HRESULT ClearElements(int index, int length);

  //--//

  void Relocate();

  //--//

  static bool CheckRange(int index, int length, int numOfElements);

  static HRESULT IndexOf(
    CLR_RT_HeapBlock_Array *array,
    CLR_RT_HeapBlock &match,
    int start,
    int stop,
    bool fForward,
    int &index);
  static HRESULT Copy(
    CLR_RT_HeapBlock_Array *arraySrc,
    int indexSrc,
    CLR_RT_HeapBlock_Array *arrayDst,
    int indexDst,
    int length);
  };

struct CLR_RT_HeapBlock_Delegate : public CLR_RT_HeapBlock_Node // OBJECT HEAP - DO RELOCATION -
  {
  uint32_t m_cls;
  uint32_t m_ftn;
#if defined(NANOCLR_DELEGATE_PRESERVE_STACK)
  uint32_t m_numOfStackFrames;
#endif
  CLR_RT_HeapBlock m_object; // ANY HEAP - DO RELOCATION -

#if defined(NANOCLR_APPDOMAINS)
  CLR_RT_AppDomain *m_appDomain;
#endif

  //--//

  const uint32_t &DelegateFtn() const
    {
    return m_ftn;
    }
#if defined(NANOCLR_DELEGATE_PRESERVE_STACK)
  uint32_t DelegateNumOfStackFrames() const
    {
    return m_numOfStackFrames;
    }
  uint32_t *GetStackFrames()
    {
    return (uint32_t *)&this[1];
    }
#endif
  //--//

  static HRESULT CreateInstance(CLR_RT_HeapBlock &reference, uint32_t ftn, CLR_RT_StackFrame *call);

  void Relocate();
  };

struct CLR_RT_HeapBlock_Delegate_List : public CLR_RT_HeapBlock_Node // OBJECT HEAP - DO RELOCATION -
  {
  static const uint32_t c_Weak = 0x00000001;

  uint32_t m_cls;
  uint32_t m_length;
  uint32_t m_flags;

  CLR_RT_HeapBlock *GetDelegates()
    {
    return &this[1];
    } // ANY HEAP - DO RELOCATION -

    //--//

  static HRESULT CreateInstance(CLR_RT_HeapBlock_Delegate_List *&list, uint32_t length);

  static HRESULT Combine(
    CLR_RT_HeapBlock &reference,
    CLR_RT_HeapBlock &delegateSrc,
    CLR_RT_HeapBlock &delegateNew,
    bool fWeak);
  static HRESULT Remove(CLR_RT_HeapBlock &reference, CLR_RT_HeapBlock &delegateSrc, CLR_RT_HeapBlock &delegateOld);

  void Relocate();

  private:
    static HRESULT Change(
      CLR_RT_HeapBlock &reference,
      CLR_RT_HeapBlock &delegateSrc,
      CLR_RT_HeapBlock &delegateTarget,
      bool fCombine,
      bool fWeak);

    CLR_RT_HeapBlock *CopyAndCompress(CLR_RT_HeapBlock *src, CLR_RT_HeapBlock *dst, uint32_t num);
  };

//--//

struct CLR_RT_HeapBlock_BinaryBlob : public CLR_RT_HeapBlock
  {
  friend struct CLR_RT_Memory;

  //--//

  CLR_RT_Assembly *m_assembly;

  void *GetData()
    {
    return (void *)&this[1];
    }
  static CLR_RT_HeapBlock_BinaryBlob *GetBlob(void *data)
    {
    return (CLR_RT_HeapBlock_BinaryBlob *)data - 1;
    }

  //--//

  static HRESULT CreateInstance(
    CLR_RT_HeapBlock &reference,
    uint32_t length,
    CLR_RT_MarkingHandler mark,
    CLR_RT_RelocationHandler relocate,
    uint32_t flags);

  void Release(bool fEvent);

  void Relocate();

  private:
    static CLR_RT_HeapBlock_BinaryBlob *Allocate(uint32_t length, uint32_t flags);
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CLR_RT_ObjectToEvent_Destination : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  CLR_RT_DblLinkedList m_references; // EVENT HEAP - NO RELOCATION - list of CLR_RT_ObjectToEvent_Source

  //--//

  void Initialize();
  void CheckAll();
  void SignalAll();
  void DetachAll();
  bool IsReadyForRelease();
  bool ReleaseWhenDead();
  };

struct CLR_RT_ObjectToEvent_Source : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  CLR_RT_ObjectToEvent_Destination *m_eventPtr; // EVENT HEAP  - NO RELOCATION - Pointer to the event referenced.

  CLR_RT_HeapBlock *m_objectPtr; // OBJECT HEAP - DO RELOCATION - Pointer to the object to keep alive.
  CLR_RT_HeapBlock
    *m_referencePtr; // OBJECT HEAP - DO RELOCATION - Pointer to the HeapBlock referencing this structure.

//--//

  static HRESULT CreateInstance(
    CLR_RT_ObjectToEvent_Destination *event,
    CLR_RT_HeapBlock &object,
    CLR_RT_HeapBlock &reference);
  static CLR_RT_ObjectToEvent_Source *ExtractInstance(CLR_RT_HeapBlock &reference);

  void EnsureObjectIsAlive();
  void Detach();

  void Relocate();
  };

////////////////////////////////////////

struct CLR_RT_HeapBlock_Button : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  int m_key;
  bool m_fPressed;
  };

//--//

struct CLR_RT_HeapBlock_Lock : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  struct Owner : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
    {
    CLR_RT_SubThread *m_owningSubThread; // EVENT HEAP - NO RELOCATION -
    uint32_t m_recursion;
    };

  CLR_RT_Thread *m_owningThread; // EVENT HEAP - NO RELOCATION -

  CLR_RT_HeapBlock m_resource; // OBJECT HEAP - DO RELOCATION -

#if defined(NANOCLR_APPDOMAINS)
  CLR_RT_AppDomain *m_appDomain; // EVENT HEAP - NO RELOCATION -
#endif

  CLR_RT_DblLinkedList m_owners;   // EVENT HEAP - NO RELOCATION - list of CLR_RT_HeapBlock_Lock::Owner
  CLR_RT_DblLinkedList m_requests; // EVENT HEAP - NO RELOCATION - list of CLR_RT_HeapBlock_LockRequest

  //--//

  static HRESULT CreateInstance(CLR_RT_HeapBlock_Lock *&lock, CLR_RT_Thread *th, CLR_RT_HeapBlock &resource);
  static HRESULT IncrementOwnership(
    CLR_RT_HeapBlock_Lock *lock,
    CLR_RT_SubThread *sth,
    const int64_t &timeExpire,
    bool fForce);
  static HRESULT DecrementOwnership(CLR_RT_HeapBlock_Lock *lock, CLR_RT_SubThread *sth);

  void DestroyOwner(CLR_RT_SubThread *sth);
  void ChangeOwner();

  void Relocate();
  void Relocate_Owner();
  };

struct CLR_RT_HeapBlock_LockRequest : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  CLR_RT_SubThread *m_subthreadWaiting; // EVENT HEAP - NO RELOCATION -
  int64_t m_timeExpire;
  bool m_fForce;

  //--//

  static HRESULT CreateInstance(
    CLR_RT_HeapBlock_Lock *lock,
    CLR_RT_SubThread *sth,
    const int64_t &timeExpire,
    bool fForce);
  };

struct CLR_RT_HeapBlock_Timer : public CLR_RT_ObjectToEvent_Destination // EVENT HEAP - NO RELOCATION -
  {
  static const uint32_t c_NoFixedChange = 0x00000000;
  static const uint32_t c_SecondChange = 0x00000001;
  static const uint32_t c_MinuteChange = 0x00000002;
  static const uint32_t c_HourChange = 0x00000003;
  static const uint32_t c_DayChange = 0x00000004;
  static const uint32_t c_TimeZoneChange = 0x00000005;
  static const uint32_t c_SetTimeChange = 0x00000006;
  static const uint32_t c_AnyChange = 0x0000000F;

  static const uint32_t c_Triggered = 0x00000010;
  static const uint32_t c_Executing = 0x00000020;

  static const uint32_t c_ACTION_Create = 0x00010000;
  static const uint32_t c_ACTION_Change = 0x00020000;
  static const uint32_t c_ACTION_Destroy = 0x00040000;
  static const uint32_t c_UNUSED_00080000 = 0x00080000;

  static const uint32_t c_INPUT_Int32 = 0x00100000;
  static const uint32_t c_INPUT_TimeSpan = 0x00200000;

  static const uint32_t c_UNUSED_10000000 = 0x10000000;
  static const uint32_t c_Recurring = 0x40000000;
  static const uint32_t c_EnabledTimer = 0x80000000;

  //--//

  static const int32_t c_TickPerMillisecond = 10000;
  //--//

  uint32_t m_flags;
  int64_t m_timeExpire;
  int64_t m_timeFrequency;
  int64_t m_timeLastExpiration;
  int64_t m_ticksLastExpiration;

  //--//

  static HRESULT CreateInstance(uint32_t flags, CLR_RT_HeapBlock &owner, CLR_RT_HeapBlock &tmRef);
  static HRESULT ExtractInstance(CLR_RT_HeapBlock &ref, CLR_RT_HeapBlock_Timer *&timer);
  static HRESULT ConfigureObject(CLR_RT_StackFrame &stack, uint32_t flags);
  static bool CheckDisposed(CLR_RT_StackFrame &stack);

  void AdjustNextFixedExpire(const SYSTEMTIME &systemTime, bool fNext);

  void Trigger();
  void Reschedule();
  void RecoverFromGC();
  void SpawnTimer(CLR_RT_Thread *thread);

  static void ThreadTerminationCallback(void *param);
  };

struct CLR_RT_HeapBlock_EndPoint : public CLR_RT_ObjectToEvent_Destination // EVENT HEAP - NO RELOCATION -
  {
  struct Port
    {
    uint32_t m_type;
    uint32_t m_id;

    bool Compare(const Port &port);
    };

  struct Address
    {
    uint32_t m_seq;
    Port m_from;
    Port m_to;
    };

  struct Message : public CLR_RT_HeapBlock_Node
    {
    uint32_t m_cmd;
    Address m_addr;

    uint32_t m_found;
    uint32_t m_length;

    uint8_t m_data[1];
    };

  Port m_addr;
  CLR_RT_DblLinkedList m_messages;
  uint32_t m_seq;

  //--//

  static CLR_RT_DblLinkedList m_endPoints;

  static void HandlerMethod_Initialize();
  static void HandlerMethod_RecoverFromGC();
  static void HandlerMethod_CleanUp();

  static CLR_RT_HeapBlock_EndPoint *FindEndPoint(const CLR_RT_HeapBlock_EndPoint::Port &port);

  static HRESULT CreateInstance(
    const CLR_RT_HeapBlock_EndPoint::Port &port,
    CLR_RT_HeapBlock &owner,
    CLR_RT_HeapBlock &epRef);
  static HRESULT ExtractInstance(CLR_RT_HeapBlock &ref, CLR_RT_HeapBlock_EndPoint *&endPoint);

  bool ReleaseWhenDeadEx();
  void RecoverFromGC();

  Message *FindMessage(uint32_t cmd, const uint32_t *seq);
  };

//--//

struct CLR_RT_HeapBlock_WaitForObject : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  int64_t m_timeExpire;
  uint32_t m_cObjects;
  bool m_fWaitAll;

  CLR_RT_HeapBlock *GetWaitForObjects()
    {
    return &this[1];
    } // EVENT HEAP - DO RELOCATION -

    //--//

  static HRESULT WaitForSignal(CLR_RT_StackFrame &stack, const int64_t &timeExpire, CLR_RT_HeapBlock &object);
  static HRESULT WaitForSignal(
    CLR_RT_StackFrame &stack,
    const int64_t &timeExpire,
    CLR_RT_HeapBlock *objects,
    uint32_t cObjects,
    bool fWaitAll);
  static void SignalObject(CLR_RT_HeapBlock &object);

  void Relocate();

  private:
    static bool TryWaitForSignal(CLR_RT_Thread *caller, CLR_RT_HeapBlock *objects, uint32_t cObjects, bool fWaitAll);
    static void TryWaitForSignal(CLR_RT_Thread *caller);
    static HRESULT CreateInstance(
      CLR_RT_Thread *caller,
      const int64_t &timeExpire,
      CLR_RT_HeapBlock *objects,
      uint32_t cObjects,
      bool fWaitAll);
  };

//--//

struct CLR_RT_HeapBlock_Finalizer : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  CLR_RT_HeapBlock *m_object; // OBJECT HEAP - DO RELOCATION -
  uint32_t m_md;

#if defined(NANOCLR_APPDOMAINS)
  CLR_RT_AppDomain *m_appDomain;
#endif

  //--//

  static HRESULT CreateInstance(CLR_RT_HeapBlock *object, const CLR_RT_TypeDef_Instance &inst);

  void Relocate();

  static void SuppressFinalize(CLR_RT_HeapBlock *object);

  private:
    static void RemoveInstance(CLR_RT_HeapBlock *object, CLR_RT_DblLinkedList &lst);
  };

//--//

struct CLR_RT_HeapBlock_MemoryStream : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  struct Buffer : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
    {
    static const int c_NumOfBlocks = 32;
    static const int c_PayloadSize = c_NumOfBlocks * sizeof(CLR_RT_HeapBlock) - sizeof(uint8_t *) - sizeof(int);

    uint8_t *m_data;
    int m_length;
    uint8_t m_payload[c_PayloadSize];
    };

  //--//

  CLR_RT_DblLinkedList m_buffers; // EVENT HEAP - NO RELOCATION - list of CLR_RT_HeapBlock_MemoryStream::Buffer

  Buffer *m_current;
  int m_pos;
  uint32_t m_avail;

  //--//

  static HRESULT CreateInstance(CLR_RT_HeapBlock_MemoryStream *&stream, uint8_t *buf, int len);
  static void DeleteInstance(CLR_RT_HeapBlock_MemoryStream *&stream);

  void Release();

  void Rewind();
  void Reset();

  HRESULT ToArray(CLR_RT_HeapBlock &ref);

  int BitsAvailable();
  int BitsWritten();

  HRESULT ReadBits(uint32_t &val, uint32_t bits);
  HRESULT WriteBits(uint32_t val, uint32_t bits);

  HRESULT ReadBits(uint64_t &val, uint32_t bits);
  HRESULT WriteBits(uint64_t val, uint32_t bits);

  HRESULT ReadArray(uint8_t *buf, uint32_t bytes);
  HRESULT WriteArray(const uint8_t *buf, uint32_t bytes);

  private:
    HRESULT Initialize(uint8_t *buf, uint32_t len);

    Buffer *NewBuffer();
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CLR_RT_HeapBlock_WeakReference_Identity
  {
  uint32_t m_crc;
  uint32_t m_flags;
  uint32_t m_length;

  uint32_t m_selectorHash;
  uint32_t m_id;
  int32_t m_priority;

  uint32_t ComputeCRC(const uint8_t *ptr, uint32_t len) const;
  };

struct CLR_RT_HeapBlock_WeakReference : public CLR_RT_HeapBlock_Node // OBJECT HEAP - DO RELOCATION -
  {
  static const uint32_t WR_SurviveBoot = 0x00000001;
  static const uint32_t WR_SurvivePowerdown = 0x00000002;
  static const uint32_t WR_ArrayOfBytes = 0x00000004;
  static const uint32_t WR_Unused_00000008 = 0x00000008;
  static const uint32_t WR_Unused_00000010 = 0x00000010;
  static const uint32_t WR_Unused_00000020 = 0x00000020;
  static const uint32_t WR_Unused_00000040 = 0x00000040;
  static const uint32_t WR_Unused_00000080 = 0x00000080;
  static const uint32_t WR_Unused_00000100 = 0x00000100;
  static const uint32_t WR_Unused_00000200 = 0x00000200;
  static const uint32_t WR_Unused_00000400 = 0x00000400;
  static const uint32_t WR_Unused_00000800 = 0x00000800;
  static const uint32_t WR_Unused_00001000 = 0x00001000;
  static const uint32_t WR_Unused_00002000 = 0x00002000;
  static const uint32_t WR_Unused_00004000 = 0x00004000;
  static const uint32_t WR_Unused_00008000 = 0x00008000;
  static const uint32_t WR_Unused_00010000 = 0x00010000;
  static const uint32_t WR_Unused_00020000 = 0x00020000;
  static const uint32_t WR_Unused_00040000 = 0x00040000;
  static const uint32_t WR_Unused_00080000 = 0x00080000;
  static const uint32_t WR_Unused_00100000 = 0x00100000;
  static const uint32_t WR_Unused_00200000 = 0x00200000;
  static const uint32_t WR_Unused_00400000 = 0x00400000;
  static const uint32_t WR_Unused_00800000 = 0x00800000;
  static const uint32_t WR_Unused_01000000 = 0x01000000;
  static const uint32_t WR_Unused_02000000 = 0x02000000;
  static const uint32_t WR_Unused_04000000 = 0x04000000;
  static const uint32_t WR_Unused_08000000 = 0x08000000;
  static const uint32_t WR_Unused_10000000 = 0x10000000;
  static const uint32_t WR_Persisted = 0x20000000;
  static const uint32_t WR_Restored = 0x40000000;

  //--//

  CLR_RT_HeapBlock_WeakReference_Identity m_identity;

  CLR_RT_HeapBlock *m_targetDirect;           // OBJECT HEAP - DO RELOCATION -
  CLR_RT_HeapBlock_Array *m_targetSerialized; // OBJECT HEAP - DO RELOCATION -
  CLR_RT_HeapBlock_Array *m_targetCopied;     // OBJECT HEAP - NO RELOCATION -

  //--//

  static HRESULT CreateInstance(CLR_RT_HeapBlock_WeakReference *&weakref);

  static void RecoverObjects(CLR_RT_DblLinkedList &lstHeap);
  static bool PrepareForRecovery(CLR_RT_HeapBlock_Node *ptr, CLR_RT_HeapBlock_Node *end, uint32_t blockSize);

  HRESULT SetTarget(CLR_RT_HeapBlock &targetReference);
  HRESULT GetTarget(CLR_RT_HeapBlock &targetReference);

  void InsertInPriorityOrder();

  void Relocate();
  };

//--//

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

struct CLR_RT_Persistence_Manager
  {
  static const uint32_t c_Erased = 0xFFFFFFFF;

  struct ObjectHeader
    {
    static const uint32_t c_Version = 0x504F5631; // POV1 = Persisted Object V1

    static const uint32_t c_InUseBlock = 0xFFFFFFFF;
    static const uint32_t c_DeletedBlock = 0x00000000;

    uint32_t m_signature;
    uint32_t m_status;

    uint32_t m_crcIdentity;
    CLR_RT_HeapBlock_WeakReference_Identity m_identity;

    CLR_RT_HeapBlock_Array m_object;

    //--//

    static ObjectHeader *Find(FLASH_WORD *start, FLASH_WORD *end);

    bool Initialize(CLR_RT_HeapBlock_WeakReference *ref);

    bool HasGoodSignature() const
      {
      return m_signature == c_Version;
      }
    bool IsInUse() const
      {
      return m_status == c_InUseBlock;
      }

    bool IsGood(bool fIncludeData) const;

    void Delete();

    uint32_t Length() const;
    static uint32_t Length(const CLR_RT_HeapBlock_WeakReference *ref);
    static uint32_t Length(uint32_t data);

    ObjectHeader *Next() const;

    private:
      uint32_t ComputeCRC() const;

      //--//
    };

  uint32_t m_state;

  CLR_RT_HeapBlock_WeakReference *m_pending_object;
  ObjectHeader *m_pending_header;
  uint32_t m_pending_size;
  FLASH_WORD *m_pending_src;
  FLASH_WORD *m_pending_dst;

  int m_eraseIndex;

  //--//

  void Initialize();
  void Uninitialize();
  void EraseAll();

  void InvalidateEntry(CLR_RT_HeapBlock_WeakReference *weak);

  void Relocate();

  ObjectHeader *RecoverHeader(CLR_RT_HeapBlock_WeakReference *ref);

  static void Callback(void *arg);

#if !defined(BUILD_RTM)
  void GenerateStatistics(uint32_t &totalSize, uint32_t &inUse);
#endif

  void Flush();

  //--//

#undef DECL_POSTFIX
#if defined(NANOCLR_TRACE_PERSISTENCE)
#define DECL_POSTFIX
#else
#define DECL_POSTFIX                                                                                                   \
    {                                                                                                                  \
    }
#endif

  static void Trace_Emit(char *szText) DECL_POSTFIX;

  static void Trace_Printf(const char *format, ...) DECL_POSTFIX;

  static void Trace_DumpIdentity(char *&szBuffer, size_t &iBuffer, CLR_RT_HeapBlock_WeakReference_Identity *identity)
    DECL_POSTFIX;

  static void Trace_DumpState(
    const char *szText,
    FLASH_WORD *dst,
    ObjectHeader *oh,
    CLR_RT_HeapBlock_WeakReference *wr) DECL_POSTFIX;

  //--//

  private:
    bool AdvanceState(bool force);

    void EnqueueNextCallback();
  };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

extern CLR_RT_Persistence_Manager g_CLR_RT_Persistence_Manager;

//--//

#define Library_corlib_native_System_Collections_ArrayList__FIELD___items 1
#define Library_corlib_native_System_Collections_ArrayList__FIELD___size  2

struct CLR_RT_HeapBlock_ArrayList : public CLR_RT_HeapBlock
  {
  public:
    HRESULT GetItem(int32_t index, CLR_RT_HeapBlock *&value);
    HRESULT SetItem(int32_t index, CLR_RT_HeapBlock *value);

    HRESULT Add(CLR_RT_HeapBlock *value, int32_t &index);
    HRESULT Clear();
    HRESULT Insert(int32_t index, CLR_RT_HeapBlock *value);
    HRESULT RemoveAt(int32_t index);
    HRESULT SetCapacity(uint32_t newCapacity);

    //--//

    __inline int32_t GetSize()
      {
      return ((CLR_RT_HeapBlock *)this)[Library_corlib_native_System_Collections_ArrayList__FIELD___size]
        .NumericByRef()
        .s4;
      }

  private:
    // Keep in-sync with _defaultCapacity in System.Collections.ArrayList class in ArrayList.cs
    static const int32_t c_DefaultCapacity = 4;

    HRESULT EnsureCapacity(int32_t min, int32_t currentCapacity);

    __inline CLR_RT_HeapBlock_Array *GetItems()
      {
      return ((CLR_RT_HeapBlock *)this)[Library_corlib_native_System_Collections_ArrayList__FIELD___items]
        .DereferenceArray();
      }

    __inline void SetItems(CLR_RT_HeapBlock_Array *items)
      {
      ((CLR_RT_HeapBlock *)this)[Library_corlib_native_System_Collections_ArrayList__FIELD___items]
        .SetObjectReference(items);
      }
    __inline void SetSize(int32_t size)
      {
      ((CLR_RT_HeapBlock *)this)[Library_corlib_native_System_Collections_ArrayList__FIELD___size].SetInteger(size);
      }
  };

#if (NANOCLR_SYSTEM_COLLECTIONS == TRUE)

#define Library_nf_system_collections_System_Collections_Queue__FIELD___array 1
#define Library_nf_system_collections_System_Collections_Queue__FIELD___head  2
#define Library_nf_system_collections_System_Collections_Queue__FIELD___tail  3
#define Library_nf_system_collections_System_Collections_Queue__FIELD___size  4

struct CLR_RT_HeapBlock_Queue : public CLR_RT_HeapBlock
  {
  public:
    HRESULT Dequeue(CLR_RT_HeapBlock *&value);
    HRESULT Enqueue(CLR_RT_HeapBlock *value);

    HRESULT Peek(CLR_RT_HeapBlock *&value);
    HRESULT Clear();

    HRESULT CopyTo(CLR_RT_HeapBlock_Array *toArray, int32_t index);

    //--//

    __inline int32_t GetSize()
      {
      return ((CLR_RT_HeapBlock *)this)[Library_nf_system_collections_System_Collections_Queue__FIELD___size]
        .NumericByRef()
        .s4;
      }

  private:
    // Keep in-sync with _defaultCapacity in System.Collections.Queue class in Queue.cs
    static const int32_t c_DefaultCapacity = 4;

    static HRESULT ObjArrayMemcpy(
      CLR_RT_HeapBlock_Array *arraySrc,
      int indexSrc,
      CLR_RT_HeapBlock_Array *arrayDst,
      int indexDst,
      int length);

    __inline CLR_RT_HeapBlock_Array *GetArray()
      {
      return ((CLR_RT_HeapBlock *)this)[Library_nf_system_collections_System_Collections_Queue__FIELD___array]
        .DereferenceArray();
      }
    __inline int32_t Head()
      {
      return ((CLR_RT_HeapBlock *)this)[Library_nf_system_collections_System_Collections_Queue__FIELD___head]
        .NumericByRef()
        .s4;
      }
    __inline int32_t GetTail()
      {
      return ((CLR_RT_HeapBlock *)this)[Library_nf_system_collections_System_Collections_Queue__FIELD___tail]
        .NumericByRef()
        .s4;
      }

    __inline void SetArray(CLR_RT_HeapBlock_Array *array)
      {
      ((CLR_RT_HeapBlock *)this)[Library_nf_system_collections_System_Collections_Queue__FIELD___array]
        .SetObjectReference(array);
      }
    __inline void SetHead(int32_t head)
      {
      ((CLR_RT_HeapBlock *)this)[Library_nf_system_collections_System_Collections_Queue__FIELD___head].SetInteger(
        head);
      }
    __inline void SetTail(int32_t tail)
      {
      ((CLR_RT_HeapBlock *)this)[Library_nf_system_collections_System_Collections_Queue__FIELD___tail].SetInteger(
        tail);
      }
    __inline void SetSize(int32_t size)
      {
      ((CLR_RT_HeapBlock *)this)[Library_nf_system_collections_System_Collections_Queue__FIELD___size].SetInteger(
        size);
      }
  };

#define Library_nf_system_collections_System_Collections_Stack__FIELD___array 1
#define Library_nf_system_collections_System_Collections_Stack__FIELD___size  2

struct CLR_RT_HeapBlock_Stack : public CLR_RT_HeapBlock
  {
  public:
    HRESULT Pop(CLR_RT_HeapBlock *&value);
    HRESULT Push(CLR_RT_HeapBlock *value);

    HRESULT Peek(CLR_RT_HeapBlock *&value);
    HRESULT Clear();

    //--//

    __inline int32_t GetSize()
      {
      return ((CLR_RT_HeapBlock *)this)[Library_nf_system_collections_System_Collections_Stack__FIELD___size]
        .NumericByRef()
        .s4;
      }

  protected:
    // Keep in-sync with _defaultCapacity in System.Collections.Stack class in Stack.cs
    static const int32_t c_DefaultCapacity = 4;

    __inline CLR_RT_HeapBlock_Array *GetArray()
      {
      return ((CLR_RT_HeapBlock *)this)[Library_nf_system_collections_System_Collections_Stack__FIELD___array]
        .DereferenceArray();
      }

    __inline void SetArray(CLR_RT_HeapBlock_Array *array)
      {
      ((CLR_RT_HeapBlock *)this)[Library_nf_system_collections_System_Collections_Stack__FIELD___array]
        .SetObjectReference(array);
      }
    __inline void SetSize(int32_t size)
      {
      ((CLR_RT_HeapBlock *)this)[Library_nf_system_collections_System_Collections_Stack__FIELD___size].SetInteger(
        size);
      }
  };

#endif // (NANOCLR_SYSTEM_COLLECTIONS == TRUE)

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _NANOCLR_RUNTIME__HEAPBLOCK_H_
