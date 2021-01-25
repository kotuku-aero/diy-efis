/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "ion.h"
#include "../photon/photon.h"

#include <ctype.h>

extern "C" {
  extern const uint8_t neo[];
  extern const uint16_t neo_length;
  }

// include the runtime interface
#include "nano/CLR/Include/nanoCLR_Runtime.h"

static semaphore_p ion_mutex;

// these run the system.  They are normally in the nanoFramework Core.cpp
// however we don't want the Hardware interface.
CLR_RT_ExecutionEngine     g_CLR_RT_ExecutionEngine;
CLR_RT_WellKnownTypes      g_CLR_RT_WellKnownTypes;
CLR_RT_WellKnownMethods    g_CLR_RT_WellKnownMethods;
CLR_RT_TypeSystem          g_CLR_RT_TypeSystem;
CLR_RT_EventCache          g_CLR_RT_EventCache;
CLR_RT_GarbageCollector    g_CLR_RT_GarbageCollector;

uint32_t g_buildCRC = 0xBAADF00D;

static uint8_t *heap_base = 0;
// 8mb
#define HEAP_SIZE  8388608

// this defines the RTL heap.
void HeapLocation(unsigned char *&base_address, unsigned int &size_in_bytes)
  {
  if (heap_base == 0)
    {
    // allocate 8mb of memory to the heap
    heap_base = (uint8_t *)ion_malloc(HEAP_SIZE);
    }

  base_address = heap_base;
  size_in_bytes = HEAP_SIZE;
  }

// these interface from the nano debugger
int WP_ReceiveBytes(uint8_t *ptr, unsigned short *size)
  {
  return 0;
  }

extern result_t ion_close(ion_context_t *ion)
  {
  // stop the worker
  close_task(ion->worker);

  neutron_free(ion);
  return s_ok;
  }

struct assembly_loader_t
  {
  uint32_t length;
  uint8_t* data;
  };

static result_t get_byte_assembly(handle_t parg, uint32_t offset, uint8_t* data)
  {
  assembly_loader_t* loader = (assembly_loader_t*)parg;

  if (offset >= loader->length)
    return e_bad_pointer;

  *data = loader->data[offset];
  return s_ok;
  }

static result_t set_byte_assembly(handle_t parg, uint32_t offset, uint8_t data)
  {
  assembly_loader_t* loader = (assembly_loader_t*)parg;

  if (offset >= loader->length)
    return e_bad_pointer;

  loader->data[offset] = data;
  return s_ok;
  }

// the stream is assumed to be at the start of a local_file in an zip archive
static result_t expand_zip_local_file(stream_p stream, assembly_loader_t* loader, uint32_t *entry_size)
  {
  if (stream == 0 || loader == 0)
    return e_bad_parameter;

  result_t result;
  /*
4.3.7  Local file header:

      local file header signature     4 bytes  (0x04034b50)
      version needed to extract       2 bytes
      general purpose bit flag        2 bytes
      compression method              2 bytes
      last mod file time              2 bytes
      last mod file date              2 bytes
      crc-32                          4 bytes
      compressed size                 4 bytes
      uncompressed size               4 bytes
      file name length                2 bytes
      extra field length              2 bytes
  */
  uint8_t file_4[4];
  if (failed(result = stream_read(stream, file_4, 4, 0)))
    return result;

  if (file_4[3] != 0x04 ||
    file_4[2] != 0x03 ||
    file_4[1] != 0x4b ||
    file_4[0] != 0x50)
    {
    // check to see if this is the central directory header
    if (file_4[3] == 0x02 ||
      file_4[2] == 0x01 ||
      file_4[1] == 0x4b ||
      file_4[0] == 0x50)
      return e_not_found;
    // nope file corrupt
    return e_bad_parameter;
    }

  uint8_t file_2[2];
  // version needed to extract
  if (failed(result = stream_read(stream, file_2, 2, 0)))
    return result;
  // General flags
  if (failed(result = stream_read(stream, file_2, 2, 0)))
    return result;
  // compression method
  if (failed(result = stream_read(stream, file_2, 2, 0)))
    return result;
  // only Deflate is supported
  if (file_2[1] != 0 ||
    file_2[0] != 8)
    return e_bad_parameter;

  // skip to uncompressed_size
  if (failed(result = stream_read(stream, file_2, 2, 0)) ||   // last mod file time
    failed(result = stream_read(stream, file_2, 2, 0)) ||     // last mod file date
    failed(result = stream_read(stream, file_4, 4, 0)) ||     // crc-32
    failed(result = stream_read(stream, file_4, 4, 0)))       // compressed size
    return result;

  if(entry_size != 0)
    *entry_size = (((uint32_t)file_4[3]) << 24) | (((uint32_t)file_4[2]) << 16) | (((uint32_t)file_4[1]) << 8) | ((uint32_t)file_4[0]);

  // read decomressed size
  if (failed(result = stream_read(stream, file_4, 4, 0)))
    return result;

  loader->length = (((size_t)file_4[3]) << 24) | (((size_t)file_4[2]) << 16) | (((size_t)file_4[1]) << 8) | ((size_t)file_4[0]);

  // filename length
  if (failed(result = stream_read(stream, file_2, 2, 0)))
    return result;

  size_t filename_size = (((size_t)file_2[1]) << 8) | ((size_t)file_2[0]);

  // extra lenth
  if (failed(result = stream_read(stream, file_2, 2, 0)))
    return result;

  size_t extra_size = (((size_t)file_2[0]) << 8) | ((size_t)file_2[1]);

  uint32_t pos;
  if (failed(result = stream_getpos(stream, &pos)))
    return result;

  pos += extra_size + filename_size;
  // add header size to the variable bits
  *entry_size = *entry_size + extra_size + filename_size + 30;
  if (failed(result = stream_setpos(stream, pos)))
    return result;

  loader->data = (uint8_t*)ion_malloc(loader->length);
  if (loader->data == 0)
    return e_not_enough_memory;

  // expand the image into the buffer.
  // all image assemblies are stored in a DEFLATE form
  if (failed(result = decompress(stream, loader, get_byte_assembly, set_byte_assembly, 0)))
    {
    ion_free(loader->data);
    return result;
    }

  return s_ok;
  }

// expand a file, and add to the runtime.
static result_t load_application(stream_p stream)
  {
  // This code assumes a zip file that contains all of the assemblies in a compressed form
  // makes swapping applications easy.

  result_t result;
  // allocate a block of memory in the CLR heap area to expand the stream into
  assembly_loader_t loader;
  uint32_t entry_size = 0;
  uint32_t header_position = 0;
  CLR_RT_Assembly *assembly;
  do
  {
    result = expand_zip_local_file(stream, &loader, &entry_size);
    if (result == e_not_found)
      break;      // end of compressed files in the archive

    if (failed(result = CLR_RT_Assembly::CreateInstance((CLR_RECORD_ASSEMBLY*)loader.data, assembly)))
      {
      ion_free(loader.data);
      return result;
      }

    const CLR_RT_NativeAssemblyData* pNativeAssmData = GetAssemblyNativeData(assembly->m_szName);

    trace_info("Load assembly %s\n", assembly->m_szName);

    // If pNativeAssmData not NULL- means this assembly has native calls and there is pointer to table with native calls.
    if (pNativeAssmData != NULL)
      {
      // First verify that check sum in assembly object matches hardcoded check sum.
      if (assembly->m_header->nativeMethodsChecksum != pNativeAssmData->m_checkSum)
        {
        trace_debug("\r\n\r\n***********************************************************************\r\n");
        trace_debug("*                                                                     *\r\n");
        trace_debug("* ERROR!!!!  Firmware version does not match managed code version!!!! *\r\n");
        trace_debug("*                                                                     *\r\n");
        trace_debug("*                                                                     *\r\n");
        trace_debug("* Invalid native checksum: %s 0x%08X!=0x%08X *\r\n",
          assembly->m_szName,
          assembly->m_header->nativeMethodsChecksum,
          pNativeAssmData->m_checkSum
        );
        trace_debug("*                                                                     *\r\n");
        trace_debug("***********************************************************************\r\n");

        ion_free(loader.data);
        return e_bad_type;
        }

      // Assembly has valid pointer to table with native methods. Save it.
      assembly->m_nativeCode = (const CLR_RT_MethodHandler*)pNativeAssmData->m_pNativeMethods;
      }

    // and link the assembly to the native type system
    g_CLR_RT_TypeSystem.Link(assembly);

    // skip to the next file entry
    header_position += entry_size;
    if (failed(result = stream_setpos(stream, header_position)))
      return result;

    } while (failed(stream_eof(stream)));


  return s_ok;
  }

extern result_t neutron_clr_init();

result_t ion_create(ion_context_t *ion_context)
  {
  result_t result;

  trace_info("ion runtime start");

  if (failed(result = semaphore_create(&ion_mutex)) ||
    failed(result = semaphore_signal(ion_mutex)))
    return result;

  // load the neo font which is always available
  if (failed(register_font(neo, neo_length)))
    {
    trace_error("Cannot load the neo font when starting ion");
    return result;
    }

  // set the global nano options
  g_CLR_RT_ExecutionEngine.m_fPerformGarbageCollection = true;
  g_CLR_RT_ExecutionEngine.m_fPerformHeapCompaction = true;

  // init the heap
  CLR_RT_Memory::Reset();

  // load the CLR engine
  if (failed(result = CLR_RT_ExecutionEngine::CreateInstance()))
    {
    trace_error("Ion cannot start the EE");
    return result;
    }

  // enable the debugger
  bool debugger_enabled = false;
  if (succeeded(reg_get_bool(ion_context->home, "debugger", &debugger_enabled)))
    {
    CLR_EE_DBG_SET(Enabled);
    }

  if (failed(result = g_CLR_RT_ExecutionEngine.StartHardware()))
    {
    trace_error("Cannot start the CLR environment");
    return result;
    }

  // start the debugger handlers.
  // CLR_DBG_Debugger::Debugger_Discovery();

  // load all of the assemblies defined in the startup registry key
  char startup_assembly[REG_STRING_MAX];
  if (failed(result = reg_get_string(ion_context->home, "start", startup_assembly, 0)))
    {
    trace_warning("No startup assembly defined\n");
    return result;
    }

  stream_p startup_stream;
  // we only support loading of assemblies from the file system (never the registry)
  if(failed(result = stream_open(startup_assembly, &startup_stream)))
    {
    trace_error("The startup assembly for ion %s cannot be found\n", startup_assembly);
    return result;
    }

  // register all of the types loaded in the file.
  // The application should contain:
  // The startup application as <application>.pe
  // ..\\packages\\nanoFramework.CoreLibrary.1.0.0-preview038\\lib\\mscorlib.pe
  // 
  if (failed(result = load_application(startup_stream)))
    {
    trace_error("The application for ion %s cannot be loaded due to a corrupt zip file format");
    return result;
    }

  trace_info("Load the type system\n");

  if (failed(result = g_CLR_RT_TypeSystem.ResolveAll()))
    {
    trace_error("Cannot resolve all types when starting CLR\n");
    return result;
    }

  if (failed(result = g_CLR_RT_TypeSystem.PrepareForExecution()))
    {
    trace_error("Cannot prepare CLR application for execution\n");
    return result;
    }

#if !defined(BUILD_RTM)
  if (result == CLR_E_TYPE_UNAVAILABLE)
    {
    // exception occurred during type resolution
    CLR_EE_DBG_SET(StateResolutionFailed);
    }
#endif

  // start the event handler
  if (failed(result = neutron_clr_init()))
    {
    trace_error("cannot start the clr integration");
    return result;
    }

  trace_info("CLR started ok, ion ready\n");
  return result;
  }

extern "C" {
  extern result_t pen_init();
  }

void ion_run(void *arg)
  {
  result_t result;

  ion_context_t *ion_context = (ion_context_t *)arg;

  int32_t max_context_switches;
  if (failed(result = reg_get_int32(ion_context->home, "max-switches", &max_context_switches)))
    max_context_switches = 50;

  bool enter_debugger_after_exit;
  if (failed(result = reg_get_bool(ion_context->home, "debug-after-exit", &enter_debugger_after_exit)))
    enter_debugger_after_exit = false;

  bool softReboot = false;

  do
    {
    pen_init();

    CLR_RT_Assembly::InitString();
    CLR_RT_Memory::Reset();

    trace_info("Starting CLR\n");

    result_t result = g_CLR_RT_ExecutionEngine.Execute(NULL, max_context_switches);

    // notify debugger the program has exited
    debug_program_exit(0);

    // this could be a reboot
    if ((g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions & c_fDebugger_RebootPending) == 0)
      {
      CLR_EE_DBG_SET_MASK(StateProgramExited, StateMask);

      // break into the debugger, return s_ok to restart
      if (failed(debug_break()))
        break;    // exit the interpreter

      softReboot = true;
      }
    } while (softReboot);


    // release the context
    neutron_free(ion_context);
  }

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_mscorlib;

// table of native assembly handlers

const CLR_RT_NativeAssemblyData *g_CLR_InteropAssembliesNativeData[] =
  {
    &g_CLR_AssemblyNative_mscorlib,
    NULL
  };

uint16_t g_CLR_InteropAssembliesCount = ARRAYSIZE(g_CLR_InteropAssembliesNativeData) - 1;

void NFReleaseInfo::Init(
  NFReleaseInfo &NFReleaseInfo,
  unsigned short int major,
  unsigned short int minor,
  unsigned short int build,
  unsigned short int revision,
  const char *info,
  size_t infoLen,
  const char *target,
  size_t targetLen,
  const char *platform,
  size_t platformLen)
  {
  }

uint64_t  HAL_Time_CurrentDateTime(bool datePartOnly)
  {
  tm_t time;
  result_t result;
  if (failed(result = now(&time)))
    return 0;

  SYSTEMTIME st;
  st.wYear = time.year;
  st.wMonth = time.month;
  st.wDay = time.day;

  if (datePartOnly)
    {
    st.wHour = 0;
    st.wMinute = 0;
    st.wSecond = 0;
    st.wMilliseconds = 0;
    }
  else
    {
    st.wHour = time.hour;
    st.wMinute = time.minute;
    st.wSecond = time.second;
    st.wMilliseconds = time.milliseconds;
    }

  // problem is different epoch's
  return HAL_Time_ConvertFromSystemTime(&st);
  }

bool DateTimeToString(const int64_t &time, char *&buf, size_t &len)
  {
  SYSTEMTIME st;
  HAL_Time_ToSystemTime(time, &st);


  return CLR_SafeSprintf(buf, len, "%4d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear,
    st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
  }

bool HAL_Time_TimeSpanToStringEx(const int64_t &_ticks, char *&buf, size_t &len)
  {
  int64_t ticks = _ticks;
  bool minus = ticks < 0;
  if (minus)
    ticks *= -1;

  // convert .1 ns to ms
  ticks /= 10000;

  uint32_t msec = (uint32_t)( ticks % 1000);
  ticks /= 1000;
  uint32_t secs = (uint32_t)(ticks % 1000);
  ticks /= 1000;
  uint32_t min = (uint32_t)(ticks % 60);
  ticks /= 60;
  uint32_t hours = (uint32_t)(ticks % 60);
  ticks /= 60;
  uint32_t days = (uint32_t)(ticks % 24);

  if (minus)
    return CLR_SafeSprintf(buf, len, "-%05d %02d:%02d:%02d.%03d", days, hours, min, secs, msec);

  return CLR_SafeSprintf(buf, len, "%05d %02d:%02d:%02d.%03d", days, hours, min, secs, msec);
  }

char *DateTimeToString(const int64_t &time)
  {
  static char rgBuffer[128];
  char *szBuffer = rgBuffer;
  size_t iBuffer = ARRAYSIZE(rgBuffer);

  DateTimeToString(time, szBuffer, iBuffer);

  return rgBuffer;
  }

const char *HAL_Time_CurrentDateTimeToString()
  {
  return DateTimeToString(HAL_Time_CurrentDateTime(false));
  }

uint64_t CPU_MillisecondsToTicks(uint64_t ticks)
  {
  return ticks;
  }

uint64_t HAL_Time_SysTicksToTime(unsigned int sysTicks)
  {
  return ((uint64_t)sysTicks) * 10000;
  }

uint32_t HAL_Time_CurrentSysTicks()
  {
  return ticks();
  }

