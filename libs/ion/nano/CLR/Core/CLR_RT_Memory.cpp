//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NANOCLR_TRACE_MALLOC)
static uint32_t s_TotalAllocated;
#endif

CLR_RT_MemoryRange s_CLR_RT_Heap = { 0, 0 };

static int s_PreHeapInitIndex = 0;

////////////////////////////////////////////////////////////



//--//

void CLR_RT_Memory::Reset()
  {
 
  ::HeapLocation(s_CLR_RT_Heap.m_location, s_CLR_RT_Heap.m_size);
  }

void *CLR_RT_Memory::SubtractFromSystem(size_t len)
  {
 
  len = ROUNDTOMULTIPLE(len, uint32_t);

  if (len <= s_CLR_RT_Heap.m_size)
    {
    s_CLR_RT_Heap.m_size -= (uint32_t)len;

    return &s_CLR_RT_Heap.m_location[s_CLR_RT_Heap.m_size];
    }

  return NULL;
  }

//--//

#if defined(NANOCLR_FILL_MEMORY_WITH_DIRTY_PATTERN)

#define DEBUG_POINTER_INCREMENT(ptr,size) ptr = (void*)((char*)ptr + (size))
#define DEBUG_POINTER_DECREMENT(ptr,size) ptr = (void*)((char*)ptr - (size))

const uint32_t c_extra = sizeof(CLR_RT_HeapBlock) * 2;

#endif


void CLR_RT_Memory::Release(void *ptr)
  {
 

  // CLR heap not initialized yet, return (this is not an error condition because we allow pre
  if (s_CLR_RT_Heap.m_size == 0)
    {
    return;
    }

  if (ptr)
    {
#if defined(NANOCLR_FILL_MEMORY_WITH_DIRTY_PATTERN)
    DEBUG_POINTER_DECREMENT(ptr, c_extra + sizeof(uint32_t));
#endif

    CLR_RT_HeapBlock_BinaryBlob *pThis = CLR_RT_HeapBlock_BinaryBlob::GetBlob(ptr);

    if (pThis->DataType() != DATATYPE_BINARY_BLOB_HEAD)
      {
      NANOCLR_STOP();
      }
    else
      {
#if defined(NANOCLR_FILL_MEMORY_WITH_DIRTY_PATTERN)
      uint32_t len = *(uint32_t *)ptr;
      uint8_t *blk;
      uint32_t pos;

      for (pos = 0, blk = (uint8_t *)ptr + sizeof(len); pos < c_extra; pos++, blk++)
        {
        if (*blk != 0xDD)
          {
          trace_debug("CLR_RT_Memory::Release: HEAP OVERRUN START %08x(%d) = %08x\r\n", ptr, len, blk);
          NANOCLR_STOP();
          }
        }

      for (pos = 0, blk = (uint8_t *)ptr + len - c_extra; pos < c_extra; pos++, blk++)
        {
        if (*blk != 0xDD)
          {
          trace_debug("CLR_RT_Memory::Release: HEAP OVERRUN END %08x(%d) = %08x\r\n", ptr, len, blk);
          NANOCLR_STOP();
          }
        }
#endif

#if defined(NANOCLR_TRACE_MALLOC)
      s_TotalAllocated -= pThis->DataSize();
      trace_debug("CLR_RT_Memory::Release : %08x = %3d blocks (tot %4d)\r\n", ptr, pThis->DataSize(), s_TotalAllocated);
#endif

      pThis->Release(true);
      }
    }
  }

void *CLR_RT_Memory::Allocate(size_t len, uint32_t flags)
  {
 

  if (s_CLR_RT_Heap.m_size == 0)
    {
    unsigned char *heapStart = NULL;
    unsigned int heapSize = 0;

    ::HeapLocation(heapStart, heapSize);

    if (len > heapSize)
      {
      ASSERT(false);
      return NULL;

      }

    // use the current index to prevent heap thrashing before initialization
    heapStart = &heapStart[s_PreHeapInitIndex];

    s_PreHeapInitIndex += len;

    return heapStart;
    }

  flags |= CLR_RT_HeapBlock::HB_Event;

#if defined(NANOCLR_FILL_MEMORY_WITH_DIRTY_PATTERN)
  len += c_extra * 2 + sizeof(uint32_t);
#endif

  CLR_RT_HeapBlock_BinaryBlob *obj = CLR_RT_HeapBlock_BinaryBlob::Allocate((uint32_t)len, flags);
  if (obj)
    {
    void *res = obj->GetData();

#if defined(NANOCLR_TRACE_MALLOC)
    s_TotalAllocated += obj->DataSize();
    trace_debug("CLR_RT_Memory::Allocate: %08x = %3d blocks (tot %4d), %d bytes\r\n", res, obj->DataSize(), s_TotalAllocated, len);
#endif

#if defined(NANOCLR_FILL_MEMORY_WITH_DIRTY_PATTERN)
    memset(res, 0xDD, len);

    *(uint32_t *)res = (uint32_t)len;

    DEBUG_POINTER_INCREMENT(res, c_extra + sizeof(uint32_t));
#endif

    return res;
    }

  return NULL;
  }

void *CLR_RT_Memory::Allocate_And_Erase(size_t len, uint32_t flags)
  {
 
  void *ptr = CLR_RT_Memory::Allocate(len, flags);

  if (ptr) ZeroFill(ptr, len);

  return ptr;
  }


void *CLR_RT_Memory::ReAllocate(void *ptr, size_t len)
  {
 

  // allocate always as an event but do not run GC on failure
  void *p = CLR_RT_Memory::Allocate(len, CLR_RT_HeapBlock::HB_Event | CLR_RT_HeapBlock::HB_NoGcOnFailedAllocation); if (!p) return NULL;

  if (ptr)
    {
    CLR_RT_HeapBlock_BinaryBlob *pThis = CLR_RT_HeapBlock_BinaryBlob::GetBlob(ptr);

    size_t prevLen = pThis->DataSize() * sizeof(CLR_RT_HeapBlock);

    memcpy(p, ptr, len > prevLen ? prevLen : len);

    CLR_RT_Memory::Release(ptr);

    ptr = p;
    }

  return p;
  }

