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
#include "runtime/HeapBlockDispatcher.h"

#include "../nano/CLR/Include/WireProtocol_Message.h"

static semaphore_p ion_mutex;

// these run the system.  They are normally in the nanoFramework Core.cpp
// however we don't want the Hardware interface.
CLR_RT_ExecutionEngine     g_CLR_RT_ExecutionEngine;
CLR_RT_WellKnownTypes      g_CLR_RT_WellKnownTypes;
CLR_RT_WellKnownMethods    g_CLR_RT_WellKnownMethods;
CLR_RT_TypeSystem          g_CLR_RT_TypeSystem;
CLR_RT_EventCache          g_CLR_RT_EventCache;
CLR_RT_GarbageCollector    g_CLR_RT_GarbageCollector;

CLR_RT_HeapBlock_CanFlyMsgDispatcher g_CLR_MessageDispatcher;

static uint8_t *heap_base = 0;
static size_t heap_size = 0;

// this defines the RTL heap.
void HeapLocation(unsigned char *&base_address, unsigned int &size_in_bytes)
  {
  if (heap_base == 0)
    {
    // allocate 8mb of memory to the heap
    heap_size = 8192 * 1024 * 1024;
    heap_base = (uint8_t *)ion_malloc(heap_size);
    }

  base_address = heap_base;
  size_in_bytes = heap_size;
  }

// these interface from the nano debugger
int WP_ReceiveBytes(uint8_t *ptr, unsigned short *size)
  {
  return 0;
  }

int WP_TransmitMessage(WP_Message *message)
  {
  return 0;
  }


result_t ion_init()
  {
  result_t result;
  if (failed(result = semaphore_create(&ion_mutex)) ||
    failed(semaphore_signal(ion_mutex)))
    return result;

  // create the ion keys
  memid_t ion_home;
  if (failed(result = reg_open_key(0, ion_s, &ion_home)))
    {
    if (failed(result = reg_create_key(0, ion_s, &ion_home)))
      return result;
    }

  memid_t parent;
  if (failed(result = reg_open_key(ion_home, event_s, &parent)))
    {
    if (failed(result = reg_create_key(ion_home, event_s, &parent)))
      return result;
    }

  return s_ok;
  }

extern result_t ion_close(struct _ion_context_t *ion)
  {
  // stop the worker
  close_task(ion->worker);

  neutron_free(ion);
  return s_ok;
  }

result_t ion_queue_message(struct _ion_context_t *ion, const canmsg_t *msg)
  {
  g_CLR_MessageDispatcher.SaveToProtonQueue(msg);
  return s_ok;
  }

static bool ion_hook_handler(const canmsg_t* msg, void* parg)
  {
  // the hook handler discards messages so that if
  // the dispatcher is hung up the publisher is not stopped.
  ion_queue_message(0, msg);

  return false;
  }

static msg_hook_t ion_hook = { 0, 0, ion_hook_handler };

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
static result_t expand_zip_local_file(stream_p stream, assembly_loader_t* loader, uint32_t *compressed_size)
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

  if (file_4[0] != 0x04 ||
    file_4[1] != 0x03 ||
    file_4[2] != 0x4b ||
    file_4[3] != 0x50)
    {
    // check to see if this is the central directory header
    if (file_4[0] != 0x02 ||
      file_4[1] != 0x01 ||
      file_4[2] != 0x4b ||
      file_4[3] != 0x50)
      return e_not_found;
    // nope file corrupt
    return e_bad_parameter;
    }

  uint8_t file_2[2];
  if (failed(result = stream_read(stream, file_2, 2, 0)))
    return result;

  if (failed(result = stream_read(stream, file_2, 2, 0)))
    return result;

  // only support deflate.
  if (file_2[0] != 2 ||
    file_2[1] != 0)
    return e_bad_parameter;

  if (failed(result = stream_read(stream, file_2, 2, 0)))
    return result;

  if (failed(result = stream_read(stream, file_2, 2, 0)))
    return result;

  // only Deflate is supported
  if (file_2[0] != 0 ||
    file_2[1] != 9)
    return e_bad_parameter;

  // skip to uncompressed_size
  if (failed(result = stream_read(stream, file_2, 2, 0)) ||   // last mod file time
    failed(result = stream_read(stream, file_2, 2, 0)) ||     // last mod file date
    failed(result = stream_read(stream, file_4, 4, 0)) ||     // crc-32
    failed(result = stream_read(stream, file_4, 4, 0)))       // compressed size
    return result;

  if(compressed_size != 0)
    *compressed_size = (((uint32_t)file_4[0]) << 24) | (((uint32_t)file_4[1]) << 16) | (((uint32_t)file_4[2]) << 8) | ((uint32_t)file_4[3]);

  // read decomressed size
  if (failed(result = stream_read(stream, file_4, 4, 0)))
    return result;

  loader->length = (((size_t)file_4[0]) << 24) | (((size_t)file_4[1]) << 16) | (((size_t)file_4[2]) << 8) | ((size_t)file_4[3]);

  if (failed(result = stream_read(stream, file_2, 2, 0)))
    return result;

  size_t filename_size = (((size_t)file_2[0]) << 8) | ((size_t)file_2[1]);


  if (failed(result = stream_read(stream, file_2, 2, 0)))
    return result;

  size_t extra_size = (((size_t)file_2[0]) << 8) | ((size_t)file_2[1]);

  uint32_t pos;
  if (failed(result = stream_getpos(stream, &pos)))
    return result;

  pos += extra_size;
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
  uint32_t compressed_size = 0;
  uint32_t header_position = 0;
  CLR_RT_Assembly *assembly;
  do
  {
    result = expand_zip_local_file(stream, &loader, &compressed_size);
    if (result == e_not_found)
      break;      // end of compressed files in the archive

    if (failed(result = CLR_RT_Assembly::CreateInstance((CLR_RECORD_ASSEMBLY*)loader.data, assembly)))
      {
      ion_free(loader.data);
      return result;
      }

    const CLR_RT_NativeAssemblyData* pNativeAssmData = GetAssemblyNativeData(assembly->m_szName);
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
    header_position += compressed_size;
    if (failed(result = stream_setpos(stream, header_position)))
      return result;

    } while (failed(stream_eof(stream)));


  return result;
  }

result_t ion_create(memid_t home,
                    const char *path,
                    handle_t ci,
                    handle_t co,
                    handle_t cerr,
                    struct _ion_context_t **ion)
  {
  result_t result;

  trace_info("ion runtime start");

  // load the neo font which is always available
  if (failed(register_font(neo, neo_length)))
    {
    trace_error("Cannot load the neo font when starting ion");
    return result;
    }

  // enumerate the ion keys
  memid_t ion_home;
  if (failed(result = reg_open_key(0, ion_s, &ion_home)))
    {
    if (failed(result = reg_create_key(0, ion_s, &ion_home)))
      {
      trace_error("Unable to create the ion key when starting ion");
      return result;
      }
    }

  // set the global nano options
  g_CLR_RT_ExecutionEngine.m_fPerformGarbageCollection = true;
  g_CLR_RT_ExecutionEngine.m_fPerformHeapCompaction = true;

  // load the CLR engine
  if (failed(result = CLR_RT_ExecutionEngine::CreateInstance()))
    {
    trace_error("Ion cannot start the EE");
    return result;
    }

  // enable the debugger
  bool debugger_enabled = false;
  if (succeeded(reg_get_bool(ion_home, "debugger", &debugger_enabled)))
    {
    CLR_EE_DBG_SET(Enabled);
    }

  if (failed(result = g_CLR_RT_ExecutionEngine.StartHardware()))
    {
    trace_error("Cannot start the CLR environment");
    return result;
    }

  // start the debugger handlers.
  CLR_DBG_Debugger::Debugger_Discovery();

  if (failed(result = subscribe(&ion_hook)))
    {
    trace_error("Ion cannot install hook");
    return result;
    }

  // load all of the assemblies defined in the startup registry key
  char startup_assembly[REG_STRING_MAX];
  if (failed(result = reg_get_string(ion_home, "start", startup_assembly, 0)))
    {
    trace_warning("No startup assembly defined");
    return result;
    }

  stream_p startup_stream;
  // we only support loading of assemblies from the file system (never the registry)
  if(failed(result = stream_open(startup_assembly, &startup_stream)))
    {
    trace_error("The startup assembly for ion %s cannot be found", startup_assembly);
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

  trace_info("Load the type system");

  if (failed(result = g_CLR_RT_TypeSystem.ResolveAll()))
    {
    trace_error("Cannot resolve all types when starting CLR");
    return result;
    }

  if (failed(result = g_CLR_RT_TypeSystem.PrepareForExecution()))
    {
    trace_error("Cannot prepare CLR application for execution");
    return result;
    }

#if defined(NANOCLR_PROFILE_HANDLER)
  CLR_PROF_Handler::Calibrate();
#endif

#if !defined(BUILD_RTM)
  if (result == CLR_E_TYPE_UNAVAILABLE)
    {
    // exception occurred during type resolution
    CLR_EE_DBG_SET(StateResolutionFailed);
    }
#endif

  trace_info("CLR started ok, ion ready")
  return result;
  }

result_t ion_run(memid_t key)
  {
  result_t result;
  memid_t ion_home;
  if (failed(result = reg_open_key(0, ion_s, &ion_home)))
    return result;

  int32_t max_context_switches;
  if (failed(result = reg_get_int32(ion_home, "max-switches", &max_context_switches)))
    max_context_switches = 50;

  bool enter_debugger_after_exit;
  if (failed(result = reg_get_bool(ion_home, "debug-after-exit", &enter_debugger_after_exit)))
    enter_debugger_after_exit = false;
  
  while (true)
    {
    bool softReboot;

    do
      {
      softReboot = false;

      CLR_RT_Assembly::InitString();
      CLR_RT_Memory::Reset();

      trace_info("Starting CLR");

      result_t result = g_CLR_RT_ExecutionEngine.Execute(NULL, max_context_switches);

      // this could be a reboot
      if (CLR_EE_DBG_IS_NOT(RebootPending))
        {
        CLR_EE_DBG_SET_MASK(StateProgramExited, StateMask);
        CLR_EE_DBG_EVENT_BROADCAST(CLR_DBG_Commands::c_Monitor_ProgramExit, 0, NULL, WP_Flags_c_NonCritical);


        if (enter_debugger_after_exit)
          CLR_DBG_Debugger::Debugger_WaitForCommands();
        }

      // DO NOT USE 'ELSE IF' here because the state can change in Debugger_WaitForCommands() call

      if (CLR_EE_DBG_IS(RebootPending))
        {
        if (CLR_EE_REBOOT_IS(ClrOnly))
          {
          softReboot = true;

          enter_debugger_after_exit = CLR_EE_REBOOT_IS(WaitForDebugger);

          //nanoHAL_Uninitialize();

          //re-init the hal for the reboot (initially it is called in bootentry)
          //nanoHAL_Initialize();
          }
        else
          break;        // TODO: reset the device....
        }
      } while (softReboot);
    }

  return s_ok;
  }
