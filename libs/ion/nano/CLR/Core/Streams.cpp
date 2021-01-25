//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_HeapBlock_MemoryStream::CreateInstance(CLR_RT_HeapBlock_MemoryStream *&stream, uint8_t *buf, int len)
  {
 
  HRESULT hr;

  stream = EVENTCACHE_EXTRACT_NODE(g_CLR_RT_EventCache, CLR_RT_HeapBlock_MemoryStream, DATATYPE_MEMORY_STREAM_HEAD); CHECK_ALLOCATION(stream);

  NANOCLR_SET_AND_LEAVE(stream->Initialize(buf, len));

  NANOCLR_NOCLEANUP();
  }

void CLR_RT_HeapBlock_MemoryStream::DeleteInstance(CLR_RT_HeapBlock_MemoryStream *&stream)
  {
 
  if (stream)
    {
    stream->Release();

    g_CLR_RT_EventCache.Append_Node(stream);

    stream = NULL;
    }
  }

HRESULT CLR_RT_HeapBlock_MemoryStream::Initialize(uint8_t *buf, uint32_t len)
  {
 
  HRESULT hr;

  m_buffers.DblLinkedList_Initialize(); // CLR_RT_DblLinkedList m_buffers;           // EVENT HEAP - NO RELOCATION - list of CLR_RT_HeapBlock_MemoryStream::Buffer

  CHECK_ALLOCATION(NewBuffer());

  if (buf)
    {
    m_current->m_data = buf;
    m_current->m_length = len;
    }

  NANOCLR_NOCLEANUP();
  }

void CLR_RT_HeapBlock_MemoryStream::Release()
  {
 
  m_buffers.DblLinkedList_PushToCache();
  }

//--//

void CLR_RT_HeapBlock_MemoryStream::Rewind()
  {
 
  m_current = (Buffer *)m_buffers.FirstNode();
  m_pos = -1;
  m_avail = 0;
  }

void CLR_RT_HeapBlock_MemoryStream::Reset()
  {
 
  NANOCLR_FOREACH_NODE(Buffer, node, m_buffers)
    {
    node->m_length = 0;
    }
  NANOCLR_FOREACH_NODE_END();
  Rewind();
  }

HRESULT CLR_RT_HeapBlock_MemoryStream::ToArray(CLR_RT_HeapBlock &ref)
  {
 
  HRESULT hr;

  uint32_t              tot;
  CLR_RT_HeapBlock_Array *array;
  uint8_t *buf;

  tot = 0;
  NANOCLR_FOREACH_NODE(Buffer, node, m_buffers)
    {
    tot += node->m_length;
    }
  NANOCLR_FOREACH_NODE_END();

  NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(ref, tot, g_CLR_RT_WellKnownTypes.m_UInt8));

  array = ref.DereferenceArray();
  buf = array->GetFirstElement();

  NANOCLR_FOREACH_NODE(Buffer, node, m_buffers)
    {
    memcpy(buf, node->m_payload, node->m_length); buf += node->m_length;
    }
  NANOCLR_FOREACH_NODE_END();

  NANOCLR_NOCLEANUP();
  }

//--//

CLR_RT_HeapBlock_MemoryStream::Buffer *CLR_RT_HeapBlock_MemoryStream::NewBuffer()
  {
 
  Buffer *ptr = EVENTCACHE_EXTRACT_NODE(g_CLR_RT_EventCache, Buffer, DATATYPE_MEMORY_STREAM_DATA);

  if (ptr)
    {
    m_current = ptr;
    m_pos = -1;
    m_avail = 0;

    ptr->m_data = ptr->m_payload;
    ptr->m_length = 0;

    m_buffers.LinkAtBack(ptr);
    }

  return ptr;
  }

//--//

int CLR_RT_HeapBlock_MemoryStream::BitsAvailable()
  {
 
  int val = m_avail - 8 * m_pos - 8;

  NANOCLR_FOREACH_NODE__DIRECT(Buffer, ptr, m_current)
    {
    val += 8 * ptr->m_length;
    }
  NANOCLR_FOREACH_NODE_END();

  return val;
  }

int CLR_RT_HeapBlock_MemoryStream::BitsWritten()
  {
 
  int val = m_pos * 8 - m_avail + 8;

  NANOCLR_FOREACH_NODE_BACKWARD__DIRECT(Buffer, ptr, m_current->Prev())
    {
    val += 8 * ptr->m_length;
    }
  NANOCLR_FOREACH_NODE_END();

  return val;
  }

HRESULT CLR_RT_HeapBlock_MemoryStream::ReadBits(uint64_t &res, uint32_t bits)
  {
 
  HRESULT hr;

  uint32_t *ptr = (uint32_t *)&res;

  if (bits > 32) { NANOCLR_CHECK_HRESULT(ReadBits(ptr[1], bits - 32)); bits = 32; }
  else { ptr[1] = 0; }

  NANOCLR_SET_AND_LEAVE(ReadBits(ptr[0], bits));

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock_MemoryStream::ReadBits(uint32_t &res, uint32_t bits)
  {
 
  HRESULT hr;

  uint32_t val = 0;
  uint32_t pos = bits;

  while (bits > 0)
    {
    while (m_avail == 0)
      {
      m_pos++;
      m_avail = 8;

      while (m_pos >= m_current->m_length)
        {
        m_current = (Buffer *)m_current->Next();

        if (m_current->Next() == NULL)
          {
          NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
          }

        m_pos = 0;
        }
      }

    uint32_t insert = (bits < m_avail) ? bits : m_avail;
    uint32_t mask = (1 << insert) - 1;

    pos -= insert; val |= (((uint32_t)m_current->m_data[m_pos] >> (m_avail - insert)) & mask) << pos;
    bits -= insert;
    m_avail -= insert;
    }

  res = val;

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock_MemoryStream::WriteBits(uint64_t res, uint32_t bits)
  {
 
  HRESULT hr;

  if (bits > 32) { NANOCLR_CHECK_HRESULT(WriteBits((uint32_t)(res >> 32), bits - 32)); bits = 32; }

  NANOCLR_SET_AND_LEAVE(WriteBits((uint32_t)res, bits));

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock_MemoryStream::WriteBits(uint32_t val, uint32_t bits)
  {
 
  HRESULT hr;

  uint32_t pos = bits;

  while (bits > 0)
    {
    while (m_avail == 0)
      {
      m_pos++;

      if (m_pos < (int)sizeof(m_current->m_payload))
        {
        m_current->m_length++;
        m_current->m_payload[m_pos] = 0;
        m_avail = 8;
        break;
        }

      CHECK_ALLOCATION(NewBuffer());
      }

    uint32_t insert = (bits < m_avail) ? bits : m_avail;
    uint32_t mask = (1 << insert) - 1;

    pos -= insert; m_current->m_payload[m_pos] |= (uint8_t)(((val >> pos) & mask) << (m_avail - insert));
    bits -= insert;
    m_avail -= insert;

    }

  NANOCLR_NOCLEANUP();
  }

//--//

HRESULT CLR_RT_HeapBlock_MemoryStream::ReadArray(uint8_t *buf, uint32_t bytes)
  {
 
  HRESULT hr;

  uint32_t data;

  while (bytes-- > 0)
    {
    NANOCLR_CHECK_HRESULT(ReadBits(data, 8));

    *buf++ = (uint8_t)data;
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock_MemoryStream::WriteArray(const uint8_t *buf, uint32_t bytes)
  {
 
  HRESULT hr;

  while (bytes-- > 0)
    {
    NANOCLR_CHECK_HRESULT(WriteBits((uint32_t)*buf++, 8));
    }

  NANOCLR_NOCLEANUP();
  }

