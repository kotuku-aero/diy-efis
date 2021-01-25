//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t CLR_RT_GarbageCollector::ExecuteCompaction()
  {
 

#if defined(NANOCLR_TRACE_MEMORY_STATS)
  if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Info)
    {
    trace_debug("GC: performing heap compaction\r\n");
    }
#endif

  ////////////////////////////////////////////////////////////////////////////////////////////////

  CLR_RT_ExecutionEngine::ExecutionConstraint_Suspend();

  Heap_Compact();

  CLR_RT_ExecutionEngine::ExecutionConstraint_Resume();

  m_numberOfCompactions++;

#if defined(NANOCLR_TRACE_MEMORY_STATS)
  if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Info)
    {
    trace_debug("GC: heap compaction completed\r\n");
    }
#endif

  return 0;
  }

////////////////////////////////////////////////////////////////////////////////

void CLR_RT_GarbageCollector::Heap_Compact()
  {
 

  ValidatePointers();

  //--//

  RelocationRegion relocHelper[c_minimumSpaceForCompact];
  const size_t     relocMax = ARRAYSIZE(relocHelper);

  Heap_Relocate_Prepare(relocHelper, relocMax);

  RelocationRegion *relocBlocks = relocHelper;
  RelocationRegion *relocCurrent = relocBlocks;

  //--//

  TestPointers_PopulateOld();


  CLR_RT_HeapCluster *freeRegion_hc = NULL;;
  CLR_RT_HeapBlock_Node *freeRegion = NULL;

  CLR_RT_HeapCluster *currentSource_hc = (CLR_RT_HeapCluster *)g_CLR_RT_ExecutionEngine.m_heap.FirstNode();
  while (currentSource_hc->Next())
    {
    CLR_RT_HeapBlock_Node *currentSource = currentSource_hc->m_payloadStart;
    CLR_RT_HeapBlock_Node *currentSource_end = currentSource_hc->m_payloadEnd;

    if (!freeRegion)
      {
      //
      // Move to the next first free region.
      //
      freeRegion_hc = (CLR_RT_HeapCluster *)g_CLR_RT_ExecutionEngine.m_heap.FirstNode();
      while (true)
        {
        CLR_RT_HeapCluster *freeRegion_hcNext = (CLR_RT_HeapCluster *)freeRegion_hc->Next(); if (!freeRegion_hcNext) break;

        freeRegion = freeRegion_hc->m_freeList.FirstNode(); if (freeRegion->Next()) break;

        freeRegion = NULL;
        freeRegion_hc = freeRegion_hcNext;
        }
      if (!freeRegion) break;
      }

    while (true)
      {
      //
      // We can only move backward.
      //
      if (currentSource < freeRegion)
        {
        currentSource_hc = freeRegion_hc;
        currentSource = freeRegion;
        currentSource_end = freeRegion_hc->m_payloadEnd;
        }

      while (currentSource < currentSource_end && currentSource->IsFlagSet(CLR_RT_HeapBlock::HB_Unmovable))
        {
        currentSource += currentSource->DataSize();
        }

      if (currentSource == currentSource_end) break;

      //////////////////////////////////////////////////////
      //
      // At this point, we have at least ONE movable block.
      //
      //////////////////////////////////////////////////////

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_4_CompactionPlus
      if (IsBlockInFreeList(g_CLR_RT_ExecutionEngine.m_heap, freeRegion, true) == false)
        {
        trace_debug("'freeRegion' is not in a free list!! %08x\r\n", freeRegion);

        NANOCLR_DEBUG_STOP();
        }

      if (IsBlockInFreeList(g_CLR_RT_ExecutionEngine.m_heap, currentSource, false))
        {
        trace_debug("'currentSource' is in a free list!! %08x\r\n", currentSource);

        NANOCLR_DEBUG_STOP();
        }
#endif

      if (m_relocCount >= relocMax)
        {
        ValidateHeap(g_CLR_RT_ExecutionEngine.m_heap);

        Heap_Relocate();

        ValidateHeap(g_CLR_RT_ExecutionEngine.m_heap);

        relocBlocks = m_relocBlocks;
        relocCurrent = relocBlocks;

        TestPointers_PopulateOld();
        }

      {
      uint32_t move = 0;
      uint32_t freeRegion_Size = freeRegion->DataSize();
      bool       fSlide;

      relocCurrent->m_destination = (uint8_t *)freeRegion;
      relocCurrent->m_start = (uint8_t *)currentSource;
      relocCurrent->m_offset = (uint32_t)(relocCurrent->m_destination - relocCurrent->m_start);


      //
      // Are the free block and the last moved block adjacent?
      //
      if (currentSource == freeRegion + freeRegion_Size)
        {
        while (currentSource < currentSource_end && currentSource->IsFlagSet(CLR_RT_HeapBlock::HB_Unmovable) == false)
          {
          uint32_t len = currentSource->DataSize();

          currentSource += len;
          move += len;
          }

        fSlide = true;
        }
      else
        {
        while (freeRegion_Size && currentSource < currentSource_end && currentSource->IsFlagSet(CLR_RT_HeapBlock::HB_Unmovable) == false)
          {
          uint32_t len = currentSource->DataSize();

          if (freeRegion_Size < len)
            {
            break;
            }

          freeRegion_Size -= len;
          currentSource += len;
          move += len;
          }

        fSlide = false;
        }

      if (move)
        {
        //
        // Skip forward to the next movable block.
        //
        while (currentSource < currentSource_end && currentSource->IsFlagSet(CLR_RT_HeapBlock::HB_Unmovable))
          {
          currentSource += currentSource->DataSize();
          }

        uint32_t moveBytes = move * sizeof(*currentSource);

        relocCurrent->m_end = relocCurrent->m_start + moveBytes;

        //--//

        //
        // Remove the old free block, copy the data, recreate the new free block.
        // Merge with the following one if they are adjacent now.
        //
        CLR_RT_HeapBlock_Node *freeRegionNext = freeRegion->Next();

        freeRegion->Unlink();

        memmove(relocCurrent->m_destination, relocCurrent->m_start, moveBytes);

        if (freeRegion_Size)
          {

          freeRegion = freeRegion_hc->InsertInOrder(freeRegion + move, freeRegion_Size);

          }
        else
          {
          freeRegion = freeRegionNext;

          }

        if (fSlide == false)
          {
          CLR_RT_HeapBlock_Node *dst = currentSource_hc->InsertInOrder((CLR_RT_HeapBlock_Node *)relocCurrent->m_start, move);

          if (dst < freeRegion && freeRegion < (dst + dst->DataSize()))
            {
            freeRegion = dst;
            }

          }

        CLR_RT_GarbageCollector::ValidateCluster(currentSource_hc);
        CLR_RT_GarbageCollector::ValidateCluster(freeRegion_hc);

        relocCurrent++;
        m_relocCount++;
        }
      else
        {
        freeRegion = freeRegion->Next();

        }

      if (freeRegion->Next() == NULL)
        {

        freeRegion = NULL;
        freeRegion_hc = (CLR_RT_HeapCluster *)freeRegion_hc->Next();
        while (true)
          {
          CLR_RT_HeapCluster *freeRegion_hcNext = (CLR_RT_HeapCluster *)freeRegion_hc->Next(); if (!freeRegion_hcNext) break;

          freeRegion = freeRegion_hc->m_freeList.FirstNode(); if (freeRegion->Next()) break;

          freeRegion = NULL;
          freeRegion_hc = freeRegion_hcNext;
          }
        if (!freeRegion) break;

        }
      }
      }

    currentSource_hc = (CLR_RT_HeapCluster *)currentSource_hc->Next();
    }

  if (m_relocCount)
    {
    ValidateHeap(g_CLR_RT_ExecutionEngine.m_heap);

    Heap_Relocate();

    ValidateHeap(g_CLR_RT_ExecutionEngine.m_heap);
    }
  }

void CLR_RT_GarbageCollector::Heap_Relocate_Prepare(RelocationRegion *blocks, size_t total)
  {
 
  m_relocBlocks = blocks;
  m_relocTotal = total;
  m_relocCount = 0;
  }

void CLR_RT_GarbageCollector::Heap_Relocate_AddBlock(uint8_t *dst, uint8_t *src, uint32_t length)
  {
 
  RelocationRegion *reloc = m_relocBlocks;
  size_t            count = m_relocCount;

  while (count)
    {
    if (reloc->m_start > src)
      {
      //
      // Insert region, so they are sorted by start address.
      //
      memmove(&reloc[1], &reloc[0], count * sizeof(*reloc));
      break;
      }

    reloc++;
    count--;
    }

  reloc->m_start = src;
  reloc->m_end = &src[length];
  reloc->m_destination = dst;
  reloc->m_offset = (uint32_t)(dst - src);

  if (++m_relocCount == m_relocTotal)
    {
    Heap_Relocate();
    }
  }

void CLR_RT_GarbageCollector::Heap_Relocate()
  {
 
  if (m_relocCount)
    {
    RelocationRegion *relocBlocks = m_relocBlocks;

    uint8_t *relocMinimum = relocBlocks->m_start;
    uint8_t *relocMaximum = relocBlocks->m_end;

    for (size_t i = 0; i < m_relocCount; i++, relocBlocks++)
      {
      if (relocMinimum > relocBlocks->m_start) relocMinimum = relocBlocks->m_start;
      if (relocMaximum < relocBlocks->m_end) relocMaximum = relocBlocks->m_end;
      }

    m_relocMinimum = relocMinimum;
    m_relocMaximum = relocMaximum;

    TestPointers_Remap();

    Heap_Relocate_Pass(NULL);

    ValidatePointers();

    TestPointers_PopulateNew();

    m_relocCount = 0;
    }
  }

void CLR_RT_GarbageCollector::Heap_Relocate_Pass(RelocateFtn ftn)
  {
 

#if NANOCLR_VALIDATE_HEAP > NANOCLR_VALIDATE_HEAP_0_None
  m_relocWorker = ftn;
#else
  (void)ftn;
#endif

  NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster, hc, g_CLR_RT_ExecutionEngine.m_heap)
    {
    CLR_RT_HeapBlock_Node *ptr = hc->m_payloadStart;
    CLR_RT_HeapBlock_Node *end = hc->m_payloadEnd;


    while (ptr < end)
      {
      CLR_RT_HEAPBLOCK_RELOCATE(ptr);

      ptr += ptr->DataSize();
      }
    }
  NANOCLR_FOREACH_NODE_END();

  g_CLR_RT_ExecutionEngine.Relocate();
  }

//--//

void CLR_RT_GarbageCollector::Heap_Relocate(CLR_RT_HeapBlock *lst, uint32_t len)
  {
 
  while (len--)
    {
    CLR_RT_HEAPBLOCK_RELOCATE(lst);

    lst++;
    }
  }

void CLR_RT_GarbageCollector::Heap_Relocate(void **ref)
  {
 
  uint8_t *dst = (uint8_t *)*ref;

#if NANOCLR_VALIDATE_HEAP > NANOCLR_VALIDATE_HEAP_0_None
  if (g_CLR_RT_GarbageCollector.m_relocWorker)
    {
    g_CLR_RT_GarbageCollector.m_relocWorker(ref);
    }
  else
#endif
    {
    if (dst >= g_CLR_RT_GarbageCollector.m_relocMinimum && dst < g_CLR_RT_GarbageCollector.m_relocMaximum)
      {
      RelocationRegion *relocBlocks = g_CLR_RT_GarbageCollector.m_relocBlocks;
      size_t            left = 0;
      size_t            right = g_CLR_RT_GarbageCollector.m_relocCount;

      while (left < right)
        {
        size_t            center = (left + right) / 2;
        RelocationRegion &relocCurrent = relocBlocks[center];

        if (dst < relocCurrent.m_start)
          {
          right = center;
          }
        else if (dst >= relocCurrent.m_end)
          {
          left = center + 1;
          }
        else
          {
          *ref = (void *)(dst + relocCurrent.m_offset);

          return;
          }
        }
      }
    }
  }

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_3_Compaction

bool CLR_RT_GarbageCollector::Relocation_JustCheck(void **ref)
  {
 
  uint8_t *dst = (uint8_t *)*ref;

  if (dst)
    {
    ValidateBlockNotInFreeList(g_CLR_RT_ExecutionEngine.m_heap, (CLR_RT_HeapBlock_Node *)dst);
    }

  return true;
  }

#endif

