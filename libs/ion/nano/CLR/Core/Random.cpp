//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

void CLR_RT_Random::Initialize()
  {
  int64_t  st = HAL_Time_CurrentTime();

  srand((unsigned int)st);
  }

void CLR_RT_Random::Initialize(int seed)
  {
  srand(seed);
  }

uint32_t CLR_RT_Random::Next()
  {
  return rand();
  }

double CLR_RT_Random::NextDouble()
  {
  // Next() will return value between 0 - 0x7FFFFFFF (inclusive)
  return ((double)rand()) / ((double)0x7FFFFFFF);
  }

void CLR_RT_Random::NextBytes(unsigned char *buffer, unsigned int count)
  {
  unsigned int i;

  for (i = 0; i < count; i++)
    {
    buffer[i] = (unsigned char)rand();
    }
  }
