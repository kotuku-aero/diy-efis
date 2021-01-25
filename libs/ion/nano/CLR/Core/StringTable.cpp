//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StringTableData.cpp"

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_Assembly::InitString()
  {
 
  }

const char *CLR_RT_Assembly::GetString(uint16_t i)
  {
 
  static const uint16_t iMax = 0xFFFF - c_CLR_StringTable_Size;

  if (i >= iMax)
    {
    return &c_CLR_StringTable_Data[c_CLR_StringTable_Lookup[(uint16_t)0xFFFF - i]];
    }

  return &(((const char *)GetTable(TBL_Strings))[i]);
  }

#if defined(_WIN32)

void CLR_RT_Assembly::InitString(std::map<std::string, uint16_t> &map)
  {
 
  const uint16_t *array = c_CLR_StringTable_Lookup;
  size_t            len = c_CLR_StringTable_Size;
  uint16_t        idx = 0xFFFF;

  map.clear();

  while (len-- > 0)
    {
    map[&c_CLR_StringTable_Data[*array++]] = idx--;
    }
  }

#endif

