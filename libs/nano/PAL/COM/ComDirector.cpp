//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "../Include/nanoPAL.h"

int DebuggerPort_Write(COM_HANDLE ComPortNum, const char *Data, size_t size, int maxRetries)
  {
  return DebuggerPort_Write(ComPortNum, Data, size);
  }
