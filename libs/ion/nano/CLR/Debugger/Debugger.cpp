//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "../Include/nanoCLR_Runtime.h"
#include "../Include/nanoCLR_Debugging.h"
#include "../../HAL/Include/nanoHAL.h"
#include "../Include/WireProtocol.h"
#include "../Include/WireProtocol_Message.h"
#include "../Include/WireProtocol_MonitorCommands.h"
#include "Debugger.h"
#include "../CorLib/mscorlib/corlib_native.h"
#include "../Core/Core.h"

#define __min(a, b) (((a) < (b)) ? (a) : (b))

//--//

extern const CLR_RT_NativeAssemblyData *g_CLR_InteropAssembliesNativeData[];
extern uint16_t g_CLR_InteropAssembliesCount;

CLR_DBG_Debugger *g_CLR_DBG_Debugger;

// BlockStorageDevice *CLR_DBG_Debugger::m_deploymentStorageDevice = NULL;

//--//

void CLR_DBG_Debugger::Debugger_WaitForCommands()
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

#if !defined(BUILD_RTM)
  CLR_Debug::Printf("Waiting for debug commands...\r\n");
#endif

  while (!CLR_EE_DBG_IS(RebootPending) && !CLR_EE_DBG_IS(ExitPending))
    {
    g_CLR_RT_ExecutionEngine.DebuggerLoop();
    }
  }

void CLR_DBG_Debugger::Debugger_Discovery()
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_INT32 wait_sec = 5;

  CLR_INT64 expire = HAL_Time_CurrentTime() + (wait_sec * TIME_CONVERSION__TO_SECONDS);

  // Send "presence" ping.
  Monitor_Ping_Command cmd;
  cmd.Source = Monitor_Ping_c_Ping_Source_NanoCLR;

  while (true)
    {
    CLR_EE_DBG_EVENT_BROADCAST(
      CLR_DBG_Commands::c_Monitor_Ping,
      sizeof(cmd),
      &cmd,
      WP_Flags_c_NoCaching | WP_Flags_c_NonCritical);

    // if we support soft reboot and the debugger is not stopped then we don't need to connect the debugger
    if (!CLR_EE_DBG_IS(Stopped)  && ::CPU_IsSoftRebootSupported() )
      {
      break;
      }

    g_CLR_RT_ExecutionEngine.DebuggerLoop();

    if (CLR_EE_DBG_IS(Enabled))
      {
      // Debugger on the other side, let's exit the discovery loop.
      CLR_Debug::Printf("Debugger found. Resuming boot sequence.\r\n");
      break;
      }

    CLR_INT64 now = HAL_Time_CurrentTime();

    if (expire < now)
      {
      // no response after timeout...
      CLR_Debug::Printf("No debugger found...\r\n");
      break;
      }
    }

  g_CLR_RT_ExecutionEngine.WaitForDebugger();
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_DBG_Debugger::CreateInstance()
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  NANOCLR_HEADER();

  // alloc memory for debugger
  g_CLR_DBG_Debugger = (CLR_DBG_Debugger *)platform_malloc(sizeof(CLR_DBG_Debugger));

  // sanity check...
  FAULT_ON_NULL(g_CLR_DBG_Debugger);

  //... and clear memory
  memset(g_CLR_DBG_Debugger, 0, sizeof(CLR_DBG_Debugger));

  // alloc memory for debugger messaging
  g_CLR_DBG_Debugger->m_messaging = (CLR_Messaging *)platform_malloc(sizeof(CLR_Messaging));

  // sanity check...
  FAULT_ON_NULL(g_CLR_DBG_Debugger->m_messaging);

  //... and clear memory
  memset(g_CLR_DBG_Debugger->m_messaging, 0, sizeof(CLR_Messaging));

  NANOCLR_CHECK_HRESULT(g_CLR_DBG_Debugger->Debugger_Initialize(HalSystemConfig.DebuggerPort));


  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_DBG_Debugger::Debugger_Initialize(COM_HANDLE port)
  {
  (void)port;

  NATIVE_PROFILE_CLR_DEBUGGER();
  NANOCLR_HEADER();

  m_messaging->Initialize(
    c_Debugger_Lookup_Request,
    c_Debugger_Lookup_Request_count,
    c_Debugger_Lookup_Reply,
    c_Debugger_Lookup_Reply_count);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

//--//

HRESULT CLR_DBG_Debugger::DeleteInstance()
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  NANOCLR_HEADER();

  g_CLR_DBG_Debugger->Debugger_Cleanup();

  // free messaging
  platform_free(g_CLR_DBG_Debugger->m_messaging);

  // free debugger
  platform_free(g_CLR_DBG_Debugger);

  g_CLR_DBG_Debugger = NULL;

  NANOCLR_NOCLEANUP_NOLABEL();
  }

void CLR_DBG_Debugger::Debugger_Cleanup()
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  m_messaging->Cleanup();
  }

//--//

// void CLR_DBG_Debugger::ProcessCommands()
// {
//     NATIVE_PROFILE_CLR_DEBUGGER();
//     m_messaging->m_controller.AdvanceState();
// }

void CLR_DBG_Debugger::BroadcastEvent(
  unsigned int cmd,
  unsigned int payloadSize,
  unsigned char *payload,
  unsigned int flags)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  WP_PrepareAndSendProtocolMessage(cmd, payloadSize, payload, flags);
  }

//--//

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

#if defined(NANOCLR_APPDOMAINS)

CLR_RT_AppDomain *CLR_DBG_Debugger::GetAppDomainFromID(CLR_UINT32 id)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  NANOCLR_FOREACH_NODE(CLR_RT_AppDomain, appDomain, g_CLR_RT_ExecutionEngine.m_appDomains)
    {
    if (appDomain->m_id == id)
      return appDomain;
    }
  NANOCLR_FOREACH_NODE_END();

  return NULL;
  }
#endif // NANOCLR_APPDOMAINS

CLR_RT_Thread *CLR_DBG_Debugger::GetThreadFromPid(CLR_INT32 pid)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, g_CLR_RT_ExecutionEngine.m_threadsReady)
    {
    if (th->m_pid == pid)
      return th;
    }
  NANOCLR_FOREACH_NODE_END();

  NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, g_CLR_RT_ExecutionEngine.m_threadsWaiting)
    {
    if (th->m_pid == pid)
      return th;
    }
  NANOCLR_FOREACH_NODE_END();

  return NULL;
  }

HRESULT CLR_DBG_Debugger::CreateListOfThreads(CLR_DBG_Commands::Debugging_Thread_List::Reply *&cmdReply, int &totLen)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  NANOCLR_HEADER();

  CLR_UINT32 *pidDst;
  int num;

  num = g_CLR_RT_ExecutionEngine.m_threadsReady.NumOfNodes() + g_CLR_RT_ExecutionEngine.m_threadsWaiting.NumOfNodes();

  totLen = sizeof(*cmdReply) + (num - 1) * sizeof(CLR_UINT32);

  cmdReply = (CLR_DBG_Commands::Debugging_Thread_List::Reply *)CLR_RT_Memory::Allocate_And_Erase(totLen, true);
  CHECK_ALLOCATION(cmdReply);

  cmdReply->m_num = num;

  pidDst = cmdReply->m_pids;

  NANOCLR_FOREACH_NODE(CLR_RT_Thread, thSrc, g_CLR_RT_ExecutionEngine.m_threadsReady)
    {
    *pidDst++ = thSrc->m_pid;
    }
  NANOCLR_FOREACH_NODE_END();

  NANOCLR_FOREACH_NODE(CLR_RT_Thread, thSrc, g_CLR_RT_ExecutionEngine.m_threadsWaiting)
    {
    *pidDst++ = thSrc->m_pid;
    }
  NANOCLR_FOREACH_NODE_END();

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_DBG_Debugger::CreateListOfCalls(
  CLR_INT32 pid,
  CLR_DBG_Commands::Debugging_Thread_Stack::Reply *&cmdReply,
  int &totLen)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  NANOCLR_HEADER();

  CLR_RT_Thread *th = GetThreadFromPid(pid);
  FAULT_ON_NULL(th);

  for (int pass = 0; pass < 2; pass++)
    {
    int num = 0;

    NANOCLR_FOREACH_NODE(CLR_RT_StackFrame, call, th->m_stackFrames)
      {
      if (pass == 1)
        {
        int tmp = num;

#ifndef CLR_NO_IL_INLINE
        if (call->m_inlineFrame)
          {
          CLR_DBG_Commands::Debugging_Thread_Stack::Reply::Call &dst = cmdReply->m_data[tmp++];

          dst.m_md = call->m_inlineFrame->m_frame.m_call;
          dst.m_IP = (CLR_UINT32)(call->m_inlineFrame->m_frame.m_IP - call->m_inlineFrame->m_frame.m_IPStart);
#if defined(NANOCLR_APPDOMAINS)
          dst.m_appDomainID = call->m_appDomain->m_id;
          dst.m_flags = call->m_flags;
#endif
          }
#endif
        CLR_DBG_Commands::Debugging_Thread_Stack::Reply::Call &dst = cmdReply->m_data[tmp];

        dst.m_md = call->m_call;
        dst.m_IP = (CLR_UINT32)(call->m_IP - call->m_IPstart);

        if (dst.m_IP && call != th->CurrentFrame())
          {
          // With the exception of when the IP is 0, for a breakpoint on Push,
          // The call->m_IP is the next instruction to execute, not the currently executing one.
          // For non-leaf frames, this will return the IP within the call.
          dst.m_IP--;
          }

#if defined(NANOCLR_APPDOMAINS)
        dst.m_appDomainID = call->m_appDomain->m_id;
        dst.m_flags = call->m_flags;
#endif
        }

#ifndef CLR_NO_IL_INLINE
      if (call->m_inlineFrame)
        {
        num++;
        }
#endif

      num++;
      }
    NANOCLR_FOREACH_NODE_END();

    if (pass == 0)
      {
      totLen = sizeof(*cmdReply) + (num - 1) * sizeof(CLR_DBG_Commands::Debugging_Thread_Stack::Reply::Call);

      cmdReply = (CLR_DBG_Commands::Debugging_Thread_Stack::Reply *)CLR_RT_Memory::Allocate_And_Erase(
        totLen,
        CLR_RT_HeapBlock::HB_CompactOnFailure);
      CHECK_ALLOCATION(cmdReply);

      cmdReply->m_num = num;
      cmdReply->m_status = th->m_status;
      cmdReply->m_flags = th->m_flags;
      }
    }

  NANOCLR_NOCLEANUP();
  }

#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CLR_DBG_Debugger::Monitor_Ping(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  bool fStopOnBoot = true;

  //
  // There's someone on the other side!!
  //
  CLR_EE_DBG_SET(Enabled);

  if ((msg->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
    Monitor_Ping_Reply cmdReply;
    Monitor_Ping_Command *cmd = (Monitor_Ping_Command *)msg->m_payload;

    // default is to stop the debugger (backwards compatibility)
    fStopOnBoot = (cmd != NULL) && (cmd->Flags & Monitor_Ping_c_Ping_DbgFlag_Stop);

    cmdReply.Source = Monitor_Ping_c_Ping_Source_NanoCLR;

    // now fill in the flags
    cmdReply.Flags = CLR_EE_DBG_IS(StateProgramExited) != 0 ? Monitor_Ping_c_Ping_DbgFlag_AppExit : 0;

#if defined(WP_IMPLEMENTS_CRC32)
    cmdReply.Flags |= Monitor_Ping_c_Ping_WPFlag_SupportsCRC32;
#endif

    // Wire Protocol packet size
#if (WP_PACKET_SIZE == 512)
    cmdReply.Flags |= Monitor_Ping_c_PacketSize_0512;
#elif (WP_PACKET_SIZE == 256)
    cmdReply.Flags |= Monitor_Ping_c_PacketSize_0256;
#elif (WP_PACKET_SIZE == 128)
    cmdReply.Flags |= Monitor_Ping_c_PacketSize_0128;
#elif (WP_PACKET_SIZE == 1024)
    cmdReply.Flags |= Monitor_Ping_c_PacketSize_1024;
#endif

    // done, send reply
    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));
    }
  else
    {
    Monitor_Ping_Reply *cmdReply = (Monitor_Ping_Reply *)msg->m_payload;

    // default is to stop the debugger (backwards compatibility)
    fStopOnBoot = (cmdReply != NULL) && (cmdReply->Flags & Monitor_Ping_c_Ping_DbgFlag_Stop);
    }

  if (CLR_EE_DBG_IS_MASK(StateInitialize, StateMask))
    {
    if (fStopOnBoot)
      CLR_EE_DBG_SET(Stopped);
    else
      CLR_EE_DBG_CLR(Stopped);
    }

  return true;
  }

bool CLR_DBG_Debugger::Monitor_Execute(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Monitor_Execute *cmd = (CLR_DBG_Commands::Monitor_Execute *)msg->m_payload;

#if defined(BUILD_RTM)
  if (!DebuggerPort_IsUsingSsl(HalSystemConfig.DebuggerPort))
    return false;
#endif

  WP_ReplyToCommand(msg, true, false, NULL, 0);

  ((void (*)())(size_t)cmd->m_address)();

  return true;
  }

bool CLR_DBG_Debugger::Monitor_ReadMemory(WP_Message *msg)
  {
  return true;
  }

bool CLR_DBG_Debugger::Monitor_WriteMemory(WP_Message *msg)
  {
  return true;
  }


bool CLR_DBG_Debugger::Monitor_Reboot(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  bool success = true;

  CLR_DBG_Commands::Monitor_Reboot *cmd = (CLR_DBG_Commands::Monitor_Reboot *)msg->m_payload;

  if (NULL != cmd)
    {
    g_CLR_RT_ExecutionEngine.m_iReboot_Options = cmd->m_flags;
    }

  CPU_Sleep(); // give message a little time to be flushed

  WP_ReplyToCommand(msg, success, false, NULL, 0);

  CPU_Sleep(); // give message a little time to be flushed

  CLR_EE_DBG_SET(RebootPending);

  return true;
  }

bool CLR_DBG_Debugger::Monitor_MemoryMap(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

#ifndef _WIN32
  MemoryMap_Range map[2];

  map[0].m_address = HalSystemConfig.RAM1.Base;
  map[0].m_length = HalSystemConfig.RAM1.Size;
  map[0].m_flags = Monitor_MemoryMap_c_RAM;

  map[1].m_address = HalSystemConfig.FLASH1.Base;
  map[1].m_length = HalSystemConfig.FLASH1.Size;
  map[1].m_flags = Monitor_MemoryMap_c_FLASH;

  WP_ReplyToCommand(msg, true, false, map, sizeof(map));
#endif

  return true;
  }
bool CLR_DBG_Debugger::Debugging_Execution_BasePtr(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Execution_BasePtr::Reply cmdReply;

  cmdReply.m_EE = (CLR_UINT32)(size_t)&g_CLR_RT_ExecutionEngine;

  WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Execution_ChangeConditions(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Execution_ChangeConditions *cmd =
    (CLR_DBG_Commands::Debugging_Execution_ChangeConditions *)msg->m_payload;

  g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions |= cmd->FlagsToSet;
  g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions &= ~cmd->FlagsToReset;

  // updating the debugging execution conditions requires sometime to propagate
  // make sure we allow enough time for that to happen
  OS_DELAY(100);

  if ((msg->m_header.m_flags & WP_Flags_c_NonCritical) == 0)
    {
    CLR_DBG_Commands::Debugging_Execution_ChangeConditions::Reply cmdReply;

    cmdReply.CurrentState = g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions;

    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));
    }

  return true;
  }

//--//

static void GetClrReleaseInfo(CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::ClrInfo &clrInfo)
  {
  NFReleaseInfo::Init(
    clrInfo.m_clrReleaseInfo,
    VERSION_MAJOR,
    VERSION_MINOR,
    VERSION_BUILD,
    VERSION_REVISION,
    OEMSYSTEMINFOSTRING,
    ARRAYSIZE(OEMSYSTEMINFOSTRING),
    TARGETNAMESTRING,
    ARRAYSIZE(TARGETNAMESTRING),
    PLATFORMNAMESTRING,
    ARRAYSIZE(PLATFORMNAMESTRING));

  if (g_CLR_RT_TypeSystem.m_assemblyMscorlib && g_CLR_RT_TypeSystem.m_assemblyMscorlib->m_header)
    {
    const CLR_RECORD_VERSION *mscorlibVer = &(g_CLR_RT_TypeSystem.m_assemblyMscorlib->m_header->version);
    NFVersion::Init(
      clrInfo.m_TargetFrameworkVersion,
      mscorlibVer->iMajorVersion,
      mscorlibVer->iMinorVersion,
      mscorlibVer->iBuildNumber,
      mscorlibVer->iRevisionNumber);
    }
  else
    {
    NFVersion::Init(clrInfo.m_TargetFrameworkVersion, 0, 0, 0, 0);
    }
  }

static bool GetInteropNativeAssemblies(uint8_t *&data, int *size, uint32_t startIndex, uint32_t count)
  {
  uint32_t index = 0;

  CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::NativeAssemblyDetails *interopNativeAssemblies = NULL;

  // sanity checks on the requested size
  // - if 0, adjust to the assemblies count to make the execution backwards compatible
  // - trim if over the available assembly count
  // (max possible page size is 255)
  if (startIndex == 0 && count == 0)
    {
    // adjust to the assemblies count to make the execution backwards compatible
    count = g_CLR_InteropAssembliesCount;
    }
  else if (count > 255 || count + startIndex > g_CLR_InteropAssembliesCount)
    {
    // adjust to the assemblies count so it doesn't overflow
    count = g_CLR_InteropAssembliesCount - startIndex;
    }

  // alloc buffer to hold the requested number of assemblies
  interopNativeAssemblies =
    (CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::NativeAssemblyDetails *)platform_malloc(
      sizeof(CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::NativeAssemblyDetails) * count);

  // check for malloc failure
  if (interopNativeAssemblies == NULL)
    {
    return false;
    }

  // clear buffer memory
  memset(
    interopNativeAssemblies,
    0,
    sizeof(CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::NativeAssemblyDetails) * count);

  // fill the array
  for (uint32_t i = 0; i < g_CLR_InteropAssembliesCount; i++)
    {
    // check if the assembly at this position it's on the requested range
    if (i >= startIndex && i < (startIndex + count))
      {
      interopNativeAssemblies[index].CheckSum = g_CLR_InteropAssembliesNativeData[i]->m_checkSum;
      hal_strcpy_s(
        (char *)interopNativeAssemblies[index].AssemblyName,
        ARRAYSIZE(interopNativeAssemblies[index].AssemblyName),
        g_CLR_InteropAssembliesNativeData[i]->m_szAssemblyName);

      NFVersion::Init(
        interopNativeAssemblies[index].Version,
        g_CLR_InteropAssembliesNativeData[i]->m_Version.iMajorVersion,
        g_CLR_InteropAssembliesNativeData[i]->m_Version.iMinorVersion,
        g_CLR_InteropAssembliesNativeData[i]->m_Version.iBuildNumber,
        g_CLR_InteropAssembliesNativeData[i]->m_Version.iRevisionNumber);

      index++;
      }
    }

  // copy back the buffer
  data = (uint8_t *)interopNativeAssemblies;

  // set buffer size
  *size = (sizeof(CLR_DBG_Commands::Debugging_Execution_QueryCLRCapabilities::NativeAssemblyDetails) * count);

  return true;
  }


bool CLR_DBG_Debugger::Debugging_Execution_Allocate(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Execution_Allocate *cmd =
    (CLR_DBG_Commands::Debugging_Execution_Allocate *)msg->m_payload;
  CLR_DBG_Commands::Debugging_Execution_Allocate::Reply reply;

  reply.m_address = (CLR_UINT32)(size_t)CLR_RT_Memory::Allocate(cmd->m_size, CLR_RT_HeapBlock::HB_CompactOnFailure);

  if (!reply.m_address)
    return false;

  WP_ReplyToCommand(msg, true, false, &reply, sizeof(reply));

  return true;
  }

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

static bool FillValues(
  CLR_RT_HeapBlock *ptr,
  CLR_DBG_Commands::Debugging_Value *&array,
  size_t num,
  CLR_RT_HeapBlock *reference,
  CLR_RT_TypeDef_Instance *pTD)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  if (!ptr)
    return true;

  if (!array || num == 0)
    return false;

  CLR_DBG_Commands::Debugging_Value *dst = array++;
  num--;
  CLR_RT_TypeDescriptor desc;

  memset(dst, 0, sizeof(*dst));

  dst->m_referenceID = (reference != NULL) ? reference : ptr;
  dst->m_dt = ptr->DataType();
  dst->m_flags = ptr->DataFlags();
  dst->m_size = ptr->DataSize();

  if (pTD != NULL)
    {
    dst->m_td = *pTD;
    }
  else if (SUCCEEDED(desc.InitializeFromObject(*ptr)))
    {
    dst->m_td = desc.m_handlerCls;
    }

  switch (dst->m_dt)
    {
    case DATATYPE_BOOLEAN:
    case DATATYPE_I1:
    case DATATYPE_U1:

    case DATATYPE_CHAR:
    case DATATYPE_I2:
    case DATATYPE_U2:

    case DATATYPE_I4:
    case DATATYPE_U4:
    case DATATYPE_R4:

    case DATATYPE_I8:
    case DATATYPE_U8:
    case DATATYPE_R8:
    case DATATYPE_DATETIME:
    case DATATYPE_TIMESPAN:
    case DATATYPE_REFLECTION:
      //
      // Primitives or optimized value types.
      //
      memcpy(dst->m_builtinValue, (void *)&ptr->NumericByRefConst().u1, 8);
      break;

    case DATATYPE_STRING:
    {
    const char *text = ptr->StringText();

    if (text != NULL)
      {
      dst->m_charsInString = text;
      dst->m_bytesInString = (CLR_UINT32)hal_strlen_s(text);

      hal_strncpy_s(
        (char *)dst->m_builtinValue,
        ARRAYSIZE(dst->m_builtinValue),
        text,
        __min(dst->m_bytesInString, MAXSTRLEN(dst->m_builtinValue)));
      }
    else
      {
      dst->m_charsInString = NULL;
      dst->m_bytesInString = 0;
      dst->m_builtinValue[0] = 0;
      }
    }
    break;

    case DATATYPE_OBJECT:
    case DATATYPE_BYREF:
      return FillValues(ptr->Dereference(), array, num, NULL, pTD);

    case DATATYPE_CLASS:
    case DATATYPE_VALUETYPE:
      dst->m_td = ptr->ObjectCls();
      break;

    case DATATYPE_SZARRAY:
    {
    CLR_RT_HeapBlock_Array *ptr2 = (CLR_RT_HeapBlock_Array *)ptr;

    dst->m_array_numOfElements = ptr2->m_numOfElements;
    dst->m_array_depth = ptr2->ReflectionDataConst().m_levels;
    dst->m_array_typeIndex = ptr2->ReflectionDataConst().m_data.m_type;
    }
    break;

    ////////////////////////////////////////

    case DATATYPE_WEAKCLASS:
      break;

    case DATATYPE_ARRAY_BYREF:
      dst->m_arrayref_referenceID = ptr->Array();
      dst->m_arrayref_index = ptr->ArrayIndex();

      break;

    case DATATYPE_DELEGATE_HEAD:
      break;

    case DATATYPE_DELEGATELIST_HEAD:
      break;

    case DATATYPE_FREEBLOCK:
    case DATATYPE_CACHEDBLOCK:
    case DATATYPE_ASSEMBLY:
    case DATATYPE_OBJECT_TO_EVENT:
    case DATATYPE_BINARY_BLOB_HEAD:

    case DATATYPE_THREAD:
    case DATATYPE_SUBTHREAD:
    case DATATYPE_STACK_FRAME:
    case DATATYPE_TIMER_HEAD:
    case DATATYPE_LOCK_HEAD:
    case DATATYPE_LOCK_OWNER_HEAD:
    case DATATYPE_LOCK_REQUEST_HEAD:
    case DATATYPE_WAIT_FOR_OBJECT_HEAD:
    case DATATYPE_FINALIZER_HEAD:
    case DATATYPE_MEMORY_STREAM_HEAD:
    case DATATYPE_MEMORY_STREAM_DATA:

    case DATATYPE_SERIALIZER_HEAD:
    case DATATYPE_SERIALIZER_DUPLICATE:
    case DATATYPE_SERIALIZER_STATE:

    case DATATYPE_ENDPOINT_HEAD:

#if defined(NANOCLR_APPDOMAINS)
    case DATATYPE_APPDOMAIN_HEAD:
    case DATATYPE_TRANSPARENT_PROXY:
    case DATATYPE_APPDOMAIN_ASSEMBLY:
#endif

      break;
    }

  return true;
  }

bool CLR_DBG_Debugger::GetValue(
  WP_Message *msg,
  CLR_RT_HeapBlock *ptr,
  CLR_RT_HeapBlock *reference,
  CLR_RT_TypeDef_Instance *pTD)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  CLR_DBG_Commands::Debugging_Value reply[4];
  CLR_DBG_Commands::Debugging_Value *array = reply;

  if (FillValues(ptr, array, ARRAYSIZE(reply), reference, pTD))
    {
    WP_ReplyToCommand(msg, true, false, reply, (int)((size_t)array - (size_t)reply));

    return true;
    }

  WP_ReplyToCommand(msg, false, false, NULL, 0);

  return false;
  }

//--//

bool CLR_DBG_Debugger::Debugging_Execution_SetCurrentAppDomain(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
#if defined(NANOCLR_APPDOMAINS)

  CLR_DBG_Commands::Debugging_Execution_SetCurrentAppDomain *cmd =
    (CLR_DBG_Commands::Debugging_Execution_SetCurrentAppDomain *)msg->m_payload;
  CLR_RT_AppDomain *appDomain = g_CLR_DBG_Debugger->GetAppDomainFromID(cmd->m_id);

  if (appDomain)
    {
    g_CLR_RT_ExecutionEngine.SetCurrentAppDomain(appDomain);
    }

  WP_ReplyToCommand(msg, appDomain != NULL, false, NULL, 0);

  return true;
#else
  (void)msg;
  return false;
#endif
  }

bool CLR_DBG_Debugger::Debugging_Execution_Breakpoints(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Execution_Breakpoints *cmd =
    (CLR_DBG_Commands::Debugging_Execution_Breakpoints *)msg->m_payload;

  g_CLR_RT_ExecutionEngine.InstallBreakpoints(
    cmd->m_data,
    (msg->m_header.m_size - sizeof(cmd->m_flags)) / sizeof(CLR_DBG_Commands::Debugging_Execution_BreakpointDef));

  WP_ReplyToCommand(msg, true, false, NULL, 0);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Execution_BreakpointStatus(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Execution_BreakpointStatus::Reply reply;

  if (g_CLR_RT_ExecutionEngine.DequeueActiveBreakpoint(reply.m_lastHit) == false)
    {
    memset(&reply.m_lastHit, 0, sizeof(reply.m_lastHit));
    }

  WP_ReplyToCommand(msg, true, false, &reply, sizeof(reply));

  return true;
  }

//--//

CLR_RT_Assembly *CLR_DBG_Debugger::IsGoodAssembly(CLR_IDX idxAssm)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  NANOCLR_FOREACH_ASSEMBLY(g_CLR_RT_TypeSystem)
    {
    if (pASSM->m_idx == idxAssm)
      return pASSM;
    }
  NANOCLR_FOREACH_ASSEMBLY_END();

  return NULL;
  }

bool CLR_DBG_Debugger::CheckTypeDef(const CLR_RT_TypeDef_Index &td, CLR_RT_TypeDef_Instance &inst)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  CLR_RT_Assembly *assm = IsGoodAssembly(td.Assembly());

  if (assm && td.Type() < assm->m_pTablesSize[TBL_TypeDef])
    {
    return inst.InitializeFromIndex(td);
    }

  return false;
  }

bool CLR_DBG_Debugger::CheckFieldDef(const CLR_RT_FieldDef_Index &fd, CLR_RT_FieldDef_Instance &inst)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  CLR_RT_Assembly *assm = IsGoodAssembly(fd.Assembly());

  if (assm && fd.Field() < assm->m_pTablesSize[TBL_FieldDef])
    {
    return inst.InitializeFromIndex(fd);
    }

  return false;
  }

bool CLR_DBG_Debugger::CheckMethodDef(const CLR_RT_MethodDef_Index &md, CLR_RT_MethodDef_Instance &inst)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  CLR_RT_Assembly *assm = IsGoodAssembly(md.Assembly());

  if (assm && md.Method() < assm->m_pTablesSize[TBL_MethodDef])
    {
    return inst.InitializeFromIndex(md);
    }

  return false;
  }

CLR_RT_StackFrame *CLR_DBG_Debugger::CheckStackFrame(CLR_INT32 pid, CLR_UINT32 depth, bool &isInline)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  CLR_RT_Thread *th = GetThreadFromPid(pid);

  isInline = false;

  if (th)
    {
    NANOCLR_FOREACH_NODE(CLR_RT_StackFrame, call, th->m_stackFrames)
      {
#ifndef CLR_NO_IL_INLINE
      if (call->m_inlineFrame)
        {
        if (depth-- == 0)
          {
          isInline = true;
          return call;
          }
        }
#endif

      if (depth-- == 0)
        return call;
      }
    NANOCLR_FOREACH_NODE_END();
    }

  return NULL;
  }

//--//

static HRESULT Debugging_Thread_Create_Helper(CLR_RT_MethodDef_Index &md, CLR_RT_Thread *&th, CLR_INT32 pid)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  NANOCLR_HEADER();

  CLR_RT_HeapBlock ref;
  ref.SetObjectReference(NULL);
  CLR_RT_ProtectFromGC gc(ref);
  CLR_RT_Thread *realThread = (pid != 0) ? CLR_DBG_Debugger::GetThreadFromPid(pid) : NULL;

  th = NULL;

  NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Delegate::CreateInstance(ref, md, NULL));

  NANOCLR_CHECK_HRESULT(
    g_CLR_RT_ExecutionEngine.NewThread(th, ref.DereferenceDelegate(), ThreadPriority_Highest, -1));

  if (realThread)
    {
    th->m_realThread = realThread;
    }

  {
  CLR_RT_StackFrame *stack = th->CurrentFrame();
  const CLR_RECORD_METHODDEF *target = stack->m_call.m_target;
  CLR_UINT8 numArgs = target->numArgs;

  if (numArgs)
    {
    CLR_RT_SignatureParser parser;
    parser.Initialize_MethodSignature(stack->m_call.m_assm, target);
    CLR_RT_SignatureParser::Element res;
    CLR_RT_HeapBlock *args = stack->m_arguments;

    if (parser.m_flags & PIMAGE_CEE_CS_CALLCONV_HASTHIS)
      {
      args->SetObjectReference(NULL);

      numArgs--;
      args++;
      }

    //
    // Skip return value.
    //
    NANOCLR_CHECK_HRESULT(parser.Advance(res));

    //
    // None of the arguments can be ByRef.
    //
    {
    CLR_RT_SignatureParser parser2 = parser;

    for (; parser2.Available() > 0;)
      {
      NANOCLR_CHECK_HRESULT(parser2.Advance(res));

      if (res.m_fByRef)
        {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }
      }
    }

    for (CLR_UINT8 i = 0; i < numArgs; i++, args++)
      {
      NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.InitializeReference(*args, parser));
      }
    }
  }

  NANOCLR_CLEANUP();

  if (FAILED(hr))
    {
    if (th)
      {
      th->Terminate();
      th = NULL;
      }
    }

  NANOCLR_CLEANUP_END();
  }

bool CLR_DBG_Debugger::Debugging_Thread_CreateEx(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Thread_CreateEx *cmd = (CLR_DBG_Commands::Debugging_Thread_CreateEx *)msg->m_payload;
  CLR_DBG_Commands::Debugging_Thread_CreateEx::Reply cmdReply;
  CLR_RT_Thread *th;

  if (SUCCEEDED(Debugging_Thread_Create_Helper(cmd->m_md, th, cmd->m_pid)))
    {
    th->m_scratchPad = cmd->m_scratchPad;

    cmdReply.m_pid = th->m_pid;
    }
  else
    {
    cmdReply.m_pid = -1;
    }

  WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Thread_List(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Thread_List::Reply *cmdReply = NULL;
  int len = 0;

  if (FAILED(g_CLR_DBG_Debugger->CreateListOfThreads(cmdReply, len)))
    {
    WP_ReplyToCommand(msg, false, false, NULL, 0);
    }
  else
    {
    WP_ReplyToCommand(msg, true, false, cmdReply, len);
    }

  CLR_RT_Memory::Release(cmdReply);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Thread_Stack(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Thread_Stack *cmd = (CLR_DBG_Commands::Debugging_Thread_Stack *)msg->m_payload;
  CLR_DBG_Commands::Debugging_Thread_Stack::Reply *cmdReply = NULL;
  int len = 0;

  if (FAILED(g_CLR_DBG_Debugger->CreateListOfCalls(cmd->m_pid, cmdReply, len)))
    {
    WP_ReplyToCommand(msg, false, false, NULL, 0);
    }
  else
    {
    WP_ReplyToCommand(msg, true, false, cmdReply, len);
    }

  CLR_RT_Memory::Release(cmdReply);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Thread_Kill(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Thread_Kill *cmd = (CLR_DBG_Commands::Debugging_Thread_Kill *)msg->m_payload;
  CLR_DBG_Commands::Debugging_Thread_Kill::Reply cmdReply;
  CLR_RT_Thread *th = g_CLR_DBG_Debugger->GetThreadFromPid(cmd->m_pid);

  if (th)
    {
    th->Terminate();

    cmdReply.m_result = 1;
    }
  else
    {
    cmdReply.m_result = 0;
    }

  WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Thread_Suspend(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Thread_Suspend *cmd = (CLR_DBG_Commands::Debugging_Thread_Suspend *)msg->m_payload;
  CLR_RT_Thread *th = g_CLR_DBG_Debugger->GetThreadFromPid(cmd->m_pid);

  if (th)
    {
    th->Suspend();
    }

  WP_ReplyToCommand(msg, th != NULL, false, NULL, 0);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Thread_Resume(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Thread_Resume *cmd = (CLR_DBG_Commands::Debugging_Thread_Resume *)msg->m_payload;
  CLR_RT_Thread *th = g_CLR_DBG_Debugger->GetThreadFromPid(cmd->m_pid);

  if (th)
    {
    th->Resume();
    }

  WP_ReplyToCommand(msg, th != NULL, false, NULL, 0);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Thread_Get(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  CLR_DBG_Debugger *dbg = g_CLR_DBG_Debugger;
  CLR_DBG_Commands::Debugging_Thread_Get *cmd = (CLR_DBG_Commands::Debugging_Thread_Get *)msg->m_payload;
  CLR_RT_Thread *th = dbg->GetThreadFromPid(cmd->m_pid);
  CLR_RT_HeapBlock *pThread = 0;
  bool fFound = false;

  if (th == NULL)
    return false;

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  // If we are a thread spawned by the debugger to perform evaluations,
  // return the thread object that correspond to thread that has focus in debugger.
  th = th->m_realThread;
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

  // Find an existing managed thread, if it exists
  // making sure to only return the managed object association with the current appdomain
  // to prevent leaking of managed Thread objects across AD boundaries.

  NANOCLR_FOREACH_NODE(CLR_RT_ObjectToEvent_Source, src, th->m_references)
    {
    CLR_RT_HeapBlock *pManagedThread = src->m_objectPtr;
    _ASSERTE(pManagedThread != NULL);

#if defined(NANOCLR_APPDOMAINS)
    {
    CLR_RT_ObjectToEvent_Source *appDomainSrc = CLR_RT_ObjectToEvent_Source::ExtractInstance(
      pManagedThread[Library_corlib_native_System_Threading_Thread::FIELD___appDomain]);

    if (appDomainSrc == NULL)
      break;

    fFound = (appDomainSrc->m_eventPtr == g_CLR_RT_ExecutionEngine.GetCurrentAppDomain());
    }
#else
    fFound = true;
#endif

    if (fFound)
      {
      pThread = pManagedThread;

      break;
      }
    }
  NANOCLR_FOREACH_NODE_END();

  if (!fFound)
    {
    pThread = (CLR_RT_HeapBlock *)platform_malloc(sizeof(CLR_RT_HeapBlock));

    // Create the managed thread.
    // This implies that there is no state in the managed object.  This is not exactly true, as the managed thread
    // contains the priority as well as the delegate to start.  However, that state is really just used as a
    // placeholder for the data before the thread is started.  Once the thread is started, they are copied over to
    // the unmanaged thread object and no longer used.  The managed object is then used simply as a wrapper for the
    // unmanaged thread.  Therefore, it is safe to simply make another managed thread here.
    if (SUCCEEDED(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*pThread, g_CLR_RT_WellKnownTypes.m_Thread)))
      {
      CLR_RT_HeapBlock *pRes = pThread->Dereference();

      int pri = th->GetThreadPriority();

      pRes[Library_corlib_native_System_Threading_Thread::FIELD___priority].NumericByRef().s4 = pri;

      if (SUCCEEDED(CLR_RT_ObjectToEvent_Source::CreateInstance(
        th,
        *pRes,
        pRes[Library_corlib_native_System_Threading_Thread::FIELD___thread])))
        {
#if defined(NANOCLR_APPDOMAINS)
        CLR_RT_ObjectToEvent_Source::CreateInstance(
          g_CLR_RT_ExecutionEngine.GetCurrentAppDomain(),
          *pRes,
          pRes[Library_corlib_native_System_Threading_Thread::FIELD___appDomain]);
#endif
        fFound = true;
        }
      }
    }

  if (!fFound)
    return false;

  return g_CLR_DBG_Debugger->GetValue(msg, pThread, NULL, NULL);
  }

bool CLR_DBG_Debugger::Debugging_Thread_GetException(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Thread_GetException *cmd =
    (CLR_DBG_Commands::Debugging_Thread_GetException *)msg->m_payload;
  CLR_RT_Thread *th = g_CLR_DBG_Debugger->GetThreadFromPid(cmd->m_pid);
  CLR_RT_HeapBlock *blk = NULL;

  if (th)
    {
    blk = &th->m_currentException;
    }

  return g_CLR_DBG_Debugger->GetValue(msg, blk, NULL, NULL);
  }

bool CLR_DBG_Debugger::Debugging_Thread_Unwind(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Thread_Unwind *cmd = (CLR_DBG_Commands::Debugging_Thread_Unwind *)msg->m_payload;
  CLR_RT_StackFrame *call;
  CLR_RT_Thread *th;
  bool isInline = false;

  if ((call = g_CLR_DBG_Debugger->CheckStackFrame(cmd->m_pid, cmd->m_depth, isInline)) != NULL)
    {
    _ASSERTE((call->m_flags & CLR_RT_StackFrame::c_MethodKind_Native) == 0);

    th = call->m_owningThread;
    _ASSERTE(th->m_nestedExceptionsPos);

    CLR_RT_Thread::UnwindStack &us = th->m_nestedExceptions[th->m_nestedExceptionsPos - 1];
    _ASSERTE(th->m_currentException.Dereference() == us.m_exception);
    _ASSERTE(us.m_flags & CLR_RT_Thread::UnwindStack::c_ContinueExceptionHandler);

    us.m_handlerStack = call;
    us.m_flags |= CLR_RT_Thread::UnwindStack::c_MagicCatchForInteceptedException;

    us.SetPhase(CLR_RT_Thread::UnwindStack::p_2_RunningFinallys_0);
    }

  return true;
  }

//--//

bool CLR_DBG_Debugger::Debugging_Stack_Info(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Stack_Info *cmd = (CLR_DBG_Commands::Debugging_Stack_Info *)msg->m_payload;
  CLR_DBG_Commands::Debugging_Stack_Info::Reply cmdReply;
  CLR_RT_StackFrame *call;
  bool isInline = false;

  if ((call = g_CLR_DBG_Debugger->CheckStackFrame(cmd->m_pid, cmd->m_depth, isInline)) != NULL)
    {
#ifndef CLR_NO_IL_INLINE
    if (isInline)
      {
      cmdReply.m_md = call->m_inlineFrame->m_frame.m_call;
      cmdReply.m_IP = (CLR_UINT32)(call->m_inlineFrame->m_frame.m_IP - call->m_inlineFrame->m_frame.m_IPStart);
      cmdReply.m_numOfArguments = call->m_inlineFrame->m_frame.m_call.m_target->numArgs;
      cmdReply.m_numOfLocals = call->m_inlineFrame->m_frame.m_call.m_target->numLocals;
      cmdReply.m_depthOfEvalStack = (CLR_UINT32)(call->m_evalStack - call->m_inlineFrame->m_frame.m_evalStack);
      }
    else
#endif
      {
      cmdReply.m_md = call->m_call;
      cmdReply.m_IP = (CLR_UINT32)(call->m_IP - call->m_IPstart);
      cmdReply.m_numOfArguments = call->m_call.m_target->numArgs;
      cmdReply.m_numOfLocals = call->m_call.m_target->numLocals;
      cmdReply.m_depthOfEvalStack = (CLR_UINT32)call->TopValuePosition();
      }

    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

    return true;
    }

  WP_ReplyToCommand(msg, false, false, NULL, 0);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Stack_SetIP(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Stack_SetIP *cmd = (CLR_DBG_Commands::Debugging_Stack_SetIP *)msg->m_payload;
  CLR_RT_StackFrame *call;
  bool isInline = false;

  if ((call = g_CLR_DBG_Debugger->CheckStackFrame(cmd->m_pid, cmd->m_depth, isInline)) != NULL)
    {
#ifndef CLR_NO_IL_INLINE
    if (isInline)
      {
      WP_ReplyToCommand(msg, false, false, NULL, 0);

      return true;
      }
    else
#endif
      {
      call->m_IP = call->m_IPstart + cmd->m_IP;
      call->m_evalStackPos = call->m_evalStack + cmd->m_depthOfEvalStack;
      }

    call->m_flags &= ~CLR_RT_StackFrame::c_InvalidIP;

    WP_ReplyToCommand(msg, true, false, NULL, 0);

    return true;
    }

  WP_ReplyToCommand(msg, false, false, NULL, 0);

  return true;
  }

//--//

static bool IsBlockEnumMaybe(CLR_RT_HeapBlock *blk)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  const CLR_UINT32 c_MaskForPrimitive = CLR_RT_DataTypeLookup::c_Integer | CLR_RT_DataTypeLookup::c_Numeric;

  CLR_RT_TypeDescriptor desc;

  if (FAILED(desc.InitializeFromObject(*blk)))
    return false;

  const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[desc.m_handlerCls.m_target->dataType];

  return (dtl.m_flags & c_MaskForPrimitive) == c_MaskForPrimitive;
  }

static bool SetBlockHelper(CLR_RT_HeapBlock *blk, CLR_DataType dt, CLR_UINT8 *builtinValue)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  bool fCanAssign = false;

  if (blk)
    {
    CLR_DataType dtDst;
    CLR_RT_HeapBlock src;

    dtDst = blk->DataType();

    src.SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(dt, 0, 1));
    memcpy((void *)&src.NumericByRef().u1, builtinValue, sizeof(CLR_UINT64));

    if (dtDst == dt)
      {
      fCanAssign = true;
      }
    else
      {
      if (dt == DATATYPE_REFLECTION)
        {
        fCanAssign = (dtDst == DATATYPE_OBJECT && blk->Dereference() == NULL);
        }
      else if (dt == DATATYPE_OBJECT)
        {
        fCanAssign = (src.Dereference() == NULL && dtDst == DATATYPE_REFLECTION);
        }
      else
        {
        _ASSERTE(c_CLR_RT_DataTypeLookup[dtDst].m_flags & CLR_RT_DataTypeLookup::c_Numeric);

        if (c_CLR_RT_DataTypeLookup[dtDst].m_sizeInBytes == sizeof(CLR_INT32) &&
          c_CLR_RT_DataTypeLookup[dt].m_sizeInBytes < sizeof(CLR_INT32))
          {
          dt = dtDst;
          fCanAssign = true;
          }
        }
      }

    if (fCanAssign)
      {
      blk->ChangeDataType(dt);
      memcpy((void *)&blk->NumericByRef().u1, builtinValue, sizeof(CLR_UINT64));
      }
    }

  return fCanAssign;
  }

static CLR_RT_HeapBlock *GetScratchPad_Helper(int idx)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  CLR_RT_HeapBlock_Array *array = g_CLR_RT_ExecutionEngine.m_scratchPadArray;
  CLR_RT_HeapBlock tmp;
  CLR_RT_HeapBlock ref;

  tmp.SetObjectReference(array);

  if (SUCCEEDED(ref.InitializeArrayReference(tmp, idx)))
    {
    return (CLR_RT_HeapBlock *)array->GetElement(idx);
    }

  return NULL;
  }

//--//

bool CLR_DBG_Debugger::Debugging_Value_ResizeScratchPad(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Value_ResizeScratchPad *cmd =
    (CLR_DBG_Commands::Debugging_Value_ResizeScratchPad *)msg->m_payload;
  CLR_RT_HeapBlock ref;
  bool fRes = true;

  if (cmd->m_size == 0)
    {
    g_CLR_RT_ExecutionEngine.m_scratchPadArray = NULL;
    }
  else
    {
    if (SUCCEEDED(CLR_RT_HeapBlock_Array::CreateInstance(ref, cmd->m_size, g_CLR_RT_WellKnownTypes.m_Object)))
      {
      CLR_RT_HeapBlock_Array *pOld = g_CLR_RT_ExecutionEngine.m_scratchPadArray;
      CLR_RT_HeapBlock_Array *pNew = ref.DereferenceArray();

      if (pOld)
        {
        memcpy(
          pNew->GetFirstElement(),
          pOld->GetFirstElement(),
          sizeof(CLR_RT_HeapBlock) * __min(pNew->m_numOfElements, pOld->m_numOfElements));
        }

      g_CLR_RT_ExecutionEngine.m_scratchPadArray = pNew;
      }
    else
      {
      fRes = false;
      }
    }

  WP_ReplyToCommand(msg, fRes, false, NULL, 0);

  return false;
  }

bool CLR_DBG_Debugger::Debugging_Value_GetStack(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Value_GetStack *cmd = (CLR_DBG_Commands::Debugging_Value_GetStack *)msg->m_payload;
  CLR_RT_StackFrame *call;
  bool isInline = false;

  if ((call = g_CLR_DBG_Debugger->CheckStackFrame(cmd->m_pid, cmd->m_depth, isInline)) != NULL)
    {
    CLR_RT_HeapBlock *array;
    CLR_UINT32 num;
#ifndef CLR_NO_IL_INLINE
    CLR_RT_MethodDef_Instance &md = isInline ? call->m_inlineFrame->m_frame.m_call : call->m_call;
#else
    CLR_RT_MethodDef_Instance &md = call->m_call;
#endif

    switch (cmd->m_kind)
      {
      case CLR_DBG_Commands::Debugging_Value_GetStack::c_Argument:
#ifndef CLR_NO_IL_INLINE
        array = isInline ? call->m_inlineFrame->m_frame.m_args : call->m_arguments;
        num = isInline ? md.m_target->numArgs : md.m_target->numArgs;
#else
        array = call->m_arguments;
        num = call->m_call.m_target->numArgs;
#endif
        break;

      case CLR_DBG_Commands::Debugging_Value_GetStack::c_Local:
#ifndef CLR_NO_IL_INLINE
        array = isInline ? call->m_inlineFrame->m_frame.m_locals : call->m_locals;
        num = isInline ? md.m_target->numLocals : md.m_target->numLocals;
#else
        array = call->m_locals;
        num = call->m_call.m_target->numLocals;
#endif
        break;

      case CLR_DBG_Commands::Debugging_Value_GetStack::c_EvalStack:
#ifndef CLR_NO_IL_INLINE
        array = isInline ? call->m_inlineFrame->m_frame.m_evalStack : call->m_evalStack;
        num = isInline ? (CLR_UINT32)(call->m_evalStack - call->m_inlineFrame->m_frame.m_evalStack)
          : (CLR_UINT32)call->TopValuePosition();
#else
        array = call->m_evalStack;
        num = (CLR_UINT32)call->TopValuePosition();
#endif
        break;

      default:
        return false;
      }

    if (cmd->m_index >= num)
      return false;

    CLR_RT_HeapBlock *blk = &array[cmd->m_index];
    CLR_RT_HeapBlock *reference = NULL;
    CLR_RT_HeapBlock tmp;
    CLR_RT_TypeDef_Instance *pTD = NULL;
    CLR_RT_TypeDef_Instance td;

    if (cmd->m_kind != CLR_DBG_Commands::Debugging_Value_GetStack::c_EvalStack && IsBlockEnumMaybe(blk))
      {
      CLR_UINT32 iElement = cmd->m_index;
      CLR_RT_SignatureParser parser;
      CLR_RT_SignatureParser::Element res;
      CLR_RT_TypeDescriptor desc;

      if (cmd->m_kind == CLR_DBG_Commands::Debugging_Value_GetStack::c_Argument)
        {
        parser.Initialize_MethodSignature(md.m_assm, md.m_target);

        iElement++; // Skip the return value, always at the head of the signature.

        if (parser.m_flags & PIMAGE_CEE_CS_CALLCONV_HASTHIS)
          {
          if (iElement == 0)
            return false; // The requested argument is the "this" argument, it can never be a primitive.

          iElement--;
          }
        }
      else
        {
        parser.Initialize_MethodLocals(md.m_assm, md.m_target);
        }

      do
        {
        parser.Advance(res);
        } while (iElement--);

        //
        // Arguments to a methods come from the eval stack and we don't fix up the eval stack for each call.
        // So some arguments have the wrong datatype, since an eval stack push always promotes to 32 bits.
        //
        if (c_CLR_RT_DataTypeLookup[blk->DataType()].m_sizeInBytes == sizeof(CLR_INT32) &&
          c_CLR_RT_DataTypeLookup[res.m_dt].m_sizeInBytes < sizeof(CLR_INT32))
          {
          tmp.Assign(*blk);
          tmp.ChangeDataType(res.m_dt);

          reference = blk;
          blk = &tmp;
          }

        //
        // Check for enum.
        //
        desc.InitializeFromType(res.m_cls);

        if (desc.m_handlerCls.m_target->IsEnum())
          {
          td = desc.m_handlerCls;
          pTD = &td;
          }
      }

    return g_CLR_DBG_Debugger->GetValue(msg, blk, reference, pTD);
    }

  return false;
  }

bool CLR_DBG_Debugger::Debugging_Value_GetField(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Value_GetField *cmd = (CLR_DBG_Commands::Debugging_Value_GetField *)msg->m_payload;
  CLR_RT_HeapBlock *blk = cmd->m_heapblock;
  CLR_RT_HeapBlock *reference = NULL;
  CLR_RT_HeapBlock tmp;
  CLR_RT_TypeDescriptor desc;
  CLR_RT_TypeDef_Instance td;
  CLR_RT_TypeDef_Instance *pTD = NULL;
  CLR_RT_FieldDef_Instance inst;
  CLR_UINT32 offset;

  if (blk != NULL && cmd->m_offset > 0)
    {
    if (FAILED(desc.InitializeFromObject(*blk)))
      return false;

    td = desc.m_handlerCls;
    offset = cmd->m_offset - 1;

    while (true)
      {
      CLR_UINT32 iFields = td.m_target->iFields_Num;
      CLR_UINT32 totalFields = td.CrossReference().m_totalFields;
      CLR_UINT32 dFields = totalFields - iFields;

      if (offset >= dFields)
        {
        offset -= dFields;
        break;
        }

      if (!td.SwitchToParent())
        return false;
      }

    cmd->m_fd.Set(td.Assembly(), td.m_target->iFields_First + offset);
    }

  if (!g_CLR_DBG_Debugger->CheckFieldDef(cmd->m_fd, inst))
    return false;

  if (blk == NULL)
    {
    blk = CLR_RT_ExecutionEngine::AccessStaticField(cmd->m_fd);
    }
  else
    {
    if (cmd->m_offset == 0)
      {
      cmd->m_offset = inst.CrossReference().m_offset;
      }

    if (cmd->m_offset == 0)
      return false;

    switch (blk->DataType())
      {
      case DATATYPE_CLASS:
      case DATATYPE_VALUETYPE:
        break;

      default:
        if (FAILED(blk->EnsureObjectReference(blk)))
          return false;
        break;
      }

    switch (blk->DataType())
      {
      case DATATYPE_DATETIME: // Special case.
      case DATATYPE_TIMESPAN: // Special case.
        tmp.SetInteger((CLR_INT64)blk->NumericByRefConst().s8);
        reference = blk;
        blk = &tmp;
        break;

      default:
        blk = &blk[cmd->m_offset];
        break;
      }
    }

  if (IsBlockEnumMaybe(blk))
    {
    if (SUCCEEDED(desc.InitializeFromFieldDefinition(inst)))
      {
      if (desc.m_handlerCls.m_target->IsEnum())
        {
        pTD = &desc.m_handlerCls;
        }
      }
    }

  return g_CLR_DBG_Debugger->GetValue(msg, blk, reference, pTD);
  }

bool CLR_DBG_Debugger::Debugging_Value_GetArray(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Value_GetArray *cmd = (CLR_DBG_Commands::Debugging_Value_GetArray *)msg->m_payload;
  CLR_RT_HeapBlock *blk = NULL;
  CLR_RT_HeapBlock *reference = NULL;
  CLR_RT_HeapBlock tmp;
  CLR_RT_HeapBlock ref;
  CLR_RT_TypeDef_Instance *pTD = NULL;
  CLR_RT_TypeDef_Instance td;

  tmp.SetObjectReference(cmd->m_heapblock);

  if (SUCCEEDED(ref.InitializeArrayReference(tmp, cmd->m_index)))
    {
    CLR_RT_HeapBlock_Array *array = ref.Array();

    if (array->m_fReference)
      {
      blk = (CLR_RT_HeapBlock *)array->GetElement(cmd->m_index);
      }
    else
      {
      if (FAILED(tmp.LoadFromReference(ref)))
        return false;

      blk = &tmp;
      reference = (CLR_RT_HeapBlock *)-1;
      }

    if (IsBlockEnumMaybe(blk))
      {
      if (td.InitializeFromIndex(array->ReflectionDataConst().m_data.m_type))
        {
        if (td.m_target->IsEnum())
          {
          pTD = &td;
          }
        }
      }
    }

  return g_CLR_DBG_Debugger->GetValue(msg, blk, reference, pTD);
  }

bool CLR_DBG_Debugger::Debugging_Value_GetBlock(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Value_GetBlock *cmd = (CLR_DBG_Commands::Debugging_Value_GetBlock *)msg->m_payload;
  CLR_RT_HeapBlock *blk = cmd->m_heapblock;

  return g_CLR_DBG_Debugger->GetValue(msg, blk, NULL, NULL);
  }

bool CLR_DBG_Debugger::Debugging_Value_GetScratchPad(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Value_GetScratchPad *cmd =
    (CLR_DBG_Commands::Debugging_Value_GetScratchPad *)msg->m_payload;
  CLR_RT_HeapBlock *blk = GetScratchPad_Helper(cmd->m_idx);

  return g_CLR_DBG_Debugger->GetValue(msg, blk, NULL, NULL);
  }

bool CLR_DBG_Debugger::Debugging_Value_SetBlock(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Value_SetBlock *cmd = (CLR_DBG_Commands::Debugging_Value_SetBlock *)msg->m_payload;
  CLR_RT_HeapBlock *blk = cmd->m_heapblock;

  WP_ReplyToCommand(msg, SetBlockHelper(blk, (CLR_DataType)cmd->m_dt, cmd->m_builtinValue), false, NULL, 0);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Value_SetArray(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Value_SetArray *cmd = (CLR_DBG_Commands::Debugging_Value_SetArray *)msg->m_payload;
  CLR_RT_HeapBlock_Array *array = cmd->m_heapblock;
  CLR_RT_HeapBlock tmp;
  bool fSuccess = false;

  tmp.SetObjectReference(cmd->m_heapblock);

  //
  // We can only set values in arrays of primitive types.
  //
  if (array != NULL && !array->m_fReference)
    {
    CLR_RT_HeapBlock ref;

    if (SUCCEEDED(ref.InitializeArrayReference(tmp, cmd->m_index)))
      {
      if (SUCCEEDED(tmp.LoadFromReference(ref)))
        {
        if (SetBlockHelper(&tmp, tmp.DataType(), cmd->m_builtinValue))
          {
          if (SUCCEEDED(tmp.StoreToReference(ref, 0)))
            {
            fSuccess = true;
            }
          }
        }
      }
    }

  WP_ReplyToCommand(msg, fSuccess, false, NULL, 0);

  return true;
  }

//--//

bool CLR_DBG_Debugger::Debugging_Value_AllocateObject(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Value_AllocateObject *cmd =
    (CLR_DBG_Commands::Debugging_Value_AllocateObject *)msg->m_payload;
  CLR_RT_HeapBlock *blk = NULL;
  CLR_RT_HeapBlock *ptr = GetScratchPad_Helper(cmd->m_index);

  if (ptr)
    {
    if (SUCCEEDED(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*ptr, cmd->m_td)))
      {
      blk = ptr;
      }
    }

  return g_CLR_DBG_Debugger->GetValue(msg, blk, NULL, NULL);
  }

bool CLR_DBG_Debugger::Debugging_Value_AllocateString(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Value_AllocateString *cmd =
    (CLR_DBG_Commands::Debugging_Value_AllocateString *)msg->m_payload;
  CLR_RT_HeapBlock *blk = NULL;
  CLR_RT_HeapBlock *ptr = GetScratchPad_Helper(cmd->m_index);

  if (ptr)
    {
    CLR_RT_HeapBlock_String *str = CLR_RT_HeapBlock_String::CreateInstance(*ptr, cmd->m_size);

    if (str)
      {
      char *dst = (char *)str->StringText();

      //
      // Fill the string with spaces, it will be set at a later stage.
      //
      memset(dst, ' ', cmd->m_size);
      dst[cmd->m_size] = 0;

      blk = ptr;
      }
    }

  return g_CLR_DBG_Debugger->GetValue(msg, blk, NULL, NULL);
  }

bool CLR_DBG_Debugger::Debugging_Value_AllocateArray(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Value_AllocateArray *cmd =
    (CLR_DBG_Commands::Debugging_Value_AllocateArray *)msg->m_payload;
  CLR_RT_HeapBlock *blk = NULL;
  CLR_RT_HeapBlock *ptr = GetScratchPad_Helper(cmd->m_index);

  if (ptr)
    {
    CLR_RT_ReflectionDef_Index reflex;

    reflex.m_kind = REFLECTION_TYPE;
    reflex.m_levels = cmd->m_depth;
    reflex.m_data.m_type = cmd->m_td;

    if (SUCCEEDED(CLR_RT_HeapBlock_Array::CreateInstance(*ptr, cmd->m_numOfElements, reflex)))
      {
      blk = ptr;
      }
    }

  return g_CLR_DBG_Debugger->GetValue(msg, blk, NULL, NULL);
  }

#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

#if defined(NANOCLR_PROFILE_NEW)
bool CLR_DBG_Debugger::Profiling_Command(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  CLR_DBG_Debugger *dbg = g_CLR_DBG_Debugger;
  CLR_DBG_Commands::Profiling_Command *cmd = (CLR_DBG_Commands::Profiling_Command *)msg->m_payload;
  CLR_UINT8 command = cmd->m_command;

  switch (command)
    {
    case CLR_DBG_Commands::Profiling_Command::c_Command_ChangeConditions:
      return dbg->Profiling_ChangeConditions(msg);

    case CLR_DBG_Commands::Profiling_Command::c_Command_FlushStream:
      return dbg->Profiling_FlushStream(msg);

    default:
      return false;
    }
  }

bool CLR_DBG_Debugger::Profiling_ChangeConditions(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  CLR_DBG_Commands::Profiling_Command *parent_cmd = (CLR_DBG_Commands::Profiling_Command *)msg->m_payload;
  CLR_DBG_Commands::Profiling_ChangeConditions *cmd = (CLR_DBG_Commands::Profiling_ChangeConditions *)&parent_cmd[1];

  g_CLR_RT_ExecutionEngine.m_iProfiling_Conditions |= cmd->m_set;
  g_CLR_RT_ExecutionEngine.m_iProfiling_Conditions &= ~cmd->m_reset;

  CLR_DBG_Commands::Profiling_Command::Reply cmdReply;

  cmdReply.m_raw = g_CLR_RT_ExecutionEngine.m_iProfiling_Conditions;

  WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

  _ASSERTE(FIMPLIES(CLR_EE_PRF_IS(Allocations), CLR_EE_PRF_IS(Enabled)));
  _ASSERTE(FIMPLIES(CLR_EE_PRF_IS(Calls), CLR_EE_PRF_IS(Enabled)));

  if ((cmd->m_set & CLR_RT_ExecutionEngine::c_fProfiling_Enabled) != 0)
    {
    g_CLR_PRF_Profiler.SendMemoryLayout();
    }

  return true;
  }

bool CLR_DBG_Debugger::Profiling_FlushStream(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  CLR_DBG_Commands::Profiling_Command::Reply cmdReply;

  g_CLR_PRF_Profiler.Stream_Flush();

  cmdReply.m_raw = 0;

  WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

  return true;
  }

#endif //#if defined(NANOCLR_PROFILE_NEW)

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

//--//

struct AnalyzeObject
  {
  CLR_RT_HeapBlock *m_ptr;
  bool m_fNull;
  bool m_fBoxed;
  bool m_fCanBeNull;
  CLR_RT_TypeDescriptor m_desc;
  CLR_RT_HeapBlock m_value;
  };

static HRESULT AnalyzeObject_Helper(CLR_RT_HeapBlock *ptr, AnalyzeObject &ao)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  NANOCLR_HEADER();

  if (ptr && ptr->DataType() == DATATYPE_BYREF)
    ptr = ptr->Dereference();

  ao.m_ptr = ptr;

  if (ptr == NULL || (ptr->DataType() == DATATYPE_OBJECT && ptr->Dereference() == NULL))
    {
    ao.m_fNull = true;
    ao.m_fBoxed = false;
    ao.m_fCanBeNull = true;
    }
  else
    {
    NANOCLR_CHECK_HRESULT(ao.m_desc.InitializeFromObject(*ptr));

    ao.m_fNull = false;
    ao.m_fBoxed = (ptr->DataType() == DATATYPE_OBJECT && ptr->Dereference()->IsBoxed());

    switch (ao.m_desc.m_flags & CLR_RT_DataTypeLookup::c_SemanticMask2 & ~CLR_RT_DataTypeLookup::c_SemanticMask)
      {
      case CLR_RT_DataTypeLookup::c_Array:
      case CLR_RT_DataTypeLookup::c_ArrayList:
        ao.m_fCanBeNull = true;
        break;
      default:
      {
      switch (ao.m_desc.m_flags & CLR_RT_DataTypeLookup::c_SemanticMask)
        {
        case CLR_RT_DataTypeLookup::c_Primitive:
        case CLR_RT_DataTypeLookup::c_ValueType:
        case CLR_RT_DataTypeLookup::c_Enum:
          ao.m_fCanBeNull =
            ao.m_fBoxed || (ao.m_desc.m_handlerCls.m_data == g_CLR_RT_WellKnownTypes.m_String.m_data);
          break;

        default:
          ao.m_fCanBeNull = true;
          break;
        }

      break;
      }
      }
    }

  NANOCLR_NOCLEANUP();
  }

static HRESULT Assign_Helper(CLR_RT_HeapBlock *blkDst, CLR_RT_HeapBlock *blkSrc)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  NANOCLR_HEADER();

  AnalyzeObject aoDst;
  AnalyzeObject aoSrc;
  CLR_RT_HeapBlock srcVal;
  srcVal.SetObjectReference(NULL);
  CLR_RT_ProtectFromGC gc(srcVal);

  NANOCLR_CHECK_HRESULT(AnalyzeObject_Helper(blkDst, aoDst));
  NANOCLR_CHECK_HRESULT(AnalyzeObject_Helper(blkSrc, aoSrc));

  if (aoSrc.m_fNull)
    {
    if (aoDst.m_fCanBeNull == false)
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
      }

    NANOCLR_CHECK_HRESULT(srcVal.StoreToReference(*blkDst, 0));
    }
  else
    {
    NANOCLR_CHECK_HRESULT(srcVal.LoadFromReference(*blkSrc));

    if (aoDst.m_fNull)
      {
      if (aoSrc.m_fCanBeNull == false)
        {
        NANOCLR_CHECK_HRESULT(srcVal.PerformBoxing(aoSrc.m_desc.m_handlerCls));
        }

      blkDst->Assign(srcVal);
      }
    else
      {
      if (srcVal.IsAValueType())
        {
        if (blkDst->IsAValueType() == false)
          {
          NANOCLR_CHECK_HRESULT(srcVal.PerformBoxing(aoSrc.m_desc.m_handlerCls));
          }
        }
      else
        {
        if (blkDst->IsAValueType() == true)
          {
          NANOCLR_CHECK_HRESULT(srcVal.PerformUnboxing(aoSrc.m_desc.m_handlerCls));
          }
        }

      NANOCLR_CHECK_HRESULT(blkDst->Reassign(srcVal));
      }
    }

  NANOCLR_NOCLEANUP();
  }

bool CLR_DBG_Debugger::Debugging_Value_Assign(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Value_Assign *cmd = (CLR_DBG_Commands::Debugging_Value_Assign *)msg->m_payload;
  CLR_RT_HeapBlock *blkDst = cmd->m_heapblockDst;
  CLR_RT_HeapBlock *blkSrc = cmd->m_heapblockSrc;

  if (blkDst && FAILED(Assign_Helper(blkDst, blkSrc)))
    {
    blkDst = NULL;
    }

  return g_CLR_DBG_Debugger->GetValue(msg, blkDst, NULL, NULL);
  }

//--//

bool CLR_DBG_Debugger::Debugging_TypeSys_Assemblies(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_RT_Assembly_Index assemblies[CLR_RT_TypeSystem::c_MaxAssemblies];
  int num = 0;

  NANOCLR_FOREACH_ASSEMBLY(g_CLR_RT_TypeSystem)
    {
    assemblies[num++].Set(pASSM->m_idx);
    }
  NANOCLR_FOREACH_ASSEMBLY_END();

  WP_ReplyToCommand(msg, true, false, assemblies, sizeof(CLR_RT_Assembly_Index) * num);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Resolve_Assembly(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Resolve_Assembly *cmd = (CLR_DBG_Commands::Debugging_Resolve_Assembly *)msg->m_payload;
  CLR_DBG_Commands::Debugging_Resolve_Assembly::Reply cmdReply;
  CLR_RT_Assembly *assm = g_CLR_DBG_Debugger->IsGoodAssembly(cmd->m_idx.Assembly());

  if (assm)
    {
#if defined(_WIN32)
    // append path
    if (assm->m_strPath != NULL)
      {
      sprintf_s(
        cmdReply.m_szName,
        ARRAYSIZE(cmdReply.m_szName),
        "%s,%s",
        assm->m_szName,
        assm->m_strPath->c_str());
      }
    else
#endif
      {
      hal_strncpy_s(
        cmdReply.m_szName,
        ARRAYSIZE(cmdReply.m_szName),
        assm->m_szName,
        MAXSTRLEN(cmdReply.m_szName));
      }

    cmdReply.m_flags = assm->m_flags;
    cmdReply.m_version = assm->m_header->version;

    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

    return true;
    }

  WP_ReplyToCommand(msg, false, false, NULL, 0);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Resolve_Type(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Resolve_Type *cmd = (CLR_DBG_Commands::Debugging_Resolve_Type *)msg->m_payload;
  CLR_DBG_Commands::Debugging_Resolve_Type::Reply cmdReply;
  CLR_RT_TypeDef_Instance inst;

  if (g_CLR_DBG_Debugger->CheckTypeDef(cmd->m_td, inst))
    {
    char *szBuffer = cmdReply.m_type;
    size_t iBuffer = MAXSTRLEN(cmdReply.m_type);

    if (SUCCEEDED(g_CLR_RT_TypeSystem.BuildTypeName(inst, szBuffer, iBuffer)))
      {
      WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

      return true;
      }
    }

  WP_ReplyToCommand(msg, false, false, NULL, 0);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Resolve_Field(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Resolve_Field *cmd = (CLR_DBG_Commands::Debugging_Resolve_Field *)msg->m_payload;
  CLR_DBG_Commands::Debugging_Resolve_Field::Reply cmdReply;
  CLR_RT_FieldDef_Instance inst;

  if (g_CLR_DBG_Debugger->CheckFieldDef(cmd->m_fd, inst))
    {
    char *szBuffer = cmdReply.m_name;
    size_t iBuffer = MAXSTRLEN(cmdReply.m_name);

    if (SUCCEEDED(g_CLR_RT_TypeSystem.BuildFieldName(inst, szBuffer, iBuffer)))
      {
      CLR_RT_TypeDef_Instance instClass;
      instClass.InitializeFromField(inst);

      cmdReply.m_td = instClass;
      cmdReply.m_index = inst.CrossReference().m_offset;

      WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

      return true;
      }
    }

  WP_ReplyToCommand(msg, false, false, NULL, 0);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Resolve_Method(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Resolve_Method *cmd = (CLR_DBG_Commands::Debugging_Resolve_Method *)msg->m_payload;
  CLR_DBG_Commands::Debugging_Resolve_Method::Reply cmdReply;
  CLR_RT_MethodDef_Instance inst;
  CLR_RT_TypeDef_Instance instOwner;

  if (g_CLR_DBG_Debugger->CheckMethodDef(cmd->m_md, inst) && instOwner.InitializeFromMethod(inst))
    {
    char *szBuffer = cmdReply.m_method;
    size_t iBuffer = MAXSTRLEN(cmdReply.m_method);

    cmdReply.m_td = instOwner;

    CLR_SafeSprintf(szBuffer, iBuffer, "%s", inst.m_assm->GetString(inst.m_target->name));

    WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

    return true;
    }

  WP_ReplyToCommand(msg, false, false, NULL, 0);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Resolve_VirtualMethod(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Resolve_VirtualMethod *cmd =
    (CLR_DBG_Commands::Debugging_Resolve_VirtualMethod *)msg->m_payload;
  CLR_DBG_Commands::Debugging_Resolve_VirtualMethod::Reply cmdReply;
  CLR_RT_TypeDef_Index cls;
  CLR_RT_MethodDef_Index md;

  cmdReply.m_md.Clear();

  if (SUCCEEDED(CLR_RT_TypeDescriptor::ExtractTypeIndexFromObject(*cmd->m_obj, cls)))
    {
    if (g_CLR_RT_EventCache.FindVirtualMethod(cls, cmd->m_md, md))
      {
      cmdReply.m_md = md;
      }
    }

  WP_ReplyToCommand(msg, true, false, &cmdReply, sizeof(cmdReply));

  return true;
  }

#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

//--//

bool CLR_DBG_Debugger::Debugging_Deployment_Status(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  WP_ReplyToCommand(msg, false, false, NULL, 0);
  return false;
  }

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

bool CLR_DBG_Debugger::Debugging_Info_SetJMC_Method(const CLR_RT_MethodDef_Index &idx, bool fJMC)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  CLR_RT_MethodDef_Instance inst;

  if (!CheckMethodDef(idx, inst))
    return false;
  if (inst.m_target->RVA == CLR_EmptyIndex)
    return false;

  inst.DebuggingInfo().SetJMC(fJMC);

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Info_SetJMC_Type(const CLR_RT_TypeDef_Index &idx, bool fJMC)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();
  const CLR_RECORD_TYPEDEF *td;
  CLR_RT_TypeDef_Instance inst;
  int totMethods;
  CLR_RT_MethodDef_Index md;

  if (!CheckTypeDef(idx, inst))
    return false;

  td = inst.m_target;
  totMethods = td->vMethods_Num + td->iMethods_Num + td->sMethods_Num;

  for (int i = 0; i < totMethods; i++)
    {
    md.Set(idx.Assembly(), td->methods_First + i);

    Debugging_Info_SetJMC_Method(md, fJMC);
    }

  return true;
  }

bool CLR_DBG_Debugger::Debugging_Info_SetJMC(WP_Message *msg)
  {
  NATIVE_PROFILE_CLR_DEBUGGER();

  CLR_DBG_Commands::Debugging_Info_SetJMC *cmd = (CLR_DBG_Commands::Debugging_Info_SetJMC *)msg->m_payload;
  bool fJMC = (cmd->m_fIsJMC != 0);

  switch (cmd->m_kind)
    {
    case REFLECTION_ASSEMBLY:
    {
    CLR_RT_Assembly *assm = g_CLR_DBG_Debugger->IsGoodAssembly(cmd->m_data.m_assm.Assembly());

    if (!assm)
      return false;

    for (int i = 0; i < assm->m_pTablesSize[TBL_TypeDef]; i++)
      {
      CLR_RT_TypeDef_Index idx;

      idx.Set(cmd->m_data.m_assm.Assembly(), i);

      g_CLR_DBG_Debugger->Debugging_Info_SetJMC_Type(idx, fJMC);
      }

    return true;
    }

    case REFLECTION_TYPE:
      return g_CLR_DBG_Debugger->Debugging_Info_SetJMC_Type(cmd->m_data.m_type, fJMC);

    case REFLECTION_METHOD:
      return g_CLR_DBG_Debugger->Debugging_Info_SetJMC_Method(cmd->m_data.m_method, fJMC);

    default:
      return false;
    }
  }

#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
