//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

CLR_RT_HeapBlock_String *CLR_RT_HeapBlock_String::CreateInstance(CLR_RT_HeapBlock &reference, uint32_t length)
  {
 
  uint32_t               totLength = sizeof(CLR_RT_HeapBlock_String) + length + 1;
  CLR_RT_HeapBlock_String *str;

  reference.SetObjectReference(NULL);

  str = (CLR_RT_HeapBlock_String *)g_CLR_RT_ExecutionEngine.ExtractHeapBytesForObjects(DATATYPE_STRING, 0, totLength);
  if (str)
    {
    char *szText = (char *)&str[1]; szText[0] = 0;

    str->SetStringText(szText);

    reference.SetObjectReference(str);
    }

  return str;
  }

HRESULT CLR_RT_HeapBlock_String::CreateInstance(CLR_RT_HeapBlock &reference, const char *szText)
  {
 
  HRESULT hr;

  if (!szText) szText = "";

  NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_String::CreateInstance(reference, szText, (uint32_t)hal_strlen_s(szText)));

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock_String::CreateInstance(CLR_RT_HeapBlock &reference, const char *szText, uint32_t length)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock_String *str;
  char *szTextDst;

  str = CreateInstance(reference, length); CHECK_ALLOCATION(str);

  szTextDst = (char *)str->StringText();

  memcpy(szTextDst, szText, length); szTextDst[length] = 0;

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock_String::CreateInstance(CLR_RT_HeapBlock &reference, const char *szText, CLR_RT_Assembly *assm)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock_String *str;

  reference.SetObjectReference(NULL);

  str = (CLR_RT_HeapBlock_String *)g_CLR_RT_ExecutionEngine.ExtractHeapBytesForObjects(DATATYPE_STRING, 0, sizeof(CLR_RT_HeapBlock_String)); CHECK_ALLOCATION(str);

  reference.SetObjectReference(str);

  NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(reference, szText));

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock_String::CreateInstance(CLR_RT_HeapBlock &reference, uint32_t token, CLR_RT_Assembly *assm)
  {
 
  return CreateInstance(reference, assm->GetString(CLR_DataFromTk(token)), assm);
  }

HRESULT CLR_RT_HeapBlock_String::CreateInstance(CLR_RT_HeapBlock &reference, uint16_t *szText, uint32_t length)
  {
 
  HRESULT hr;

  CLR_RT_UnicodeHelper     uh;             uh.SetInputUTF16(szText);
  uint32_t               lengthInBytes = uh.CountNumberOfBytes(length);
  CLR_RT_HeapBlock_String *str = CreateInstance(reference, lengthInBytes); CHECK_ALLOCATION(str);

  uh.m_outputUTF8 = (uint8_t *)str->StringText();
  uh.m_outputUTF8_size = lengthInBytes + 1;

  uh.ConvertToUTF8(length, false);

  NANOCLR_NOCLEANUP();
  }

CLR_RT_HeapBlock_String *CLR_RT_HeapBlock_String::GetStringEmpty()
  {
  return g_CLR_RT_TypeSystem.m_assemblyMscorlib->GetStaticField(Library_corlib_native_System_String::FIELD_STATIC__Empty)->DereferenceString();
  }

