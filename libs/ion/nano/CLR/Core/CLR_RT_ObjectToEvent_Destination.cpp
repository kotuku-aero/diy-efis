//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_ObjectToEvent_Destination::Initialize()
  {
 
  m_references.DblLinkedList_Initialize();
  }

void CLR_RT_ObjectToEvent_Destination::CheckAll()
  {
 
  NANOCLR_FOREACH_NODE(CLR_RT_ObjectToEvent_Source, ref, m_references)
    {
    ref->EnsureObjectIsAlive();
    }
  NANOCLR_FOREACH_NODE_END();
  }

void CLR_RT_ObjectToEvent_Destination::SignalAll()
  {
 
  NANOCLR_FOREACH_NODE(CLR_RT_ObjectToEvent_Source, ref, m_references)
    {
    if (ref->m_objectPtr)
      {
      CLR_RT_HeapBlock_WaitForObject::SignalObject(*ref->m_objectPtr);
      }
    }
  NANOCLR_FOREACH_NODE_END();
  }

void CLR_RT_ObjectToEvent_Destination::DetachAll()
  {
 
  while (true)
    {
    CLR_RT_ObjectToEvent_Source *ref = (CLR_RT_ObjectToEvent_Source *)m_references.ExtractFirstNode(); if (!ref) break;

    ref->Detach();
    }
  }

bool CLR_RT_ObjectToEvent_Destination::IsReadyForRelease()
  {
 
  if (m_references.IsEmpty())
    {
    if (IsForcedAlive() == false)
      {
      return true;
      }
    }

  return false;
  }

bool CLR_RT_ObjectToEvent_Destination::ReleaseWhenDead()
  {
 
  bool res = IsReadyForRelease();

  if (res) g_CLR_RT_EventCache.Append_Node(this);

  return res;
  }

