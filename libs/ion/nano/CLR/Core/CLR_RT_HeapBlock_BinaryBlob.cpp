//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_HeapBlock_BinaryBlob::CreateInstance(CLR_RT_HeapBlock &reference, uint32_t length, CLR_RT_MarkingHandler mark, CLR_RT_RelocationHandler relocate, uint32_t flags)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock_BinaryBlob *obj = Allocate(length, flags); CHECK_ALLOCATION(obj);

  reference.SetObjectReference(obj);

  obj->SetBinaryBlobHandlers(mark, relocate);
  obj->m_assembly = NULL;

  NANOCLR_NOCLEANUP();
  }

CLR_RT_HeapBlock_BinaryBlob *CLR_RT_HeapBlock_BinaryBlob::Allocate(uint32_t length, uint32_t flags)
  {
 
  uint32_t totLength = (uint32_t)(sizeof(CLR_RT_HeapBlock_BinaryBlob) + length);

  CLR_RT_HeapBlock_BinaryBlob *obj;

  if (flags & CLR_RT_HeapBlock::HB_Event)
    {
    obj = EVENTCACHE_EXTRACT_NODE_AS_BYTES(g_CLR_RT_EventCache, CLR_RT_HeapBlock_BinaryBlob, DATATYPE_BINARY_BLOB_HEAD, flags, totLength);
    }
  else
    {
    obj = (CLR_RT_HeapBlock_BinaryBlob *)g_CLR_RT_ExecutionEngine.ExtractHeapBytesForObjects(DATATYPE_BINARY_BLOB_HEAD, flags, totLength);
    }

  if (obj)
    {
    obj->SetBinaryBlobHandlers(NULL, NULL);
    }

  return obj;
  }

void CLR_RT_HeapBlock_BinaryBlob::Release(bool fEvent)
  {
 
  SetBinaryBlobHandlers(NULL, NULL);

  if (fEvent) g_CLR_RT_EventCache.Append_Node(this);
  }

void CLR_RT_HeapBlock_BinaryBlob::Relocate()
  {
 
  CLR_RT_RelocationHandler relocate = BinaryBlobRelocationHandler();

  if (relocate)
    {
    relocate(this);
    }
  }

