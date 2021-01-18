//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_CORE_H_
#define _NANOCLR_CORE_H_

#include "../CorLib/mscorlib/corlib_native.h"
#include "../Include/nanoCLR_Runtime.h"
#include "../Include/nanoCLR_Checks.h"
//#include <nanoCLR_Diagnostics.h>
#include "../Include/nanoCLR_Runtime.h"
//#include <nanoCLR_Stream.h>
//#include <nanoCLR_Xml.h>
//#include <nanoCLR_Application.h>
//
//#include <nanoCLR_ParseOptions.h>
//
//#include <SPOT_native.h>
//#include <SPOT_hardware_native.h>
#include "../Include/nanoCLR_Runtime__HeapBlock.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_BUILD 0
#define VERSION_REVISION 0
#define OEMSYSTEMINFOSTRING "CanFly"
#define TARGETNAMESTRING "CanFly"
#define PLATFORMNAMESTRING "nanoFramework"

// used for tracking exceptions
struct StackTrace
  {
  CLR_RT_MethodDef_Index m_md;
  CLR_UINT32 m_IP;
  };

extern HRESULT ExceptionCreateInstance(CLR_RT_HeapBlock &ref, HRESULT hrIn, CLR_RT_StackFrame *stack);
extern HRESULT ExceptionCreateInstance(CLR_RT_HeapBlock &ref, const CLR_RT_TypeDef_Index &cls, HRESULT hr, CLR_RT_StackFrame *stack);
extern HRESULT SetStackTrace(CLR_RT_HeapBlock &ref, CLR_RT_StackFrame *stack);
extern StackTrace *GetStackTrace(CLR_RT_HeapBlock *obj, CLR_UINT32 &depth);
extern CLR_INT64 *GetValuePtr(CLR_RT_HeapBlock &ref);
extern CLR_RT_HeapBlock *ExceptionGetTarget(CLR_RT_HeapBlock &ref);
extern HRESULT ConvertToCharArray(const char *szText, CLR_RT_HeapBlock &ref, CLR_RT_HeapBlock_Array *&array, int startIndex, int length);


#endif // _NANOCLR_CORE_H_
