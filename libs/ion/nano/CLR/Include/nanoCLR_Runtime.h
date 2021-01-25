//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_RUNTIME_H_
#define _NANOCLR_RUNTIME_H_

////////////////////////////////////////////////////////////////////////////////////////////////////

#include "nanoCLR_Types.h"
#include "nanoCLR_Interop.h"
#include "nanoCLR_ErrorCodes.h"
#include "nanoSupport.h"
#include "nanoWeak.h"
#include "../../../ion_debugger.h"

struct CLR_RADIAN
  {
  short cos;
  short sin;
  };

extern const CLR_RADIAN c_CLR_radians[];

//--//

#if defined(_WIN32)

#include <set>
#include <map>
#include <list>
#include <string>
#include <vector>
#include <comutil.h>
#include <comip.h>

typedef std::set<std::wstring> CLR_RT_StringSet;
typedef CLR_RT_StringSet::iterator CLR_RT_StringSetIter;

typedef std::map<std::string, int> CLR_RT_StringMap;
typedef CLR_RT_StringMap::iterator CLR_RT_StringMapIter;

typedef std::vector<std::wstring> CLR_RT_StringVector;
typedef CLR_RT_StringVector::iterator CLR_RT_StringVectorIter;

typedef std::map<std::wstring, uint32_t> CLR_RT_SymbolToAddressMap;
typedef CLR_RT_SymbolToAddressMap::iterator CLR_RT_SymbolToAddressMapIter;

typedef std::map<uint32_t, std::wstring> CLR_RT_AddressToSymbolMap;
typedef CLR_RT_AddressToSymbolMap::iterator CLR_RT_AddressToSymbolMapIter;

#else

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_4_CompactionPlus
#include <list>
#include <map>
#endif

#endif // #if defined(_WIN32)

#if defined(_MSC_VER)
#pragma pack(push, NANOCLR_RUNTIME_H, 4)
#endif

#if defined(_WIN32)
//--//

#define _COM_SMRT_PTR(i)      typedef _com_ptr_t<_com_IIID<i, &__uuidof(i)>> i##Ptr
#define _COM_SMRT_PTR_2(i, u) typedef _com_ptr_t<_com_IIID<i, &u>> i##Ptr

_COM_SMRT_PTR(IXMLDOMDocument);
_COM_SMRT_PTR(IXMLDOMNode);

class CLR_XmlUtil
  {
  IXMLDOMDocumentPtr m_xddDoc;
  IXMLDOMNodePtr m_xdnRoot;
  HANDLE m_hEvent;           // Used to abort a download.
  unsigned long m_dwTimeout; // Used to limit a download.

  void Init();
  void Clean();

  HRESULT LoadPost(
    /*[in] */ const wchar_t* szRootTag,
    /*[out]*/ bool& fLoaded,
    /*[out]*/ bool* fFound);

  HRESULT CreateParser();

  public:
    CLR_XmlUtil(/*[in]*/ const CLR_XmlUtil& xml);
    CLR_XmlUtil(/*[in]*/ IXMLDOMDocument* xddDoc, /*[in]*/ const wchar_t* szRootTag = NULL);
    CLR_XmlUtil(/*[in]*/ IXMLDOMNode* xdnRoot = NULL, /*[in]*/ const wchar_t* szRootTag = NULL);

    ~CLR_XmlUtil();

    CLR_XmlUtil& operator=(/*[in]*/ const CLR_XmlUtil& xml);
    CLR_XmlUtil& operator=(/*[in]*/ IXMLDOMNode* xdnRoot);

    HRESULT DumpError();

    HRESULT New(/*[in]*/ IXMLDOMNode* xdnRoot, /*[in] */ bool fDeep = false);
    HRESULT New(/*[in]*/ const wchar_t* szRootTag, /*[in] */ const wchar_t* szEncoding = L"utf-8" /*L"unicode"*/);
    HRESULT Load(
      /*[in ]*/ const wchar_t* szFile,
      /*[in]*/ const wchar_t* szRootTag,
      /*[out]*/ bool& fLoaded,
      /*[out]*/ bool* fFound = NULL);
    HRESULT LoadAsStream(
      /*[in ]*/ IUnknown* pStream,
      /*[in]*/ const wchar_t* szRootTag,
      /*[out]*/ bool& fLoaded,
      /*[out]*/ bool* fFound = NULL);
    HRESULT LoadAsString(
      /*[in ]*/ BSTR bstrData,
      /*[in]*/ const wchar_t* szRootTag,
      /*[out]*/ bool& fLoaded,
      /*[out]*/ bool* fFound = NULL);
    HRESULT Save(/*[in ]*/ const wchar_t* szFile);
    HRESULT SaveAsStream(/*[out]*/ IUnknown** ppStream);
    HRESULT SaveAsString(/*[out]*/ BSTR* pbstrData);

    HRESULT SetTimeout(/*[in]*/ unsigned long dwTimeout);
    HRESULT Abort();

    HRESULT SetVersionAndEncoding(/*[in]*/ const wchar_t* szVersion, /*[in]*/ const wchar_t* szEncoding);

    HRESULT GetDocument(/*[out]*/ IXMLDOMDocument** pVal) const;
    HRESULT GetRoot(/*[out]*/ IXMLDOMNode** pVal) const;
    HRESULT GetNodes(/*[in]*/ const wchar_t* szTag, /*[out]*/ IXMLDOMNodeList** pVal) const;
    HRESULT GetNode(/*[in]*/ const wchar_t* szTag, /*[out]*/ IXMLDOMNode** pVal) const;
    HRESULT CreateNode(
      /*[in]*/ const wchar_t* szTag,
      /*[out]*/ IXMLDOMNode** pVal,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);

    HRESULT GetAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[out]*/ IXMLDOMAttribute** pValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT GetAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[out]*/ _bstr_t& bstrValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT GetAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[out]*/ std::wstring& szValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT GetAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[out]*/ signed int& lValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT GetValue(
      /*[in]*/ const wchar_t* szTag,
      /*[out]*/ _variant_t& vValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT GetValue(
      /*[in]*/ const wchar_t* szTag,
      /*[out]*/ _bstr_t& bstrValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT GetValue(
      /*[in]*/ const wchar_t* szTag,
      /*[out]*/ std::wstring& szValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);

    HRESULT ModifyAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[in] */ const _bstr_t& bstrValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT ModifyAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[in] */ const std::wstring& szValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT ModifyAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[in] */ const wchar_t* szValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT ModifyAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[in] */ signed int lValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT ModifyValue(
      /*[in]*/ const wchar_t* szTag,
      /*[in] */ const _variant_t& vValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT ModifyValue(
      /*[in]*/ const wchar_t* szTag,
      /*[in] */ const _bstr_t& bstrValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT ModifyValue(
      /*[in]*/ const wchar_t* szTag,
      /*[in] */ const std::wstring& szValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);

    HRESULT PutAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[in] */ IXMLDOMAttribute** pValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT PutAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[in] */ const _bstr_t& bstrValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT PutAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[in] */ const std::wstring& szValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT PutAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[in] */ const wchar_t* szValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT PutAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[in] */ signed int lValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT PutValue(
      /*[in]*/ const wchar_t* szTag,
      /*[in] */ const _variant_t& vValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT PutValue(
      /*[in]*/ const wchar_t* szTag,
      /*[in] */ const _bstr_t& bstrValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT PutValue(
      /*[in]*/ const wchar_t* szTag,
      /*[in] */ const std::wstring& szValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT PutValue(
      /*[in]*/ const wchar_t* szTag,
      /*[in] */ const wchar_t* szValue,
      /*[out]*/ bool& fFound,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);

    HRESULT RemoveAttribute(
      /*[in]*/ const wchar_t* szTag,
      /*[in]*/ const wchar_t* szAttr,
      /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT RemoveValue(/*[in]*/ const wchar_t* szTag, /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
    HRESULT RemoveNode(/*[in]*/ const wchar_t* szTag, /*[in]*/ IXMLDOMNode* pxdnNode = NULL);
  };

//--//

typedef std::vector<uint8_t> CLR_RT_Buffer;

struct CLR_RT_FileStore
  {
  static HRESULT LoadFile(const wchar_t* szFile, CLR_RT_Buffer& vec);
  static HRESULT SaveFile(const wchar_t* szFile, const CLR_RT_Buffer& vec);
  static HRESULT SaveFile(const wchar_t* szFile, const uint8_t* buf, size_t size);

  static HRESULT ExtractTokensFromFile(
    const wchar_t* szFileName,
    CLR_RT_StringVector& vec,
    const wchar_t* separators = L" \t",
    bool fNoComments = true);

  static void ExtractTokens(
    const CLR_RT_Buffer& buf,
    CLR_RT_StringVector& vec,
    const wchar_t* separators = L" \t",
    bool fNoComments = true);
  static void ExtractTokensFromBuffer(
    wchar_t* szLine,
    CLR_RT_StringVector& vec,
    const wchar_t* separators = L" \t",
    bool fNoComments = true);
  static void ExtractTokensFromString(
    const wchar_t* szLine,
    CLR_RT_StringVector& vec,
    const wchar_t* separators = L" \t");
  };

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
#if !defined(MAX)
#define MAX(a, b) (a > b ? a : b)
#endif
#if !defined(MIN)
#define MIN(a, b) (a < b ? a : b)
#endif
#define ABS(x)     (x > 0 ? x : (-x))
#define FLOOR32(x) ((int32_t)x)
#define FRAC(x)    (x - FLOOR32(x))

////////////////////////////////////////////////////////////////////////////////////////////////////

struct CLR_RT_HeapBlock;
struct CLR_RT_HeapBlock_Node;

struct CLR_RT_HeapBlock_WeakReference;
struct CLR_RT_HeapBlock_String;
struct CLR_RT_HeapBlock_Array;
struct CLR_RT_HeapBlock_Delegate;
struct CLR_RT_HeapBlock_Delegate_List;
struct CLR_RT_HeapBlock_BinaryBlob;

struct CLR_RT_HeapBlock_Button;
struct CLR_RT_HeapBlock_Lock;
struct CLR_RT_HeapBlock_LockRequest;
struct CLR_RT_HeapBlock_Timer;
struct CLR_RT_HeapBlock_WaitForObject;
struct CLR_RT_HeapBlock_Finalizer;
struct CLR_RT_HeapBlock_MemoryStream;

struct CLR_RT_HeapCluster;
struct CLR_RT_GarbageCollector;

struct CLR_RT_DblLinkedList;

#if defined(NANOCLR_APPDOMAINS)
struct CLR_RT_AppDomain;
struct CLR_RT_AppDomainAssembly;
#endif // NANOCLR_APPDOMAINS

struct CLR_RT_Assembly;
struct CLR_RT_TypeSystem;
struct CLR_RT_TypeDescriptor;

struct CLR_RT_Assembly_Instance;
struct CLR_RT_TypeSpec_Instance;
struct CLR_RT_TypeDef_Instance;
struct CLR_RT_MethodDef_Instance;
struct CLR_RT_FieldDef_Instance;

struct CLR_RT_StackFrame;
struct CLR_RT_SubThread;
struct CLR_RT_Thread;
struct CLR_RT_ExecutionEngine;

struct CLR_RT_ExecutionEngine_PerfCounters;

struct CLR_HW_Hardware;

typedef void (*CLR_RT_MarkingHandler)(CLR_RT_HeapBlock_BinaryBlob* ptr);
typedef void (*CLR_RT_RelocationHandler)(CLR_RT_HeapBlock_BinaryBlob* ptr);
typedef void (*CLR_RT_HardwareHandler)();


struct CLR_RT_MemoryRange
  {
  unsigned char* m_location;
  unsigned int m_size;

  bool LimitToRange(CLR_RT_MemoryRange& filtered, unsigned char* address, unsigned int length) const;
  };

extern CLR_RT_MemoryRange s_CLR_RT_Heap;

//--//

struct CLR_RT_Memory
  {
  static void ZeroFill(void* buf, size_t len)
    {
    memset(buf, 0, len);
    }

  //--//

  static void Reset();
  static void* SubtractFromSystem(size_t len);
  static void Release(void* ptr);
  static void* Allocate(size_t len, uint32_t flags = 0);
  static void* ReAllocate(void* ptr, size_t len);
  static void* Allocate_And_Erase(size_t len, uint32_t flags = 0);
  };

//--//

struct CLR_RT_Random
  {
  public:
    void Initialize();
    void Initialize(int seed);

    uint32_t Next();

    double NextDouble();
    void NextBytes(unsigned char* buffer, unsigned int count);
  };

//--//

extern void CLR_RT_GetVersion(
  unsigned short int* pMajor,
  unsigned short int* pMinor,
  unsigned short int* pBuild,
  unsigned short int* pRevision);

#define NANOCLR_CLEAR(ref) CLR_RT_Memory::ZeroFill(&ref, sizeof(ref))

//--//

static const int c_CLR_RT_Trace_None = 0;
static const int c_CLR_RT_Trace_Info = 1;
static const int c_CLR_RT_Trace_Verbose = 2;
static const int c_CLR_RT_Trace_Annoying = 3;
static const int c_CLR_RT_Trace_Obnoxious = 4;

extern int s_CLR_RT_fTrace_Errors;
extern int s_CLR_RT_fTrace_Exceptions;
extern int s_CLR_RT_fTrace_Instructions;
extern int s_CLR_RT_fTrace_Memory;
extern int s_CLR_RT_fTrace_MemoryStats;
extern int s_CLR_RT_fTrace_StopOnFAILED;
extern int s_CLR_RT_fTrace_GC;
extern int s_CLR_RT_fTrace_GC_Depth;
extern int s_CLR_RT_fTrace_SimulateSpeed;
extern int s_CLR_RT_fTrace_AssemblyOverhead;

#if defined(_WIN32)
extern int s_CLR_RT_fTrace_ARM_Execution;

extern int s_CLR_RT_fTrace_RedirectLinesPerFile;
extern std::wstring s_CLR_RT_fTrace_RedirectOutput;
extern std::wstring s_CLR_RT_fTrace_RedirectCallChain;

extern std::wstring s_CLR_RT_fTrace_HeapDump_FilePrefix;
extern bool s_CLR_RT_fTrace_HeapDump_IncludeCreators;

extern bool s_CLR_RT_fTimeWarp;

#endif

static const int MAXHOSTNAMELEN = 256; // Typical of many sockets implementations, incl. Windows
static const int MAXTYPENAMELEN = 256; // Including terminating null byte. Enforced in MetadataProcessor. The standard
                                       // imposes no limit, but we necessarily do.

////////////////////////////////////////////////////////////////////////////////////////////////////

#define NANOCLR_INDEX_IS_VALID(idx)   ((idx) != 0)
#define NANOCLR_INDEX_IS_INVALID(idx) ((idx) == 0)

//
// IMPORTANT: THE ASSEMBLY IDX IN ALL THE CLR_RT_*_Index STRUCTURES SHOULD ALWAYS BE ENCODED THE SAME WAY!!!
//
// For details, go to "bool CLR_RT_GarbageCollector::ComputeReachabilityGraphForMultipleBlocks( CLR_RT_HeapBlock*
// lstExt, uint32_t numExt )"
//

struct CLR_RT_ReflectionDef_Index
  {
  uint16_t m_kind; // CLR_ReflectionType
  uint16_t m_levels;

  union {
    uint32_t m_assm;
    uint32_t m_type;
    uint32_t m_method;
    uint32_t m_field;
    uint32_t m_raw;
    } m_data;

  //--//

  void Clear();

  uint32_t GetTypeHash() const;

  void InitializeFromHash(uint32_t hash);

  uint64_t GetRawData() const;
  void SetRawData(uint64_t data);

  //--//

  static bool Convert(CLR_RT_HeapBlock& ref, CLR_RT_Assembly_Instance& inst);
  static bool Convert(CLR_RT_HeapBlock& ref, CLR_RT_TypeDef_Instance& inst, uint32_t* levels);
  static bool Convert(CLR_RT_HeapBlock& ref, CLR_RT_MethodDef_Instance& inst);
  static bool Convert(CLR_RT_HeapBlock& ref, CLR_RT_FieldDef_Instance& inst);
  static bool Convert(CLR_RT_HeapBlock& ref, uint32_t& hash);
  };

//--//

struct CLR_RT_AssemblyRef_CrossReference
  {
  CLR_RT_Assembly* m_target; // EVENT HEAP - NO RELOCATION -
  };

struct CLR_RT_TypeRef_CrossReference
  {
  uint32_t m_target;
  };

struct CLR_RT_FieldRef_CrossReference
  {
  uint32_t m_target;
  };

struct CLR_RT_MethodRef_CrossReference
  {
  uint32_t m_target;
  };

struct CLR_RT_FieldDef_CrossReference
  {
  uint16_t m_offset;
  };

struct CLR_RT_TypeDef_CrossReference
  {
  static const uint32_t TD_CR_StaticConstructorCalled = 0x0001;
  static const uint32_t TD_CR_HasFinalizer = 0x0002;
  static const uint32_t TD_CR_IsMarshalByRefObject = 0x0004;

  uint16_t m_flags;
  uint16_t m_totalFields;
  uint32_t m_hash;
  };

struct CLR_RT_MethodDef_CrossReference
  {
  static const uint16_t MD_CR_Patched = 0x8000;
  static const uint16_t MD_CR_OwnerMask = 0x7FFF;

  uint16_t m_data;

  uint16_t GetOwner() const
    {
    return (uint16_t)(m_data);
    }
  };

struct CLR_RT_MethodDef_Patch
  {
  uint16_t m_orig;
  uint16_t m_patched;
  };

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

struct CLR_RT_MethodDef_DebuggingInfo
  {
  static const uint8_t MD_DI_JustMyCode = 0x01;
  static const uint8_t MD_DI_HasBreakpoint = 0x02;

  uint8_t m_flags;

  bool IsJMC() const
    {
    return IsFlagSet(MD_DI_JustMyCode);
    }
  bool HasBreakpoint() const
    {
    return IsFlagSet(MD_DI_HasBreakpoint);
    }

  void SetJMC(bool b)
    {
    SetResetFlags(b, MD_DI_JustMyCode);
    }
  void SetBreakpoint(bool b)
    {
    SetResetFlags(b, MD_DI_HasBreakpoint);
    }

  private:
    void SetFlags(uint8_t flags)
      {
      m_flags |= flags;
      }
    void ResetFlags(uint8_t flags)
      {
      m_flags &= ~flags;
      }
    bool IsFlagSet(uint8_t flags) const
      {
      return (m_flags & flags) != 0;
      }
    void SetResetFlags(bool b, uint8_t flags)
      {
      if (b)
        SetFlags(flags);
      else
        ResetFlags(flags);
      }
  };

#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../CLR/Include/nanoCLR_Runtime__HeapBlock.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CLR_RT_UnicodeHelper
  {
  static const uint32_t SURROGATE_HALFSHIFT = 10;
  static const uint32_t SURROGATE_HALFBASE = 0x00010000;
  static const uint32_t SURROGATE_HALFMASK = 0x000003FF;

#ifdef HIGH_SURROGATE_START
#undef HIGH_SURROGATE_START
#undef HIGH_SURROGATE_END
#undef LOW_SURROGATE_START
#undef LOW_SURROGATE_END
#endif
  static const uint16_t HIGH_SURROGATE_START = 0xD800;
  static const uint16_t HIGH_SURROGATE_END = 0xDBFF;
  static const uint16_t LOW_SURROGATE_START = 0xDC00;
  static const uint16_t LOW_SURROGATE_END = 0xDFFF;

  const uint8_t* m_inputUTF8;
  const uint16_t* m_inputUTF16;

  uint8_t* m_outputUTF8;
  int m_outputUTF8_size;

  uint16_t* m_outputUTF16;
  int m_outputUTF16_size;

  //--//

  void SetInputUTF8(const char* src)
    {
    m_inputUTF8 = (const uint8_t*)src;
    }
  void SetInputUTF16(const uint16_t* src)
    {
    m_inputUTF16 = src;
    }

  int CountNumberOfCharacters(int max = -1);
  int CountNumberOfBytes(int max = -1);

  //--//

  bool ConvertFromUTF8(int iMaxChars, bool fJustMove, int iMaxBytes = -1);
  bool ConvertToUTF8(int iMaxChars, bool fJustMove);

  bool MoveBackwardInUTF8(const char* utf8StringStart, int iMaxChars);

#if defined(_WIN32)
  static void ConvertToUTF8(const std::wstring& src, std::string& dst);
  static void ConvertFromUTF8(const std::string& src, std::wstring& dst);
#endif
  };

class UnicodeString
  {
  private:
    CLR_RT_UnicodeHelper m_unicodeHelper;
    CLR_RT_HeapBlock m_uHeapBlock;
    uint16_t* m_wCharArray;
    int m_length; /// Length in wide characters (not bytes).

  public:
    UnicodeString();
    ~UnicodeString();

    HRESULT Assign(const char* string);
    operator const wchar_t* ()
      {
      return (const wchar_t*)m_wCharArray;
      }
    unsigned int Length()
      {
      return m_length;
      }

  private:
    void Release();
  };

//--//

struct CLR_RT_ArrayListHelper
  {
  static const int c_defaultCapacity = 2;

  static const int FIELD___items = 1;
  static const int FIELD___size = 2;

  //--//

  static HRESULT PrepareArrayList(CLR_RT_HeapBlock& pThisRef, int count, int capacity);
  static HRESULT ExtractArrayFromArrayList(
    CLR_RT_HeapBlock& pThisRef,
    CLR_RT_HeapBlock_Array*& array,
    int& count,
    int& capacity);
  };

//--//

struct CLR_RT_ByteArrayReader
  {
  HRESULT Init(const unsigned char* src, unsigned int srcSize);

  HRESULT Read(void* dst, unsigned int size);
  HRESULT Read1Byte(void* dst);

  HRESULT Skip(unsigned int size);

  bool IsValid()
    {
    return (source && sourceSize > 0);
    }

  const unsigned char* source;
  unsigned int sourceSize;
  };

////////////////////////////////////////////////////////////////////////////////////////////////////

struct CLR_RT_SignatureParser
  {
  static const int c_TypeSpec = 0;
  static const int c_Interfaces = 1;
  static const int c_Field = 2;
  static const int c_Method = 3;
  static const int c_Locals = 4;
  static const int c_Object = 5;

  struct Element
    {
    bool m_fByRef;
    int m_levels;
    CLR_DataType m_dt;
    uint32_t m_cls;
    };

  CLR_RT_HeapBlock* m_lst;
  CLR_RT_Assembly* m_assm;
  CLR_PMETADATA m_sig;

  int m_type;
  uint32_t m_flags;
  int m_count;

  //--//

  void Initialize_TypeSpec(CLR_RT_Assembly* assm, const CLR_RECORD_TYPESPEC* ts);
  void Initialize_Interfaces(CLR_RT_Assembly* assm, const CLR_RECORD_TYPEDEF* td);
  void Initialize_FieldDef(CLR_RT_Assembly* assm, const CLR_RECORD_FIELDDEF* fd);
  void Initialize_MethodSignature(CLR_RT_Assembly* assm, const CLR_RECORD_METHODDEF* md);
  void Initialize_MethodLocals(CLR_RT_Assembly* assm, const CLR_RECORD_METHODDEF* md);

  void Initialize_TypeSpec(CLR_RT_Assembly* assm, CLR_PMETADATA ts);
  void Initialize_FieldDef(CLR_RT_Assembly* assm, CLR_PMETADATA fd);
  void Initialize_MethodSignature(CLR_RT_Assembly* assm, CLR_PMETADATA md);

  void Initialize_Objects(CLR_RT_HeapBlock* lst, int count, bool fTypes);

  int Available() const
    {
    return m_count;
    }

  HRESULT Advance(Element& res);
  };

////////////////////////////////////////////////////////////////////////////////////////////////////

#define NANOCLR_FOREACH_ASSEMBLY(ts)                                                                                   \
    {                                                                                                                  \
        CLR_RT_Assembly **ppASSM = (ts).m_assemblies;                                                                  \
        size_t iASSM = (ts).m_assembliesMax;                                                                           \
        for (; iASSM--; ppASSM++)                                                                                      \
        {                                                                                                              \
            CLR_RT_Assembly *pASSM = *ppASSM;                                                                          \
            if (pASSM)

#define NANOCLR_FOREACH_ASSEMBLY_END()                                                                                 \
    }                                                                                                                  \
    }

#define NANOCLR_FOREACH_ASSEMBLY_NULL(ts)                                                                              \
    {                                                                                                                  \
        CLR_RT_Assembly **ppASSM = (ts).m_assemblies;                                                                  \
        size_t iASSM = ARRAYSIZE((ts).m_assemblies);                                                                   \
        uint16_t idx = 1;                                                                                               \
        for (; iASSM--; ppASSM++, idx++)                                                                               \
        {                                                                                                              \
            if (*ppASSM == NULL)

#define NANOCLR_FOREACH_ASSEMBLY_NULL_END()                                                                            \
    }                                                                                                                  \
    }

#if defined(NANOCLR_APPDOMAINS)
#define NANOCLR_FOREACH_ASSEMBLY_IN_APPDOMAIN(ad)                                                                      \
    NANOCLR_FOREACH_NODE(CLR_RT_AppDomainAssembly, appDomainAssembly, (ad)->m_appDomainAssemblies)                     \
    {                                                                                                                  \
        CLR_RT_Assembly *pASSM = appDomainAssembly->m_assembly;

#define NANOCLR_FOREACH_ASSEMBLY_IN_APPDOMAIN_END()                                                                    \
    }                                                                                                                  \
    NANOCLR_FOREACH_NODE_END()

#define NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN(ts)                                                              \
    NANOCLR_FOREACH_ASSEMBLY_IN_APPDOMAIN(g_CLR_RT_ExecutionEngine.GetCurrentAppDomain())
#define NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN_END() NANOCLR_FOREACH_ASSEMBLY_IN_APPDOMAIN_END()
#else
#define NANOCLR_FOREACH_ASSEMBLY_IN_APPDOMAIN(ts, ad)       NANOCLR_FOREACH_ASSEMBLY(ts)
#define NANOCLR_FOREACH_ASSEMBLY_IN_APPDOMAIN_END()         NANOCLR_FOREACH_ASSEMBLY_END()
#define NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN(ts)   NANOCLR_FOREACH_ASSEMBLY(ts)
#define NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN_END() NANOCLR_FOREACH_ASSEMBLY_END()
#endif
// This type is needed on PC only for Interop code generation. For device code forward declaration only
class CLR_RT_VectorOfManagedElements;

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

struct CLR_RT_Assembly : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  struct Offsets
    {
    size_t iBase;
    size_t iAssemblyRef;
    size_t iTypeRef;
    size_t iFieldRef;
    size_t iMethodRef;
    size_t iTypeDef;
    size_t iFieldDef;
    size_t iMethodDef;
    size_t iStaticFields;
    size_t iDebuggingInfoMethods;
    };

  //--//

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // NEED TO KEEP THESE IN SYNC WITH Enum 'Commands.DebuggingResolveAssembly.ResolvedStatus' on debugger library code
  // //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  static const uint32_t Resolved = 0x00000001;
  static const uint32_t ResolutionCompleted = 0x00000002;
  static const uint32_t PreparedForExecution = 0x00000004;
  static const uint32_t Deployed = 0x00000008;
  static const uint32_t PreparingForExecution = 0x00000010;
  static const uint32_t StaticConstructorsExecuted = 0x00000020;

  uint32_t m_idx; // Relative to the type system (for static fields access).
  uint32_t m_flags;

  const CLR_RECORD_ASSEMBLY* m_header; // ANY HEAP - DO RELOCATION -
  const char* m_szName;                // ANY HEAP - DO RELOCATION -

  const CLR_RT_MethodHandler* m_nativeCode;

  int m_pTablesSize[TBL_Max];

  CLR_RT_HeapBlock* m_pStaticFields; // EVENT HEAP - NO RELOCATION - (but the data they point to has to be relocated)

  int m_iStaticFields;

  CLR_RT_HeapBlock_Array* m_pFile; // ANY HEAP - DO RELOCATION -

  CLR_RT_AssemblyRef_CrossReference
    * m_pCrossReference_AssemblyRef; // EVENT HEAP - NO RELOCATION - (but the data they point to has to be relocated)
  CLR_RT_TypeRef_CrossReference
    * m_pCrossReference_TypeRef; // EVENT HEAP - NO RELOCATION - (but the data they point to has to be relocated)
  CLR_RT_FieldRef_CrossReference
    * m_pCrossReference_FieldRef; // EVENT HEAP - NO RELOCATION - (but the data they point to has to be relocated)
  CLR_RT_MethodRef_CrossReference
    * m_pCrossReference_MethodRef; // EVENT HEAP - NO RELOCATION - (but the data they point to has to be relocated)
  CLR_RT_TypeDef_CrossReference
    * m_pCrossReference_TypeDef; // EVENT HEAP - NO RELOCATION - (but the data they point to has to be relocated)
  CLR_RT_FieldDef_CrossReference
    * m_pCrossReference_FieldDef; // EVENT HEAP - NO RELOCATION - (but the data they point to has to be relocated)
  CLR_RT_MethodDef_CrossReference
    * m_pCrossReference_MethodDef; // EVENT HEAP - NO RELOCATION - (but the data they point to has to be relocated)

  CLR_RT_MethodDef_DebuggingInfo
    * m_pDebuggingInfo_MethodDef; // EVENT HEAP - NO RELOCATION - (but the data they point to has to be relocated)

#if defined(NANOCLR_TRACE_STACK_HEAVY) && defined(_WIN32)
  int m_maxOpcodes;
  int* m_stackDepth;
#endif

#if defined(_WIN32)
  std::string* m_strPath;
#endif

  //--//

  static void InitString();

  bool IsSameAssembly(const CLR_RT_Assembly& assm) const;

#if defined(_WIN32)

  static void InitString(std::map<std::string, uint16_t>& map);
  static HRESULT CreateInstance(const CLR_RECORD_ASSEMBLY* data, CLR_RT_Assembly*& assm, const wchar_t* szName);

#endif

  //--//

  static HRESULT CreateInstance(const CLR_RECORD_ASSEMBLY* data, CLR_RT_Assembly*& assm);
  void DestroyInstance();

  void Assembly_Initialize(CLR_RT_Assembly::Offsets& offsets);

  bool Resolve_AssemblyRef(bool fOutput);
  HRESULT Resolve_TypeRef();
  HRESULT Resolve_FieldRef();
  HRESULT Resolve_MethodRef();
  void Resolve_TypeDef();
  void Resolve_MethodDef();
  void Resolve_Link();
  HRESULT Resolve_ComputeHashes();
  HRESULT Resolve_AllocateStaticFields(CLR_RT_HeapBlock* pStaticFields);

  HRESULT PrepareForExecution();

  uint32_t ComputeAssemblyHash();
  uint32_t ComputeAssemblyHash(const CLR_RECORD_ASSEMBLYREF* ar);

  bool FindTypeDef(const char* name, const char* nameSpace, uint32_t& idx);
  bool FindTypeDef(const char* name, uint16_t scope, uint32_t& idx);
  bool FindTypeDef(uint32_t hash, uint32_t& idx);

  bool FindFieldDef(
    const CLR_RECORD_TYPEDEF* src,
    const char* name,
    CLR_RT_Assembly* base,
    uint16_t sig,
    uint32_t& idx);
  bool FindMethodDef(
    const CLR_RECORD_TYPEDEF* src,
    const char* name,
    CLR_RT_Assembly* base,
    uint16_t sig,
    uint32_t& idx);

  bool FindNextStaticConstructor(uint32_t& idx);

  bool FindMethodBoundaries(uint16_t i, uint16_t& start, uint16_t& end);

  void Relocate();

  //--//

  CLR_RT_HeapBlock* GetStaticField(const int index);

  //--//

  CLR_PMETADATA GetTable(CLR_TABLESENUM tbl)
    {
    return (CLR_PMETADATA)m_header + m_header->startOfTables[tbl];
    }

#define NANOCLR_ASSEMBLY_RESOLVE(cls, tbl, idx)                                                                        \
    (const cls *)((uint8_t *)m_header + m_header->startOfTables[tbl] + (sizeof(cls) * idx))
  const CLR_RECORD_ASSEMBLYREF* GetAssemblyRef(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(CLR_RECORD_ASSEMBLYREF, TBL_AssemblyRef, i);
    }
  const CLR_RECORD_TYPEREF* GetTypeRef(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(CLR_RECORD_TYPEREF, TBL_TypeRef, i);
    }
  const CLR_RECORD_FIELDREF* GetFieldRef(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(CLR_RECORD_FIELDREF, TBL_FieldRef, i);
    }
  const CLR_RECORD_METHODREF* GetMethodRef(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(CLR_RECORD_METHODREF, TBL_MethodRef, i);
    }
  const CLR_RECORD_TYPEDEF* GetTypeDef(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(CLR_RECORD_TYPEDEF, TBL_TypeDef, i);
    }
  const CLR_RECORD_FIELDDEF* GetFieldDef(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(CLR_RECORD_FIELDDEF, TBL_FieldDef, i);
    }
  const CLR_RECORD_METHODDEF* GetMethodDef(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(CLR_RECORD_METHODDEF, TBL_MethodDef, i);
    }
  const CLR_RECORD_ATTRIBUTE* GetAttribute(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(CLR_RECORD_ATTRIBUTE, TBL_Attributes, i);
    }
  const CLR_RECORD_TYPESPEC* GetTypeSpec(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(CLR_RECORD_TYPESPEC, TBL_TypeSpec, i);
    }
  const CLR_RECORD_RESOURCE_FILE* GetResourceFile(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(CLR_RECORD_RESOURCE_FILE, TBL_ResourcesFiles, i);
    }
  const CLR_RECORD_RESOURCE* GetResource(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(CLR_RECORD_RESOURCE, TBL_Resources, i);
    }
  CLR_PMETADATA GetResourceData(uint32_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(uint8_t, TBL_ResourcesData, i);
    }
  const char* GetString(uint16_t i);
  CLR_PMETADATA GetSignature(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(uint8_t, TBL_Signatures, i);
    }
  CLR_PMETADATA GetByteCode(uint16_t i)
    {
    return NANOCLR_ASSEMBLY_RESOLVE(uint8_t, TBL_ByteCode, i);
    }
#undef NANOCLR_ASSEMBLY_RESOLVE

  //--//

#undef DECL_POSTFIX
#if defined(NANOCLR_TRACE_INSTRUCTIONS)
#define DECL_POSTFIX
#else
#define DECL_POSTFIX                                                                                                   \
    {                                                                                                                  \
    }
#endif

  public:
    void DumpOpcode(CLR_RT_StackFrame* stack, CLR_PMETADATA ip) DECL_POSTFIX;
    void DumpOpcodeDirect(CLR_RT_MethodDef_Instance& call, CLR_PMETADATA ip, CLR_PMETADATA ipStart, int pid)
      DECL_POSTFIX;

  private:
    void DumpToken(uint32_t tk) DECL_POSTFIX;
    void DumpSignature(uint16_t sig) DECL_POSTFIX;
    void DumpSignature(CLR_PMETADATA& p) DECL_POSTFIX;
    void DumpSignatureToken(CLR_PMETADATA& p) DECL_POSTFIX;

    //--//

#if defined(_WIN32)
    static FILE* s_output;
    static FILE* s_toclose;

  public:
    static void Dump_SetDevice(FILE*& outputDeviceFile, const wchar_t* szFileName);
    static void Dump_SetDevice(const wchar_t* szFileName);

    static void Dump_CloseDevice(FILE*& outputDeviceFile);
    static void Dump_CloseDevice();

    static void Dump_Printf(FILE* outputDeviceFile, const char* format, ...);
    static void Dump_Printf(const char* format, ...);

    static void Dump_Indent(const CLR_RECORD_METHODDEF* md, size_t offset, size_t level);

    void Dump(bool fNoByteCode);

    unsigned int GenerateSignatureForNativeMethods();

    bool AreInternalMethodsPresent(const CLR_RECORD_TYPEDEF* td);
    void GenerateSkeleton(const wchar_t* szFileName, const wchar_t* szProjectName);
    void GenerateSkeletonFromComplientNames(const wchar_t* szFileName, const wchar_t* szProjectName);

    void BuildParametersList(CLR_PMETADATA pMetaData, CLR_RT_VectorOfManagedElements& elemPtrArray);
    void GenerateSkeletonStubFieldsDef(
      const CLR_RECORD_TYPEDEF* pClsType,
      FILE* pFileStubHead,
      std::string strIndent,
      std::string strMngClassName);
    void GenerateSkeletonStubCode(const wchar_t* szFilePath, FILE* pFileDotNetProj);

    void BuildMethodName_NoInterop(const CLR_RECORD_METHODDEF* md, std::string& name, CLR_RT_StringMap& mapMethods);
    void GenerateSkeleton_NoInterop(LPCWSTR szFileName, LPCWSTR szProjectName);

    void BuildMethodName(const CLR_RECORD_METHODDEF* md, std::string& name, CLR_RT_StringMap& mapMethods);
    void BuildClassName(const CLR_RECORD_TYPEREF* tr, std::string& cls_name, bool fEscape);
    void BuildClassName(const CLR_RECORD_TYPEDEF* td, std::string& cls_name, bool fEscape);
    void BuildTypeName(const CLR_RECORD_TYPEDEF* td, std::string& type_name);

#endif
  private:
#if defined(_WIN32)
    void Dump_Token(uint32_t tk);
    void Dump_FieldOwner(uint32_t idx);
    void Dump_MethodOwner(uint32_t idx);
    void Dump_Signature(uint16_t sig);
    void Dump_Signature(CLR_PMETADATA& p);
    void Dump_SignatureToken(CLR_PMETADATA& p);
#endif

    //--//

    PROHIBIT_ALL_CONSTRUCTORS(CLR_RT_Assembly);

    //--//

  private:
    uint32_t ComputeHashForName(uint32_t td, uint32_t hash);

    static uint32_t ComputeHashForType(CLR_DataType dt, uint32_t hash);
  };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

//--//

#if defined(NANOCLR_APPDOMAINS)

struct CLR_RT_AppDomain : public CLR_RT_ObjectToEvent_Destination // EVENT HEAP - NO RELOCATION -
  {
  enum AppDomainState
    {
    AppDomainState_Loaded,
    AppDomainState_Unloading,
    AppDomainState_Unloaded
    };

  AppDomainState m_state;
  int m_id;
  CLR_RT_DblLinkedList m_appDomainAssemblies;
  CLR_RT_HeapBlock* m_globalLock;                          // OBJECT HEAP - DO RELOCATION -
  CLR_RT_HeapBlock_String* m_strName;                      // OBJECT HEAP - DO RELOCATION -
  CLR_RT_HeapBlock* m_outOfMemoryException;                // OBJECT HEAP - DO RELOCATION -
  CLR_RT_AppDomainAssembly* m_appDomainAssemblyLastAccess; // EVENT HEAP  - NO RELOCATION -
  bool m_fCanBeUnloaded;

  static HRESULT CreateInstance(const char* szName, CLR_RT_AppDomain*& appDomain);

  void DestroyInstance();
  void AppDomain_Initialize();
  void AppDomain_Uninitialize();
  bool IsLoaded();

  void Relocate();
  void RecoverFromGC();

  CLR_RT_AppDomainAssembly* FindAppDomainAssembly(CLR_RT_Assembly* assm);

  HRESULT MarshalObject(CLR_RT_HeapBlock& src, CLR_RT_HeapBlock& dst, CLR_RT_AppDomain* appDomainSrc = NULL);
  HRESULT MarshalParameters(
    CLR_RT_HeapBlock* callerArgs,
    CLR_RT_HeapBlock* calleeArgs,
    int count,
    bool fOnReturn,
    CLR_RT_AppDomain* appDomainSrc = NULL);

  HRESULT VerifyTypeIsLoaded(uint32_t idx);
  HRESULT GetAssemblies(CLR_RT_HeapBlock& ref);
  HRESULT LoadAssembly(CLR_RT_Assembly* assm);
  HRESULT GetManagedObject(CLR_RT_HeapBlock& obj);
  };

struct CLR_RT_AppDomainAssembly : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  static const uint32_t StaticConstructorsExecuted = 0x00000001;

  uint32_t m_flags;
  CLR_RT_AppDomain* m_appDomain;     // EVENT HEAP - NO RELOCATION -
  CLR_RT_Assembly* m_assembly;       // EVENT HEAP - NO RELOCATION -
  CLR_RT_HeapBlock* m_pStaticFields; // EVENT HEAP - NO RELOCATION - (but the data they point to has to be relocated)

  static HRESULT CreateInstance(
    CLR_RT_AppDomain* appDomain,
    CLR_RT_Assembly* assm,
    CLR_RT_AppDomainAssembly*& appDomainAssembly);

  void DestroyInstance();
  HRESULT AppDomainAssembly_Initialize(CLR_RT_AppDomain* appDomain, CLR_RT_Assembly* assm);

  void Relocate();
  };

#endif // NANOCLR_APPDOMAINS

//--//

struct CLR_RT_WellKnownTypes
  {
  uint32_t m_Boolean;
  uint32_t m_Int8;
  uint32_t m_UInt8;

  uint32_t m_Char;
  uint32_t m_Int16;
  uint32_t m_UInt16;

  uint32_t m_Int32;
  uint32_t m_UInt32;
  uint32_t m_Single;

  uint32_t m_Int64;
  uint32_t m_UInt64;
  uint32_t m_Double;
  uint32_t m_DateTime;
  uint32_t m_TimeSpan;
  uint32_t m_String;

  uint32_t m_Void;
  uint32_t m_Object;
  uint32_t m_ValueType;
  uint32_t m_Enum;

  uint32_t m_AppDomainUnloadedException;
  uint32_t m_ArgumentNullException;
  uint32_t m_ArgumentException;
  uint32_t m_ArgumentOutOfRangeException;
  uint32_t m_Exception;
  uint32_t m_IndexOutOfRangeException;
  uint32_t m_ThreadAbortException;
  uint32_t m_InvalidOperationException;
  uint32_t m_InvalidCastException;
  uint32_t m_NotSupportedException;
  uint32_t m_NotImplementedException;
  uint32_t m_NullReferenceException;
  uint32_t m_OutOfMemoryException;
  uint32_t m_ObjectDisposedException;
  uint32_t m_ConstraintException;
  uint32_t m_WatchdogException;

  uint32_t m_Delegate;
  uint32_t m_MulticastDelegate;

  uint32_t m_Array;
  uint32_t m_ArrayList;
  uint32_t m_ICloneable;
  uint32_t m_IList;

  uint32_t m_Assembly;
  uint32_t m_TypeStatic;
  uint32_t m_Type;
  uint32_t m_ConstructorInfo;
  uint32_t m_MethodInfo;
  uint32_t m_FieldInfo;

  uint32_t m_WeakReference;

  uint32_t m_SerializationHintsAttribute;
  uint32_t m_Bitmap;
  uint32_t m_Font;

  uint32_t m_TouchEvent;
  uint32_t m_TouchInput;

  uint32_t m_Thread;
  uint32_t m_ResourceManager;

  PROHIBIT_COPY_CONSTRUCTORS(CLR_RT_WellKnownTypes);
  };

extern CLR_RT_WellKnownTypes g_CLR_RT_WellKnownTypes;

struct CLR_RT_WellKnownMethods
  {
  uint32_t m_ResourceManager_GetObjectFromId;
  uint32_t m_ResourceManager_GetObjectChunkFromId;

  PROHIBIT_COPY_CONSTRUCTORS(CLR_RT_WellKnownMethods);
  };

extern CLR_RT_WellKnownMethods g_CLR_RT_WellKnownMethods;

//--//

typedef void (CLR_RT_HeapBlock::* CLR_RT_HeapBlockRelocate)();

struct CLR_RT_DataTypeLookup
  {
  static const uint8_t c_NA = 0x00;
  static const uint8_t c_VariableSize = 0xFF;

  static const uint32_t c_Primitive = 0x00000001;
  static const uint32_t c_Interface = 0x00000002;
  static const uint32_t c_Class = 0x00000004;
  static const uint32_t c_ValueType = 0x00000008;
  static const uint32_t c_Enum = 0x00000010;
  static const uint32_t c_SemanticMask = 0x0000001F;

  static const uint32_t c_Array = 0x00000020;
  static const uint32_t c_ArrayList = 0x00000040;
  static const uint32_t c_SemanticMask2 = 0x0000007F;

  static const uint32_t c_Reference = 0x00010000;
  static const uint32_t c_Numeric = 0x00020000;
  static const uint32_t c_Integer = 0x00040000;
  static const uint32_t c_Signed = 0x00080000;
  static const uint32_t c_Direct = 0x00100000;             // This isn't an indirect reference.
  static const uint32_t c_OptimizedValueType = 0x00200000; // A value type that is kept in a single HeapBlock.
  static const uint32_t c_ManagedType = 0x00400000; // this dt represents a managed type, or a pointer to a managed
                                                      // type More specificly, TypeDescriptor::InitializeFromObject
                                                      // will succeed when starting from an object of with this dt

  uint32_t m_flags;
  uint8_t m_sizeInBits;
  uint8_t m_sizeInBytes;
  uint8_t m_promoteTo;
  uint8_t m_convertToElementType;

  uint32_t* m_cls;
  CLR_RT_HeapBlockRelocate m_relocate;

#if defined(_WIN32) || defined(NANOCLR_TRACE_MEMORY_STATS)
  const char* m_name;
#endif
  };

extern const CLR_RT_DataTypeLookup c_CLR_RT_DataTypeLookup[];

//--//

struct CLR_RT_OpcodeLookup
  {
  static const uint16_t COND_BRANCH_NEVER = 0x0000;
  static const uint16_t COND_BRANCH_ALWAYS = 0x0001;
  static const uint16_t COND_BRANCH_IFTRUE = 0x0002;
  static const uint16_t COND_BRANCH_IFFALSE = 0x0003;
  static const uint16_t COND_BRANCH_IFEQUAL = 0x0004;
  static const uint16_t COND_BRANCH_IFNOTEQUAL = 0x0005;
  static const uint16_t COND_BRANCH_IFGREATER = 0x0006;
  static const uint16_t COND_BRANCH_IFGREATEROREQUAL = 0x0007;
  static const uint16_t COND_BRANCH_IFLESS = 0x0008;
  static const uint16_t COND_BRANCH_IFLESSOREQUAL = 0x0009;
  static const uint16_t COND_BRANCH_IFMATCH = 0x000A;
  static const uint16_t COND_BRANCH_THROW = 0x000B;
  static const uint16_t COND_BRANCH_MASK = 0x000F;

  static const uint16_t COND_OVERFLOW = 0x0010;
  static const uint16_t COND_UNSIGNED = 0x0020;

  static const uint16_t STACK_RESET = 0x0080;

  static const uint16_t ATTRIB_HAS_TARGET = 0x0100;
  static const uint16_t ATTRIB_HAS_DT = 0x0200;
  static const uint16_t ATTRIB_HAS_INDEX = 0x0400;
  static const uint16_t ATTRIB_HAS_TOKEN = 0x0800;
  static const uint16_t ATTRIB_HAS_I4 = 0x1000;
  static const uint16_t ATTRIB_HAS_R4 = 0x2000;
  static const uint16_t ATTRIB_HAS_I8 = 0x4000;
  static const uint16_t ATTRIB_HAS_R8 = 0x8000;

#if defined(NANOCLR_OPCODE_NAMES)
  const char* m_name;
#endif

#if defined(NANOCLR_OPCODE_STACKCHANGES)
  uint8_t m_stackChanges;
#endif

  CLR_OpcodeParam m_opParam;

  //--//

#if defined(NANOCLR_OPCODE_PARSER)
  CLR_LOGICAL_OPCODE m_logicalOpcode;
  CLR_DataType m_dt;
  int8_t m_index;
  uint16_t m_flags;
#endif
#if defined(NANOCLR_OPCODE_STACKCHANGES)
  uint32_t StackPop() const
    {
    return m_stackChanges >> 4;
    }
  uint32_t StackPush() const
    {
    return m_stackChanges & 0xF;
    }
  int32_t StackChanges() const
    {
    return StackPush() - StackPop();
    }
#endif

#if defined(NANOCLR_OPCODE_NAMES)
  const char* Name() const
    {
    return m_name;
    }
#else
  const char* Name() const
    {
    return "";
    }
#endif
  };

extern const CLR_RT_OpcodeLookup c_CLR_RT_OpcodeLookup[];

//--//

struct CLR_RT_LogicalOpcodeLookup
  {
  static const uint32_t RESTARTPOINT_NEXT = 0x00000001;
  static const uint32_t EXCEPTION = 0x00000002;
  static const uint32_t EXCEPTION_IF_OVERFLOW = 0x00000010;
  static const uint32_t EXCEPTION_IF_ZERO = 0x00000020;
  static const uint32_t EXCEPTION_ON_CAST = 0x00000040;

#if defined(NANOCLR_OPCODE_NAMES)
  const char* m_name;
#endif

  uint32_t m_flags;

#if defined(NANOCLR_OPCODE_NAMES)
  const char* Name() const
    {
    return m_name;
    }
#else
  const char* Name() const
    {
    return "";
    }
#endif
  };

extern const CLR_RT_LogicalOpcodeLookup c_CLR_RT_LogicalOpcodeLookup[];

//--//

struct CLR_RT_TypeSystem // EVENT HEAP - NO RELOCATION -
  {
  struct CompatibilityLookup
    {
    const char* name;
    CLR_RECORD_VERSION version;
    };

  //--//

  static const int c_MaxAssemblies = 64;

  //--//

  static const uint32_t TYPENAME_FLAGS_FULL = 0x1;
  static const uint32_t TYPENAME_NESTED_SEPARATOR_DOT = 0x2;

  //--//

  CLR_RT_Assembly* m_assemblies[c_MaxAssemblies]; // EVENT HEAP - NO RELOCATION - array of CLR_RT_Assembly
  size_t m_assembliesMax;
  CLR_RT_Assembly* m_assemblyMscorlib;

  uint32_t m_entryPoint;

  //--//

  void TypeSystem_Initialize();
  void TypeSystem_Cleanup();

  void Link(CLR_RT_Assembly* assm);
  void PostLinkageProcessing(CLR_RT_Assembly* assm);

  HRESULT ResolveAll();
  HRESULT PrepareForExecution();
  HRESULT PrepareForExecutionHelper(const char* szAssembly);

  CLR_RT_Assembly* FindAssembly(const char* name, const CLR_RECORD_VERSION* ver, bool fExact);

  bool FindTypeDef(const char* name, const char* nameSpace, CLR_RT_Assembly* assm, uint32_t& res);
  bool FindTypeDef(const char* name, CLR_RT_Assembly* assm, uint32_t& res);
  bool FindTypeDef(const char* name, const char* nameSpace, uint32_t& res);
  bool FindTypeDef(uint32_t hash, uint32_t& res);
  bool FindTypeDef(const char* name, uint32_t& res);
  bool FindTypeDef(const char* name, CLR_RT_Assembly* assm, CLR_RT_ReflectionDef_Index& reflex);

  HRESULT LocateResourceFile(CLR_RT_Assembly_Instance assm, const char* name, int32_t& idxResourceFile);
  HRESULT LocateResource(
    CLR_RT_Assembly_Instance assm,
    int32_t idxResourceFile,
    int16_t id,
    const CLR_RECORD_RESOURCE*& res,
    uint32_t& size);

  HRESULT BuildTypeName(
    uint32_t cls,
    char *szBuffer,
    size_t size,
    uint32_t flags,
    uint32_t levels);
  HRESULT BuildTypeName(uint32_t cls, char *szBuffer, size_t size);
  HRESULT BuildMethodName(uint32_t md, char *szBuffer, size_t size);
  HRESULT BuildFieldName(uint32_t fd, char *szBuffer, size_t size);
  HRESULT QueueStringToBuffer(char *szBuffer, size_t  size, const char* szText);

  bool FindVirtualMethodDef(
    uint32_t cls,
    uint32_t calleeMD,
    uint32_t& idx);
  bool FindVirtualMethodDef(
    uint32_t cls,
    uint32_t calleeMD,
    const char* calleeName,
    uint32_t& idx);

  static bool MatchSignature(CLR_RT_SignatureParser& parserLeft, CLR_RT_SignatureParser& parserRight);
  static bool MatchSignatureDirect(
    CLR_RT_SignatureParser& parserLeft,
    CLR_RT_SignatureParser& parserRight,
    bool fIsInstanceOfOK);
  static bool MatchSignatureElement(
    CLR_RT_SignatureParser::Element& resLeft,
    CLR_RT_SignatureParser::Element& resRight,
    bool fIsInstanceOfOK);

  static CLR_DataType MapElementTypeToDataType(uint32_t et);
  static uint32_t MapDataTypeToElementType(CLR_DataType dt);

#if defined(_WIN32)
  void Dump(const wchar_t* szFileName, bool fNoByteCode);
#endif

  //--//

  PROHIBIT_COPY_CONSTRUCTORS(CLR_RT_TypeSystem);
  };

extern CLR_RT_TypeSystem g_CLR_RT_TypeSystem;

//--//

struct CLR_RT_Assembly_Instance
  {
  uint32_t m_index;
  CLR_RT_Assembly* m_assm;

  //--//

  bool InitializeFromIndex(uint32_t idx);
  void Clear();
  };

struct CLR_RT_TypeSpec_Instance
  {
  uint32_t m_index;
  CLR_RT_Assembly* m_assm;
  CLR_PMETADATA m_target;

  //--//

  bool InitializeFromIndex(uint32_t idx);
  void Clear();

  bool ResolveToken(uint32_t tk, CLR_RT_Assembly* assm);
  };

//--//

struct CLR_RT_TypeDef_Instance
  {
  uint32_t m_index;
  CLR_RT_Assembly* m_assm;
  const CLR_RECORD_TYPEDEF* m_target;

  //--//

  bool InitializeFromReflection(const CLR_RT_ReflectionDef_Index& reflex, uint32_t* levels);
  bool InitializeFromIndex(uint32_t idx);
  bool InitializeFromMethod(const CLR_RT_MethodDef_Instance& md);
  bool InitializeFromField(const CLR_RT_FieldDef_Instance& fd);

  void Clear();

  bool ResolveToken(uint32_t tk, CLR_RT_Assembly* assm);

  //--//

  CLR_RT_TypeDef_CrossReference& CrossReference() const
    {
    return m_assm->m_pCrossReference_TypeDef[ get_type_index(m_index) ];
    }

  bool SwitchToParent();
  bool HasFinalizer() const;

  bool IsATypeHandler();
  };

//--//

struct CLR_RT_FieldDef_Instance
  {
  uint32_t m_index;
  CLR_RT_Assembly* m_assm;
  const CLR_RECORD_FIELDDEF* m_target;

  //--//

  bool InitializeFromIndex(uint32_t idx);
  void Clear();

  bool ResolveToken(uint32_t tk, CLR_RT_Assembly* assm);

  //--//

  CLR_RT_FieldDef_CrossReference& CrossReference() const
    {
    return m_assm->m_pCrossReference_FieldDef[ get_field_index(m_index) ];
    }
  };

//--//

struct CLR_RT_MethodDef_Instance
  {
  uint32_t m_index;
  CLR_RT_Assembly* m_assm;
  const CLR_RECORD_METHODDEF* m_target;

  //--//

  bool InitializeFromIndex(uint32_t idx);
  void Clear();

  bool ResolveToken(uint32_t tk, CLR_RT_Assembly* assm);

  //--//

  CLR_RT_MethodDef_CrossReference& CrossReference() const
    {
    return m_assm->m_pCrossReference_MethodDef[ get_method_index(m_index) ];
    }
  uint32_t Hits() const
    {
    return 0;
    }

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  CLR_RT_MethodDef_DebuggingInfo& DebuggingInfo() const
    {
    return m_assm->m_pDebuggingInfo_MethodDef[ get_method_index(m_index) ];
    }
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  };

////////////////////////////////////////////////////////////////////////////////////////////////////

struct CLR_RT_AttributeEnumerator
  {
  CLR_RT_Assembly* m_assm;
  const CLR_RECORD_ATTRIBUTE* m_ptr;
  int m_num;
  CLR_RECORD_ATTRIBUTE m_data;

  uint32_t m_match;
  CLR_PMETADATA m_blob;

  void Initialize(const CLR_RT_TypeDef_Instance& inst);
  void Initialize(const CLR_RT_FieldDef_Instance& inst);
  void Initialize(const CLR_RT_MethodDef_Instance& inst);

  bool Advance();

  bool MatchNext(const CLR_RT_TypeDef_Instance* instTD, const CLR_RT_MethodDef_Instance* instMD);

  void GetCurrent(CLR_RT_TypeDef_Instance* instTD);

  private:
    void Initialize(CLR_RT_Assembly* assm);
  };

struct CLR_RT_AttributeParser
  {
  struct Value
    {
    static const int c_ConstructorArgument = 1;
    static const int c_NamedField = 2;
    static const int c_NamedProperty = 3;
    static const int c_DefaultConstructor = 4;

    int m_mode;
    CLR_RT_HeapBlock m_value;

    int m_pos;
    const char* m_name;
    };

  //--//

  CLR_RT_Assembly* m_assm;
  CLR_PMETADATA m_blob;

  CLR_RT_MethodDef_Instance m_methodDefInstance;
  uint32_t m_methodIndex;
  CLR_RT_TypeDef_Instance m_typeDefInstance;
  CLR_RT_SignatureParser m_parser;
  CLR_RT_SignatureParser::Element m_res;

  int m_currentPos;
  int m_fixed_Count;
  int m_named_Count;
  Value m_lastValue;
  bool m_constructorParsed;

  //--//

  HRESULT Initialize(const CLR_RT_AttributeEnumerator& en);

  HRESULT Next(Value*& res);

  private:
    const char* GetString();
  };

////////////////////////////////////////////////////////////////////////////////

struct CLR_RT_TypeDescriptor
  {
  uint32_t m_flags;
  CLR_RT_TypeDef_Instance m_handlerCls;

  CLR_RT_ReflectionDef_Index m_reflex;

  CLR_DataType GetDataType() const
    {
    return (CLR_DataType)m_handlerCls.m_target->dataType;
    }

  //--//

  void TypeDescriptor_Initialize();

  HRESULT InitializeFromDataType(CLR_DataType dt);
  HRESULT InitializeFromReflection(const CLR_RT_ReflectionDef_Index& reflex);
  HRESULT InitializeFromTypeSpec(uint32_t sig);
  HRESULT InitializeFromType(uint32_t cls);
  HRESULT InitializeFromFieldDefinition(const CLR_RT_FieldDef_Instance& fd);
  HRESULT InitializeFromSignatureParser(CLR_RT_SignatureParser& parser);
  HRESULT InitializeFromObject(const CLR_RT_HeapBlock& ref);

  void ConvertToArray();
  bool ShouldEmitHash();

  bool GetElementType(CLR_RT_TypeDescriptor& sub);

  static HRESULT ExtractTypeIndexFromObject(const CLR_RT_HeapBlock& ref, uint32_t& res);
  static HRESULT ExtractObjectAndDataType(CLR_RT_HeapBlock*& ref, CLR_DataType& dt);
  };

#include "../../CLR/Include/nanoCLR_Runtime__Serialization.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

struct CLR_RT_HeapCluster : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  CLR_RT_DblLinkedList m_freeList; // list of CLR_RT_HeapBlock_Node
  CLR_RT_HeapBlock_Node* m_payloadStart;
  CLR_RT_HeapBlock_Node* m_payloadEnd;

  //--//

  void HeapCluster_Initialize(uint32_t size, uint32_t blockSize); // Memory is not erased by the caller.

  CLR_RT_HeapBlock* ExtractBlocks(uint32_t dataType, uint32_t flags, uint32_t length);

  void RecoverFromGC();

  CLR_RT_HeapBlock_Node* InsertInOrder(CLR_RT_HeapBlock_Node* node, uint32_t size);

  //--//

#undef DECL_POSTFIX
#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_1_HeapBlocksAndUnlink
#define DECL_POSTFIX
#else
#define DECL_POSTFIX                                                                                                   \
    {                                                                                                                  \
    }
#endif
  void ValidateBlock(CLR_RT_HeapBlock* ptr) DECL_POSTFIX;

  //--//

  PROHIBIT_ALL_CONSTRUCTORS(CLR_RT_HeapCluster);
  };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

//--//

#ifndef CLR_NO_IL_INLINE
struct CLR_RT_InlineFrame
  {
  CLR_RT_HeapBlock* m_locals;
  CLR_RT_HeapBlock* m_args;
  CLR_RT_HeapBlock* m_evalStack;
  CLR_RT_HeapBlock* m_evalPos;
  CLR_RT_MethodDef_Instance m_call;
  CLR_PMETADATA m_IP;
  CLR_PMETADATA m_IPStart;
  };

struct CLR_RT_InlineBuffer
  {
  union {
    CLR_RT_InlineBuffer* m_pNext;

    CLR_RT_InlineFrame m_frame;
    };
  };
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

struct CLR_RT_StackFrame : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  static const int c_OverheadForNewObjOrInteropMethod =
    2; // We need to have more slots in the stack to process a 'newobj' opcode.
  static const int c_MinimumStack = 10;

  static const uint32_t c_MethodKind_Native = 0x00000000;
  static const uint32_t c_MethodKind_Interpreted = 0x00000001;
  static const uint32_t c_UNUSED_00000002 = 0x00000002; // c_MethodKind_Jitted
  static const uint32_t c_MethodKind_Mask = 0x00000003;

  static const uint32_t c_NativeProfiled = 0x00000004;
  static const uint32_t c_MethodKind_Inlined = 0x00000008;

  static const uint32_t c_ExecutingConstructor = 0x00000010;
  static const uint32_t c_CompactAndRestartOnOutOfMemory = 0x00000020;
  static const uint32_t c_CallOnPop = 0x00000040;
  static const uint32_t c_CalledOnPop = 0x00000080;

  static const uint32_t c_NeedToSynchronize = 0x00000100;
  static const uint32_t c_PendingSynchronize = 0x00000200;
  static const uint32_t c_Synchronized = 0x00000400;
  static const uint32_t c_UNUSED_00000800 = 0x00000800;

  static const uint32_t c_NeedToSynchronizeGlobally = 0x00001000;
  static const uint32_t c_PendingSynchronizeGlobally = 0x00002000;
  static const uint32_t c_SynchronizedGlobally = 0x00004000;
  static const uint32_t c_UNUSED_00008000 = 0x00008000;

  static const uint32_t c_ExecutingIL = 0x00010000;
  static const uint32_t c_CallerIsCompatibleForCall = 0x00020000;
  static const uint32_t c_CallerIsCompatibleForRet = 0x00040000;
  static const uint32_t c_PseudoStackFrameForFilter = 0x00080000;

  static const uint32_t c_InlineMethodHasReturnValue = 0x00100000;
  static const uint32_t c_UNUSED_00200000 = 0x00200000;
  static const uint32_t c_UNUSED_00400000 = 0x00400000;
  static const uint32_t c_UNUSED_00800000 = 0x00800000;

  static const uint32_t c_UNUSED_01000000 = 0x01000000;
  static const uint32_t c_UNUSED_02000000 = 0x02000000;

  static const uint32_t c_AppDomainMethodInvoke = 0x04000000;
  static const uint32_t c_AppDomainInjectException = 0x08000000;
  static const uint32_t c_AppDomainTransition = 0x10000000;
  static const uint32_t c_InvalidIP = 0x20000000;
  static const uint32_t c_UNUSED_40000000 = 0x40000000;
  static const uint32_t c_HasBreakpoint = 0x80000000;

  static const uint32_t c_ProcessSynchronize =
    c_NeedToSynchronize | c_PendingSynchronize | c_NeedToSynchronizeGlobally | c_PendingSynchronizeGlobally;

  //--//

  ///////////////////////////////////////////////////////////////////////////////////////////
  //
  // These fields have to be aligned
  //
  CLR_RT_Thread* m_owningThread; // EVENT HEAP - NO RELOCATION -
  CLR_RT_HeapBlock* m_evalStack; // EVENT HEAP - NO RELOCATION -
  CLR_RT_HeapBlock* m_arguments; // EVENT HEAP - NO RELOCATION -
  CLR_RT_HeapBlock* m_locals;    // EVENT HEAP - NO RELOCATION -
  CLR_PMETADATA m_IP;            // ANY   HEAP - DO RELOCATION -
  //
  ///////////////////////////////////////////////////////////////////////////////////////////

  CLR_RT_SubThread* m_owningSubThread; // EVENT HEAP - NO RELOCATION -
  uint32_t m_flags;

  CLR_RT_MethodDef_Instance m_call;

  CLR_RT_MethodHandler m_nativeMethod;
  CLR_PMETADATA m_IPstart; // ANY   HEAP - DO RELOCATION -

  CLR_RT_HeapBlock* m_evalStackPos; // EVENT HEAP - NO RELOCATION -
  CLR_RT_HeapBlock* m_evalStackEnd; // EVENT HEAP - NO RELOCATION -

  union {
    uint32_t m_customState;
    void* m_customPointer;
    };

#ifndef CLR_NO_IL_INLINE
  CLR_RT_InlineBuffer* m_inlineFrame;
#endif

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  uint32_t m_depth;
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

#if defined(NANOCLR_PROFILE_NEW_CALLS)
  CLR_PROF_CounterCallChain m_callchain;
#endif

#if defined(NANOCLR_APPDOMAINS)
  CLR_RT_AppDomain* m_appDomain;
#endif

#if defined(ENABLE_NATIVE_PROFILER)
  bool m_fNativeProfiled;
#endif

  CLR_RT_HeapBlock m_extension[1];

  ////////////////////////////////////////

  static HRESULT Push(CLR_RT_Thread* th, const CLR_RT_MethodDef_Instance& callInst, int32_t extraBlocks);

  void Pop();

#ifndef CLR_NO_IL_INLINE
  bool PushInline(
    CLR_PMETADATA& ip,
    CLR_RT_Assembly*& assm,
    CLR_RT_HeapBlock*& evalPos,
    CLR_RT_MethodDef_Instance& calleeInst,
    CLR_RT_HeapBlock* pThis);
  void PopInline();

  void RestoreFromInlineStack();
  void RestoreStack(CLR_RT_InlineFrame& frame);
  void SaveStack(CLR_RT_InlineFrame& frame);
#endif

#if defined(NANOCLR_APPDOMAINS)
  static HRESULT PushAppDomainTransition(
    CLR_RT_Thread* th,
    const CLR_RT_MethodDef_Instance& callInst,
    CLR_RT_HeapBlock* pThis,
    CLR_RT_HeapBlock* pArgs);
  HRESULT PopAppDomainTransition();
#endif

  HRESULT FixCall();
  HRESULT MakeCall(CLR_RT_MethodDef_Instance md, CLR_RT_HeapBlock* blkThis, CLR_RT_HeapBlock* blkArgs, int nArgs);

  HRESULT HandleSynchronized(bool fAcquire, bool fGlobal);

  void SetResult(int32_t val, CLR_DataType dataType);
  void SetResult_I1(uint8_t val);
  void SetResult_I2(int16_t val);
  void SetResult_I4(int32_t val);
  void SetResult_I8(int64_t& val);
  void SetResult_U1(int8_t val);
  void SetResult_U2(uint16_t val);
  void SetResult_U4(uint32_t val);
  void SetResult_U8(uint64_t& val);

#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
  void SetResult_R4(float val);
  void SetResult_R8(double val);
#else
  void SetResult_R4(int32_t val);
  void SetResult_R8(int64_t val);
#endif

  void SetResult_Boolean(bool val);
  void SetResult_Object(CLR_RT_HeapBlock* val);
  HRESULT SetResult_String(const char* val);

  HRESULT SetupTimeoutFromTicks(CLR_RT_HeapBlock& input, int64_t*& output);

  void ConvertResultToBoolean();
  void NegateResult();

  HRESULT NotImplementedStub();

  void Relocate();

  ////////////////////////////////////////

  CLR_RT_HeapBlock& ThisRef() const
    {
    return m_arguments[0];
    }
  CLR_RT_HeapBlock* This() const
    {
    return ThisRef().Dereference();
    }

  CLR_RT_HeapBlock& Arg0() const
    {
    return m_arguments[0];
    }
  CLR_RT_HeapBlock& Arg1() const
    {
    return m_arguments[1];
    }
  CLR_RT_HeapBlock& Arg2() const
    {
    return m_arguments[2];
    }
  CLR_RT_HeapBlock& Arg3() const
    {
    return m_arguments[3];
    }
  CLR_RT_HeapBlock& Arg4() const
    {
    return m_arguments[4];
    }
  CLR_RT_HeapBlock& Arg5() const
    {
    return m_arguments[5];
    }
  CLR_RT_HeapBlock& Arg6() const
    {
    return m_arguments[6];
    }
  CLR_RT_HeapBlock& Arg7() const
    {
    return m_arguments[7];
    }

  CLR_RT_HeapBlock& ArgN(int32_t n) const
    {
    return m_arguments[n];
    }

  CLR_RT_HeapBlock& TopValue()
    {
    return m_evalStackPos[-1];
    }
  CLR_RT_HeapBlock& PushValue()
    {
    _ASSERTE(m_evalStackPos < m_evalStackEnd);
    return *m_evalStackPos++;
    }
  CLR_RT_HeapBlock& PopValue()
    {
    _ASSERTE(m_evalStackPos > m_evalStack);
    return *--m_evalStackPos;
    }
  void ResetStack()
    {
    m_evalStackPos = m_evalStack;
    }
  int TopValuePosition()
    {
    return (int)(m_evalStackPos - m_evalStack);
    }

  CLR_RT_MethodDef_Instance& MethodCall()
    {
    return m_call;
    }

  CLR_RT_HeapBlock& PushValueAndClear()
    {
    CLR_RT_HeapBlock& val = PushValue();

    val.SetObjectReference(NULL);

    return val;
    }

  CLR_RT_HeapBlock& PushValueAndAssign(const CLR_RT_HeapBlock& value)
    {
    CLR_RT_HeapBlock& top = PushValue();

    top.Assign(value);

    return top;
    }

  inline void PushValueI4(int32_t val)
    {
    SetResult_I4(val);
    }

  //--//

  CLR_RT_StackFrame* Caller()
    {
    return (CLR_RT_StackFrame*)Prev();
    }
  CLR_RT_StackFrame* Callee()
    {
    return (CLR_RT_StackFrame*)Next();
    }

  PROHIBIT_ALL_CONSTRUCTORS(CLR_RT_StackFrame);
  };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

//
// This CT_ASSERT macro generates a compiler error in case these fields get out of alignment.
//
// The use of offsetof below throwns an "invalid offset warning" because CLR_RT_StackFrame is not POD type
// C+17 is the first standard that allow this, so until we are using it we have to disable it to keep GCC happy

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

CT_ASSERT(offsetof(CLR_RT_StackFrame, m_owningThread) + sizeof(uint32_t) == offsetof(CLR_RT_StackFrame, m_evalStack))
CT_ASSERT(offsetof(CLR_RT_StackFrame, m_evalStack) + sizeof(uint32_t) == offsetof(CLR_RT_StackFrame, m_arguments))
CT_ASSERT(offsetof(CLR_RT_StackFrame, m_arguments) + sizeof(uint32_t) == offsetof(CLR_RT_StackFrame, m_locals))
CT_ASSERT(offsetof(CLR_RT_StackFrame, m_locals) + sizeof(uint32_t) == offsetof(CLR_RT_StackFrame, m_IP))

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

////////////////////////////////////////////////////////////////////////////////

struct CLR_RT_ProtectFromGC
  {
  static const uint32_t c_Generic = 0x00000001;
  static const uint32_t c_HeapBlock = 0x00000002;
  static const uint32_t c_ResetKeepAlive = 0x00000004;

  typedef void (*Callback)(void* state);

  static CLR_RT_ProtectFromGC* s_first;

  CLR_RT_ProtectFromGC* m_next;
  void** m_data;
  Callback m_fpn;
  uint32_t m_flags;

  CLR_RT_ProtectFromGC(CLR_RT_HeapBlock& ref)
    {
    Initialize(ref);
    }
  CLR_RT_ProtectFromGC(void** data, Callback fpn)
    {
    Initialize(data, fpn);
    }
  ~CLR_RT_ProtectFromGC()
    {
    Cleanup();
    }

  static void InvokeAll();

  private:
    void Initialize(CLR_RT_HeapBlock& ref);
    void Initialize(void** data, Callback fpn);
    void Cleanup();

    void Invoke();
  };

////////////////////////////////////////

#if defined(NANOCLR_TRACE_EARLYCOLLECTION)

struct CLR_RT_AssertEarlyCollection
  {
  static CLR_RT_AssertEarlyCollection* s_first;

  CLR_RT_AssertEarlyCollection* m_next;
  CLR_RT_HeapBlock* m_ptr;

  CLR_RT_AssertEarlyCollection(CLR_RT_HeapBlock* ptr);
  ~CLR_RT_AssertEarlyCollection();

  void Cancel();

  static void CheckAll(CLR_RT_HeapBlock* ptr);
  };

#define NANOCLR_FAULT_ON_EARLY_COLLECTION(ptr) CLR_RT_AssertEarlyCollection aec##ptr(ptr)
#define NANOCLR_CANCEL_EARLY_COLLECTION(ptr)   aec##ptr.Cancel()
#define NANOCLR_CHECK_EARLY_COLLECTION(ptr)    CLR_RT_AssertEarlyCollection::CheckAll(ptr)

#else

#define NANOCLR_FAULT_ON_EARLY_COLLECTION(ptr)
#define NANOCLR_CANCEL_EARLY_COLLECTION(ptr)
#define NANOCLR_CHECK_EARLY_COLLECTION(ptr)

#endif

////////////////////////////////////////

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

struct CLR_RT_GarbageCollector
  {
  typedef bool (*MarkSingleFtn)(CLR_RT_HeapBlock** ptr);
  typedef bool (*MarkMultipleFtn)(CLR_RT_HeapBlock* lstExt, uint32_t numExt);
  typedef bool (*RelocateFtn)(void** ref);

  struct MarkStackElement
    {
    CLR_RT_HeapBlock* ptr;
    uint32_t num;
#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)
    CLR_RT_AppDomain* appDomain;
#endif
    };

  struct MarkStack : CLR_RT_HeapBlock_Node
    {
    MarkStackElement* m_last;
    MarkStackElement* m_top;

    void Initialize(MarkStackElement* ptr, size_t num);
    };

  struct RelocationRegion
    {
    uint8_t* m_start;
    uint8_t* m_end;
    uint8_t* m_destination;
    uint32_t m_offset;
    };

  //--//

  static const int c_minimumSpaceForGC = 128;
  static const int c_minimumSpaceForCompact = 128;
  static const uint32_t c_pressureThreshold = 10;
  static const uint32_t c_memoryThreshold = HEAP_SIZE_THRESHOLD;
  static const uint32_t c_memoryThreshold2 = HEAP_SIZE_THRESHOLD_UPPER;

  static const uint32_t c_StartGraphEvent = 0x00000001;
  static const uint32_t c_StopGraphEvent = 0x00000002;
  static const uint32_t c_DumpGraphHeapEvent = 0x00000004;
  static const uint32_t c_DumpPerfCountersEvent = 0x00000008;

  uint32_t m_numberOfGarbageCollections;
  uint32_t m_numberOfCompactions;

  CLR_RT_DblLinkedList m_weakDelegates_Reachable; // list of CLR_RT_HeapBlock_Delegate_List

  uint32_t m_totalBytes;
  uint32_t m_freeBytes;
  uint32_t m_pressureCounter;

  CLR_RT_DblLinkedList* m_markStackList;
  MarkStack* m_markStack;

  RelocationRegion* m_relocBlocks;
  size_t m_relocTotal;
  size_t m_relocCount;
  uint8_t* m_relocMinimum;
  uint8_t* m_relocMaximum;
#if NANOCLR_VALIDATE_HEAP > NANOCLR_VALIDATE_HEAP_0_None
  RelocateFtn m_relocWorker;
#endif

  MarkSingleFtn m_funcSingleBlock;
  MarkMultipleFtn m_funcMultipleBlocks;

  bool m_fOutOfStackSpaceForGC;

#if defined(_WIN32)
  uint32_t m_events;
#endif

  //--//

  uint32_t ExecuteGarbageCollection();
  uint32_t ExecuteCompaction();

  void Mark();
  void MarkWeak();
  void Sweep();
  void CheckMemoryPressure();

#if defined(NANOCLR_APPDOMAINS)
  void AppDomain_Mark();
#endif

  void Assembly_Mark();

  void Thread_Mark(CLR_RT_DblLinkedList& threads);
  void Thread_Mark(CLR_RT_Thread* thread);

  void Heap_Compact();
  uint32_t Heap_ComputeAliveVsDeadRatio();

  void RecoverEventsFromGC();

  void Heap_Relocate_Prepare(RelocationRegion* blocks, size_t total);
  void Heap_Relocate_AddBlock(uint8_t* dst, uint8_t* src, uint32_t length);
  void Heap_Relocate();

  //--//

  static void Heap_Relocate(CLR_RT_HeapBlock* lst, uint32_t len);
  static void Heap_Relocate(void** ref);

  //--//

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_3_Compaction

  static bool Relocation_JustCheck(void** ref);

  void ValidatePointers()
    {
    Heap_Relocate_Pass(Relocation_JustCheck);
    }

  static void ValidateCluster(CLR_RT_HeapCluster* hc);
  static void ValidateHeap(CLR_RT_DblLinkedList& lst);
  static void ValidateBlockNotInFreeList(CLR_RT_DblLinkedList& lst, CLR_RT_HeapBlock_Node* dst);
  static bool IsBlockInFreeList(CLR_RT_DblLinkedList& lst, CLR_RT_HeapBlock_Node* dst, bool fExact);
  static bool IsBlockInHeap(CLR_RT_DblLinkedList& lst, CLR_RT_HeapBlock_Node* dst);

#else

  void ValidatePointers()
    {
    }

  static void ValidateCluster(CLR_RT_HeapCluster* hc)
    {
    }
  static void ValidateHeap(CLR_RT_DblLinkedList& lst)
    {
    }
  static void ValidateBlockNotInFreeList(CLR_RT_DblLinkedList& lst, CLR_RT_HeapBlock_Node* dst)
    {
    }

#endif

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_4_CompactionPlus

  struct RelocationRecord
    {
    void** oldRef;
    uint32_t* oldPtr;

    void** newRef;
    uint32_t* newPtr;

    uint32_t data;
    };

  typedef std::list<RelocationRecord*> Rel_List;
  typedef Rel_List::iterator Rel_List_Iter;

  typedef std::map<void**, RelocationRecord*> Rel_Map;
  typedef Rel_Map::iterator Rel_Map_Iter;

  static Rel_List s_lstRecords;
  static Rel_Map s_mapOldToRecord;
  static Rel_Map s_mapNewToRecord;

  void Relocation_UpdatePointer(void** ref);

  static bool TestPointers_PopulateOld_Worker(void** ref);
  static bool TestPointers_PopulateNew_Worker(void** ref);

  void TestPointers_PopulateOld();
  void TestPointers_Remap();
  void TestPointers_PopulateNew();

#else

  void TestPointers_PopulateOld()
    {
    }
  void TestPointers_Remap()
    {
    }
  void TestPointers_PopulateNew()
    {
    }

#endif

  //--//

#if defined(NANOCLR_GC_VERBOSE)

  void GC_Stats(int& resNumberObjects, int& resSizeObjects, int& resNumberEvents, int& resSizeEvents);

  void DumpThreads();

#else

  void GC_Stats(int& resNumberObjects, int& resSizeObjects, int& resNumberEvents, int& resSizeEvents)
    {
    resNumberObjects = 0;
    resSizeObjects = 0;
    resNumberEvents = 0;
    resSizeEvents = 0;
    }

  void DumpThreads()
    {
    }

#endif

  //--//

  bool CheckSingleBlock(CLR_RT_HeapBlock** ptr)
    {
    return m_funcSingleBlock((CLR_RT_HeapBlock**)ptr);
    }
  bool CheckSingleBlock(CLR_RT_HeapBlock_Array** ptr)
    {
    return m_funcSingleBlock((CLR_RT_HeapBlock**)ptr);
    }
  bool CheckSingleBlock(CLR_RT_HeapBlock_Delegate** ptr)
    {
    return m_funcSingleBlock((CLR_RT_HeapBlock**)ptr);
    }

  bool CheckSingleBlock_Force(CLR_RT_HeapBlock* ptr)
    {
    return ptr ? m_funcMultipleBlocks(ptr, 1) : true;
    }
  bool CheckMultipleBlocks(CLR_RT_HeapBlock* lst, uint32_t num)
    {
    return lst ? m_funcMultipleBlocks(lst, num) : true;
    }

  //--//

  static bool ComputeReachabilityGraphForSingleBlock(CLR_RT_HeapBlock** ptr);
  static bool ComputeReachabilityGraphForMultipleBlocks(CLR_RT_HeapBlock* lst, uint32_t num);

  //--//

  PROHIBIT_COPY_CONSTRUCTORS(CLR_RT_GarbageCollector);

  //--//

  private:
    void Heap_Relocate_Pass(RelocateFtn ftn);

    void MarkSlow();
  };

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

extern CLR_RT_GarbageCollector g_CLR_RT_GarbageCollector;

//--//

struct CLR_RT_SubThread : public CLR_RT_HeapBlock_Node // EVENT HEAP - NO RELOCATION -
  {
  static const int MODE_IncludeSelf = 0x00000001;
  static const int MODE_CheckLocks = 0x00000002;

  static const int STATUS_Triggered = 0x00000001;

  CLR_RT_Thread* m_owningThread; // EVENT HEAP - NO RELOCATION -
  CLR_RT_StackFrame* m_owningStackFrame;
  uint32_t m_lockRequestsCount;

  int m_priority;
  int64_t m_timeConstraint;
  uint32_t m_status;

  //--//

  static HRESULT CreateInstance(CLR_RT_Thread* th, CLR_RT_StackFrame* stack, int priority, CLR_RT_SubThread*& sth);
  static void DestroyInstance(CLR_RT_Thread* th, CLR_RT_SubThread* sthBase, int flags);

  bool ChangeLockRequestCount(int diff);

  //--//

  PROHIBIT_ALL_CONSTRUCTORS(CLR_RT_SubThread);
  };

//--//

struct CLR_RT_ExceptionHandler
  {
  union {
    uint32_t m_typeFilter;
    CLR_PMETADATA m_userFilterStart;
    };
  CLR_PMETADATA m_tryStart;
  CLR_PMETADATA m_tryEnd;
  CLR_PMETADATA m_handlerStart;
  CLR_PMETADATA m_handlerEnd;

  uint16_t m_ehType;
  //--//

  bool ConvertFromEH(const CLR_RT_MethodDef_Instance& owner, CLR_PMETADATA ipStart, const CLR_RECORD_EH* ehPtr);

  bool IsCatch() const
    {
    return m_ehType == CLR_RECORD_EH::EH_Catch;
    }
  bool IsCatchAll() const
    {
    return m_ehType == CLR_RECORD_EH::EH_CatchAll;
    }
  bool IsFilter() const
    {
    return m_ehType == CLR_RECORD_EH::EH_Filter;
    }
  bool IsFinally() const
    {
    return m_ehType == CLR_RECORD_EH::EH_Finally;
    }
  };

struct CLR_RT_Thread : public CLR_RT_ObjectToEvent_Destination // EVENT HEAP - NO RELOCATION -
  {
  typedef void (*ThreadTerminationCallback)(void* arg);

  struct UnwindStack
    {
    CLR_RT_StackFrame* m_stack;
    CLR_RT_HeapBlock* m_exception;
    CLR_PMETADATA m_ip;

    CLR_PMETADATA m_currentBlockStart;
    CLR_PMETADATA m_currentBlockEnd;
    CLR_PMETADATA m_handlerBlockStart;
    CLR_PMETADATA m_handlerBlockEnd;

    CLR_RT_StackFrame* m_handlerStack;
    uint8_t m_flags;

    static const uint8_t p_Phase_Mask = 0x07;
    static const uint8_t p_1_SearchingForHandler_0 = 0x01;
    static const uint8_t p_1_SearchingForHandler_1_SentFirstChance = 0x02;
    static const uint8_t p_1_SearchingForHandler_2_SentUsersChance = 0x03;
    static const uint8_t p_2_RunningFinallys_0 = 0x04;
    static const uint8_t p_2_RunningFinallys_1_SentUnwindBegin = 0x05;
    static const uint8_t p_3_RunningHandler = 0x06;
    static const uint8_t p_4_NormalCleanup = 0x07;

    static const uint8_t c_MagicCatchForInline = 0x20;
    static const uint8_t c_MagicCatchForInteceptedException = 0x40;
    static const uint8_t c_ContinueExceptionHandler = 0x80;

    uint8_t inline GetPhase()
      {
      return m_flags & p_Phase_Mask;
      }
    void inline SetPhase(uint8_t phase)
      {
      _ASSERTE((phase & ~p_Phase_Mask) == 0);
      m_flags = (m_flags & ~p_Phase_Mask) | phase;
      }
    };

  static const uint32_t TH_S_Ready = 0x00000000;
  static const uint32_t TH_S_Waiting = 0x00000001;
  static const uint32_t TH_S_Terminated = 0x00000002;
  static const uint32_t TH_S_Unstarted = 0x00000003;

  static const uint32_t TH_F_Suspended = 0x00000001;
  static const uint32_t TH_F_Aborted = 0x00000002;
  static const uint32_t TH_F_System = 0x00000004;
  static const uint32_t TH_F_ContainsDoomedAppDomain = 0x00000008;

  static const int32_t TH_WAIT_RESULT_INIT = -1;
  static const int32_t TH_WAIT_RESULT_HANDLE_0 = 0;
  static const int32_t TH_WAIT_RESULT_TIMEOUT = 0x102; // WaitHandle.WaitTimeout
  static const int32_t TH_WAIT_RESULT_HANDLE_ALL = 0x103;

  static const uint32_t c_TimeQuantum_Milliseconds = 20;
  static const int c_MaxStackUnwindDepth = 6;

  int m_pid;
  uint32_t m_status;
  uint32_t m_flags;
  int m_executionCounter;
  volatile bool m_timeQuantumExpired;

  CLR_RT_HeapBlock_Delegate* m_dlg;    // OBJECT HEAP - DO RELOCATION -
  CLR_RT_HeapBlock m_currentException; // OBJECT HEAP - DO RELOCATION -
  UnwindStack m_nestedExceptions[c_MaxStackUnwindDepth];
  int m_nestedExceptionsPos;

  //--//

  //
  // For example, timers are implemented in terms of Threads. If not NULL, this is a worker thread for a timer.
  //
  ThreadTerminationCallback m_terminationCallback;
  void* m_terminationParameter; // EVENT HEAP - NO RELOCATION -

  uint32_t m_waitForEvents;
  int64_t m_waitForEvents_Timeout;
  int64_t m_waitForEvents_IdleTimeWorkItem;

  CLR_RT_DblLinkedList m_locks; // EVENT HEAP - NO RELOCATION - list of CLR_RT_HeapBlock_Lock
  uint32_t m_lockRequestsCount;

  CLR_RT_HeapBlock_WaitForObject* m_waitForObject; // EVENT HEAP - NO RELOCATION, but the objects they point to do
  int32_t m_waitForObject_Result;

  CLR_RT_DblLinkedList m_stackFrames; // EVENT HEAP - NO RELOCATION - list of CLR_RT_StackFrame

  CLR_RT_DblLinkedList m_subThreads; // EVENT HEAP - NO RELOCATION - list of CLR_RT_SubThread

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  int m_scratchPad;
  bool m_fHasJMCStepper;

  CLR_RT_Thread* m_realThread; // Normally, this points to the CLR_RT_Thread object that contains it.
                               // However, if this thread was spawned on behalf of the debugger to evaluate
                               // a property or function call, it points to the object coresponding to the
                               // thread that is currently selected in the debugger.
#endif                           //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

#if defined(ENABLE_NATIVE_PROFILER)
  bool m_fNativeProfiled;
#endif

  //--//

  static HRESULT CreateInstance(
    int pid,
    CLR_RT_HeapBlock_Delegate* pDelegate,
    int priority,
    CLR_RT_Thread*& th,
    uint32_t flags);
  static HRESULT CreateInstance(int pid, int priority, CLR_RT_Thread*& th, uint32_t flags);
  HRESULT PushThreadProcDelegate(CLR_RT_HeapBlock_Delegate* pDelegate);

  void DestroyInstance();

  HRESULT Execute();

  HRESULT Suspend();
  HRESULT Resume();
  HRESULT Terminate();
  HRESULT Abort();

  void Restart(bool fDeleteEvent);

  void Passivate();

  bool CouldBeActivated();

  void RecoverFromGC();

  void Relocate();

  UnwindStack* PushEH();
  void PopEH_Inner(CLR_RT_StackFrame* stack, CLR_PMETADATA ip);
  bool FindEhBlock(
    CLR_RT_StackFrame* stack,
    CLR_PMETADATA from,
    CLR_PMETADATA to,
    CLR_RT_ExceptionHandler& eh,
    bool onlyFinallys);

  HRESULT ProcessException();
  HRESULT ProcessException_EndFilter();
  HRESULT ProcessException_EndFinally();
  HRESULT ProcessException_Phase1();
  HRESULT ProcessException_Phase2();
  void ProcessException_FilterPseudoFrameCopyVars(CLR_RT_StackFrame* to, CLR_RT_StackFrame* from);

  static void ProtectFromGCCallback(void* state);

  static HRESULT Execute_DelegateInvoke(CLR_RT_StackFrame& stack);
  static HRESULT Execute_IL(CLR_RT_StackFrame& stack);

  //--//

  CLR_RT_StackFrame* FirstFrame() const
    {
    return (CLR_RT_StackFrame*)m_stackFrames.FirstNode();
    }
  CLR_RT_StackFrame* CurrentFrame() const
    {
    return (CLR_RT_StackFrame*)m_stackFrames.LastNode();
    }

  CLR_RT_SubThread* CurrentSubThread() const
    {
    return (CLR_RT_SubThread*)m_subThreads.LastNode();
    }

#if defined(NANOCLR_APPDOMAINS)
  CLR_RT_AppDomain* CurrentAppDomain() const
    {
    CLR_RT_StackFrame* stack = CurrentFrame();
    return stack->Prev() ? stack->m_appDomain : NULL;
    }
#endif

  // Just return priority, no way to set it through this function.
  int GetThreadPriority() const
    {
    return CurrentSubThread()->m_priority;
    }
  // Here we set it. This function is called if managed code changes thread priority.
  void SetThreadPriority(int threadPri)
    {
    CurrentSubThread()->m_priority = threadPri;
    }

  int GetExecutionCounter() const
    {
    return CurrentSubThread()->m_priority + m_executionCounter;
    }

  // QuantumDebit is update for execution counter for each quantum:
  // System_Highest       - 1
  // Highest              - 2
  // Above Normal         - 4
  // Normal               - 8
  // Below Normal         - 16
  // Lowest               - 32
  int GetQuantumDebit() const
    {
    return 1 << (5 - GetThreadPriority());
    }

  // If thread was sleeping and get too far behind on updating of m_executionCounter
  // Then we make m_executionCounter 4 quantums above m_GlobalExecutionCounter;
  void BringExecCounterToDate(int iGlobalExecutionCounter);

  void PopEH(CLR_RT_StackFrame* stack, CLR_PMETADATA ip)
    {
    if (m_nestedExceptionsPos)
      PopEH_Inner(stack, ip);
    }

#if defined(NANOCLR_TRACE_CALLS)
  void DumpStack();
#else
  void DumpStack()
    {
    }
#endif

  bool IsFinalizerThread();
  bool ReleaseWhenDeadEx();
  void OnThreadTerminated();
  bool CanThreadBeReused();

  //--//

  PROHIBIT_ALL_CONSTRUCTORS(CLR_RT_Thread);

  //--//

  private:
    HRESULT Execute_Inner();
  };

////////////////////////////////////////////////////////////////////////////////

extern size_t LinkArraySize();
extern size_t LinkMRUArraySize();
extern size_t PayloadArraySize();
#ifndef CLR_NO_IL_INLINE
extern size_t InlineBufferCount();
#endif

extern uint32_t g_scratchVirtualMethodTableLink[];
extern uint32_t g_scratchVirtualMethodTableLinkMRU[];
extern uint32_t g_scratchVirtualMethodPayload[];
extern uint32_t g_scratchInterruptDispatchingStorage[];
#ifndef CLR_NO_IL_INLINE
extern uint32_t g_scratchInlineBuffer[];
#endif

////////////////////////////////////////////////////////////////////////////////

struct CLR_RT_EventCache
  {
  struct BoundedList
    {
    CLR_RT_DblLinkedList m_blocks;
    };

#if defined(NANOCLR_USE_AVLTREE_FOR_METHODLOOKUP)

  struct Payload
    {
    //
    // The first two fields compose the key, the last field is the value.
    //
    uint32_t m_mdVirtual; // The definition of the virtual method.
    uint32_t m_cls;         // The class of the instance we need to resolve.
    uint32_t m_md;        // The actual implementation of the virtual method.

    //--//

    int Compare(Payload& right)
      {
      if (m_mdVirtual.m_data == right.m_mdVirtual.m_data)
        {
        if (m_cls.m_data == right.m_cls.m_data)
          return 0;

        return m_cls.m_data < right.m_cls.m_data ? -1 : 1;
        }

      return m_mdVirtual.m_data < right.m_mdVirtual.m_data ? -1 : 1;
      }
    };

  struct LookupEntry : public CLR_RT_AVLTree::Entry
    {
    Payload m_payload;

    //--//

    static int Callback_Compare(void* state, CLR_RT_AVLTree::Entry* left, CLR_RT_AVLTree::Entry* right);
    };

  struct VirtualMethodTable
    {
    CLR_RT_AVLTree m_tree;
    LookupEntry* m_entries;

    CLR_RT_DblLinkedList m_list_freeItems; // list of CLR_RT_EventCache::LookupEntry
    CLR_RT_DblLinkedList m_list_inUse;     // list of CLR_RT_EventCache::LookupEntry

    //--//

    void Initialize();

    bool FindVirtualMethod(
      uint32_t cls,
      uint32_t mdVirtual,
      uint32_t& md);

    //--//

    static CLR_RT_AVLTree::Entry* Callback_NewNode(void* state, CLR_RT_AVLTree::Entry* payload);
    static void Callback_FreeNode(void* state, CLR_RT_AVLTree::Entry* node);
    static void Callback_Reassign(void* state, CLR_RT_AVLTree::Entry* from, CLR_RT_AVLTree::Entry* to);

    //--//

#if defined(_WIN32)
    void DumpTree();
    bool ConsistencyCheck();
    bool ConsistencyCheck(LookupEntry* node, int& depth);
#else
    void DumpTree()
      {
      }
    bool ConsistencyCheck()
      {
      return true;
      }
#endif
    };

#else

  struct Link
    {
    uint16_t m_next;
    uint16_t m_prev;
    };

  struct Payload
    {
    struct Key
      {
      uint32_t m_mdVirtual; // The definition of the virtual method.
      uint32_t m_cls;         // The class of the instance we need to resolve.
      };

    Key m_key;
    uint32_t m_md; // The actual implementation of the virtual method.
    };

  struct VirtualMethodTable
    {
    Link* m_entries;
    Link* m_entriesMRU;
    Payload* m_payloads;

    //--//

    void Initialize();

    bool FindVirtualMethod(
      uint32_t cls,
      uint32_t mdVirtual,
      uint32_t& md);

    private:
      uint32_t GetNewEntry()
        {
        return m_entriesMRU[LinkMRUArraySize() - 1].m_prev;
        }

      static void MoveEntryToTop(Link* entries, uint32_t slot, uint32_t idx);
    };
#endif

  //--//

  static const uint16_t c_maxFastLists = 40;

  // the scratch array is used to avoid bringing in arm ABI methods (for semihosting)
  // struct arrays require initialization with the v3.0 compiler and this is done with ABI methods,
  // unless of course you provide a work around lik this ;-)
  unsigned int m_scratch[(sizeof(BoundedList) * c_maxFastLists + 3) / sizeof(unsigned int)];
  BoundedList* m_events;

  VirtualMethodTable m_lookup_VirtualMethod;
#ifndef CLR_NO_IL_INLINE
  CLR_RT_InlineBuffer* m_inlineBufferStart;
#endif

  //--//

  void EventCache_Initialize();
  uint32_t EventCache_Cleanup();

  void Append_Node(CLR_RT_HeapBlock* node);
  CLR_RT_HeapBlock* Extract_Node_Slow(uint32_t dataType, uint32_t flags, uint32_t blocks);
  CLR_RT_HeapBlock* Extract_Node_Fast(uint32_t dataType, uint32_t flags, uint32_t blocks);
  CLR_RT_HeapBlock* Extract_Node_Bytes(uint32_t dataType, uint32_t flags, uint32_t bytes);
  CLR_RT_HeapBlock* Extract_Node(uint32_t dataType, uint32_t flags, uint32_t blocks);

  bool FindVirtualMethod(
    uint32_t cls,
    uint32_t mdVirtual,
    uint32_t *md);

#ifndef CLR_NO_IL_INLINE
  bool GetInlineFrameBuffer(CLR_RT_InlineBuffer** ppBuffer);
  bool FreeInlineBuffer(CLR_RT_InlineBuffer* pBuffer);
#endif

  //--//

#define EVENTCACHE_EXTRACT_NODE_AS_BYTES(ev, cls, type, flags, size)  (cls *)((ev).Extract_Node_Bytes(type, flags, size))
#define EVENTCACHE_EXTRACT_NODE_AS_BLOCKS(ev, cls, type, flags, size) (cls *)((ev).Extract_Node(type, flags, size))
#define EVENTCACHE_EXTRACT_NODE(ev, cls, type)                                                                         \
    EVENTCACHE_EXTRACT_NODE_AS_BLOCKS(ev, cls, type, 0, CONVERTFROMSIZETOHEAPBLOCKS(sizeof(cls)))
#define EVENTCACHE_EXTRACT_NODE_NOALLOC(ev, cls, type)                                                                 \
    EVENTCACHE_EXTRACT_NODE_AS_BLOCKS(                                                                                 \
        ev,                                                                                                            \
        cls,                                                                                                           \
        type,                                                                                                          \
        CLR_RT_HeapBlock::HB_NoGcOnFailure,                                                                            \
        CONVERTFROMSIZETOHEAPBLOCKS(sizeof(cls)))
#define EVENTCACHE_EXTRACT_NODE_INITTOZERO(ev, cls, type)                                                              \
    EVENTCACHE_EXTRACT_NODE_AS_BLOCKS(                                                                                 \
        ev,                                                                                                            \
        cls,                                                                                                           \
        type,                                                                                                          \
        CLR_RT_HeapBlock::HB_InitializeToZero,                                                                         \
        CONVERTFROMSIZETOHEAPBLOCKS(sizeof(cls)))

    //--//

  PROHIBIT_COPY_CONSTRUCTORS(CLR_RT_EventCache);
  };

extern CLR_RT_EventCache g_CLR_RT_EventCache;

//////////////////////////////////////////////////////////////////////////////////////
// keep under control the size of the Link and Payload, since we will use externally
// defined arrays to handle those data structures in the Virtual Method cache

#if defined(NANOCLR_USE_AVLTREE_FOR_METHODLOOKUP)
CT_ASSERT(sizeof(CLR_RT_EventCache::LookupEntry) == 12)
#else
CT_ASSERT(sizeof(CLR_RT_EventCache::Link) == 4)
CT_ASSERT(sizeof(CLR_RT_EventCache::Payload) == 12)
#endif

#if defined(NANOCLR_TRACE_STACK)

//
// If this is set, no memory allocation should be allowed, it could lead to a GC while in an inconsistent state!!
//
extern bool g_CLR_RT_fBadStack;

#endif

//--//
typedef enum Events
  {
  Event_SerialPortIn = 0x00000002,
  Event_SerialPortOut = 0x00000004,
  Event_EndPoint = 0x00000008,
  Event_StorageIo = 0x00000020,
  Event_I2cMaster = 0x00000080,
  Event_SpiMaster = 0x00000100,
  Event_OneWireMaster = 0x00000200,
  Event_Radio = 0x00000400,
  Event_AppDomain = 0x02000000,
  Event_Socket = 0x20000000,
  Event_IdleCPU = 0x40000000,
  Event_LowMemory = 0x80000000,
  } Events;

struct CLR_RT_ExecutionEngine
  {
  ////////////////////////////////////////////////////////////////////////////////////////////////

  static const uint32_t c_Compile_CPP = 0x00000001;
  static const uint32_t c_Compile_ARM = 0x00000002;

  ////////////////////////////////////////////////////////////////////////////////////////////////

  static const int c_HeapState_Normal = 0x00000000;
  static const int c_HeapState_UnderGC = 0x00000001;

  volatile int m_iDebugger_Conditions;

  ////////////////////////////////////////////////////////////////////////////////////////////////

  static const int c_fExecution_GC_Pending = 0x00000001; // Not currently used
  static const int c_fExecution_Compaction_Pending = 0x00000002;

  int m_iExecution_Conditions;

  int m_iReboot_Options;

  int m_iProfiling_Conditions;

  static const int c_MaxBreakpointsActive = 5;

  size_t m_breakpointsNum;
  execution_breakpoint_t *m_breakpoints;
 
  void StopOnBreakpoint(execution_breakpoint_t *def, CLR_RT_Thread *th);

#define CLR_EE_DBG_IS(Cond)                                                                                            \
    ((g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions & c_fDebugger_##Cond) != 0)
#define CLR_EE_DBG_IS_NOT(Cond)                                                                                        \
    ((g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions & c_fDebugger_##Cond) == 0)
#define CLR_EE_DBG_SET(Cond)                                                                                           \
    g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions |= c_fDebugger_##Cond
#define CLR_EE_DBG_CLR(Cond)                                                                                           \
    g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions &= ~c_fDebugger_##Cond
#define CLR_EE_DBG_SET_MASK(Cond, Mask)                                                                                \
    CLR_EE_DBG_CLR(Mask);                                                                                              \
    CLR_EE_DBG_SET(Cond);
#define CLR_EE_DBG_RESTORE(Cond, f) ((f) ? CLR_EE_DBG_SET(Cond) : CLR_EE_DBG_CLR(Cond))
#define CLR_EE_DBG_IS_MASK(Cond, Mask)                                                                                 \
    ((g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions & c_fDebugger_##Mask) ==                 \
     c_fDebugger_##Cond)

#define CLR_EE_PRF_IS(Cond)                                                                                            \
    ((g_CLR_RT_ExecutionEngine.m_iProfiling_Conditions & c_fProfiling_##Cond) != 0)
#define CLR_EE_PRF_IS_NOT(Cond)                                                                                        \
    ((g_CLR_RT_ExecutionEngine.m_iProfiling_Conditions & c_fProfiling_##Cond) == 0)

#define CLR_EE_IS(Cond)                                                                                                \
    ((g_CLR_RT_ExecutionEngine.m_iExecution_Conditions & c_fExecution_##Cond) != 0)
#define CLR_EE_IS_NOT(Cond)                                                                                            \
    ((g_CLR_RT_ExecutionEngine.m_iExecution_Conditions & c_fExecution_##Cond) == 0)
#define CLR_EE_SET(Cond) g_CLR_RT_ExecutionEngine.m_iExecution_Conditions |= c_fExecution_##Cond
#define CLR_EE_CLR(Cond)                                                                                               \
    g_CLR_RT_ExecutionEngine.m_iExecution_Conditions &= ~c_fExecution_##Cond

#define CLR_EE_REBOOT_IS(Cond)                                                                                         \
    ((g_CLR_RT_ExecutionEngine.m_iReboot_Options & CLR_DBG_Commands::Monitor_Reboot::c_##Cond) ==                      \
     CLR_DBG_Commands::Monitor_Reboot::c_##Cond)
#define CLR_EE_REBOOT_CLR g_CLR_RT_ExecutionEngine.m_iReboot_Options = CLR_DBG_Commands::Monitor_Reboot::c_ClrOnly

#define CLR_EE_DBG_EVENT_SEND(cmd, size, payload, flags)                                                               \
    ((g_CLR_DBG_Debugger->m_messaging != NULL)                                                                         \
         ? g_CLR_DBG_Debugger->m_messaging->SendEvent(cmd, size, (unsigned char *)payload, flags)                      \
         : false)

#define CLR_EE_DBG_EVENT_BROADCAST(cmd, size, payload, flags) CLR_EE_DBG_EVENT_SEND(cmd, size, payload, flags)

  ////////////////////////////////////////////////////////////////////////////////////////////////

  //
  // Used to subtract system time (GC, compaction, other) from ExecutionConstraint checks.
  //
  struct ExecutionConstraintCompensation
    {
    int32_t m_recursion;
    int32_t m_start;
    int32_t m_cumulative;

    void Suspend()
      {
      if (m_recursion++ == 0)
        {
        m_start = HAL_Time_CurrentSysTicks();
        }
      }

    void Resume()
      {
      if (m_recursion)
        {
        if (--m_recursion == 0)
          {
          m_cumulative += (HAL_Time_CurrentSysTicks() - m_start);
          }
        }
      }

    int64_t Adjust(int64_t time) const
      {
      // FIXME: evaluate if the caller code can be adjusted to use SysTicks instead of 100ns ticks
      return time + ::HAL_Time_SysTicksToTime(m_cumulative);
      }
    };

  static ExecutionConstraintCompensation s_compensation;

  //--//

  int64_t m_maximumTimeToActive;

  //--//

  int64_t m_startTime;
  int64_t m_currentNextActivityTime;
  bool m_timerCache;
  uint64_t m_timerCacheNextTimeout;

  CLR_RT_DblLinkedList m_heap; // list of CLR_RT_HeapCluster
  CLR_RT_HeapCluster* m_lastHcUsed;
  int m_heapState;

  CLR_RT_DblLinkedList m_weakReferences; // OBJECT HEAP - DO RELOCATION - list of CLR_RT_HeapBlock_WeakReference

  CLR_RT_DblLinkedList m_timers; // EVENT HEAP - NO RELOCATION - list of CLR_RT_HeapBlock_Timer
  uint32_t m_raisedEvents;

  CLR_RT_DblLinkedList m_threadsReady;   // EVENT HEAP - NO RELOCATION - list of CLR_RT_Thread
  CLR_RT_DblLinkedList m_threadsWaiting; // EVENT HEAP - NO RELOCATION - list of CLR_RT_Thread
  CLR_RT_DblLinkedList m_threadsZombie;  // EVENT HEAP - NO RELOCATION - list of CLR_RT_Thread
  int m_lastPid;
  CLR_RT_Thread* m_currentThread;

  CLR_RT_DblLinkedList m_finalizersAlive;   // EVENT HEAP - NO RELOCATION - list of CLR_RT_HeapBlock_Finalizer
  CLR_RT_DblLinkedList m_finalizersPending; // EVENT HEAP - NO RELOCATION - list of CLR_RT_HeapBlock_Finalizer
  CLR_RT_Thread* m_finalizerThread;         // EVENT HEAP - NO RELOCATION -
  CLR_RT_Thread* m_cctorThread;             // EVENT HEAP - NO RELOCATION -

#if !defined(NANOCLR_APPDOMAINS)
  CLR_RT_HeapBlock* m_globalLock;           // OBJECT HEAP - DO RELOCATION -
  CLR_RT_HeapBlock* m_outOfMemoryException; // OBJECT HEAP - DO RELOCATION -
#endif

  CLR_RT_HeapBlock* m_currentUICulture; // OBJECT HEAP - DO RELOCATION -

  //--//

  CLR_RT_Thread *m_canFlyEventThread; // EVENT HEAP - NO RELOCATION
  CLR_RT_Thread *m_timerThread;     // EVENT HEAP - NO RELOCATION

  //--//

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  CLR_RT_HeapBlock_Array* m_scratchPadArray; // OBJECT HEAP - DO RELOCATION -
#endif

  bool m_fShuttingDown;

  //--//

  static HRESULT CreateInstance();

  HRESULT ExecutionEngine_Initialize();

  static HRESULT DeleteInstance();

  void ExecutionEngine_Cleanup();

  static void Reboot(bool fHard);

  void JoinAllThreadsAndExecuteFinalizer();

  void LoadDownloadedAssemblies();

  static void ExecutionConstraint_Suspend();
  static void ExecutionConstraint_Resume();

  uint32_t PerformGarbageCollection();
  void PerformHeapCompaction();

  void Relocate();

  HRESULT ScheduleThreads(int maxContextSwitch);

  uint32_t WaitForActivity(uint32_t powerLevel, uint32_t events, int64_t timeout_ms);
  uint32_t WaitForActivity();

  HRESULT Execute(wchar_t* entryPointArgs, int maxContextSwitch);

  HRESULT WaitForDebugger();

  static CLR_RT_HeapBlock* AccessStaticField(uint32_t fd);

  void ProcessTimeEvent(uint32_t event);

  static void InvalidateTimerCache();

  static int64_t GetUptime();

  CLR_RT_HeapBlock* ExtractHeapBlocksForArray(
    CLR_RT_TypeDef_Instance& inst,
    uint32_t length,
    const CLR_RT_ReflectionDef_Index& reflex);
  CLR_RT_HeapBlock* ExtractHeapBlocksForClassOrValueTypes(
    uint32_t dataType,
    uint32_t flags,
    uint32_t cls,
    uint32_t length);
  CLR_RT_HeapBlock* ExtractHeapBytesForObjects(uint32_t dataType, uint32_t flags, uint32_t length);
  CLR_RT_HeapBlock* ExtractHeapBlocksForObjects(uint32_t dataType, uint32_t flags, uint32_t length);
  CLR_RT_HeapBlock_Node* ExtractHeapBlocksForEvents(uint32_t dataType, uint32_t flags, uint32_t length);

  HRESULT NewThread(
    CLR_RT_Thread*& th,
    CLR_RT_HeapBlock_Delegate* pDelegate,
    int priority,
    int32_t id,
    uint32_t flags = 0);
  void PutInProperList(CLR_RT_Thread* th);
  int32_t GetNextThreadId();

  HRESULT InitializeReference(CLR_RT_HeapBlock& ref, CLR_RT_SignatureParser& parser);
  HRESULT InitializeReference(CLR_RT_HeapBlock& ref, const CLR_RECORD_FIELDDEF* target, CLR_RT_Assembly* assm);

  HRESULT InitializeLocals(CLR_RT_HeapBlock* locals, CLR_RT_Assembly* assm, const CLR_RECORD_METHODDEF* md);

  HRESULT NewObjectFromIndex(CLR_RT_HeapBlock& reference, uint32_t cls);
  HRESULT NewObject(CLR_RT_HeapBlock& reference, const CLR_RT_TypeDef_Instance& inst);
  HRESULT NewObject(CLR_RT_HeapBlock& reference, uint32_t token, CLR_RT_Assembly* assm);

  HRESULT CloneObject(CLR_RT_HeapBlock& reference, const CLR_RT_HeapBlock& source);
  HRESULT CopyValueType(CLR_RT_HeapBlock* destination, const CLR_RT_HeapBlock* source);

  HRESULT NewArrayList(CLR_RT_HeapBlock& ref, int size, CLR_RT_HeapBlock_Array*& array);

  HRESULT FindFieldDef(CLR_RT_TypeDef_Instance& inst, const char* szText, uint32_t& res);
  HRESULT FindFieldDef(CLR_RT_HeapBlock& reference, const char* szText, uint32_t& res);
  HRESULT FindField(CLR_RT_HeapBlock& reference, const char* szText, CLR_RT_HeapBlock*& field);
  HRESULT SetField(CLR_RT_HeapBlock& reference, const char* szText, CLR_RT_HeapBlock& value);
  HRESULT GetField(CLR_RT_HeapBlock& reference, const char* szText, CLR_RT_HeapBlock& value);

  HRESULT LockObject(CLR_RT_HeapBlock& reference, CLR_RT_SubThread* sth, const int64_t& timeExpire, bool fForce);
  HRESULT UnlockObject(CLR_RT_HeapBlock& reference, CLR_RT_SubThread* sth);
  void DeleteLockRequests(CLR_RT_Thread* thTarget, CLR_RT_SubThread* sthTarget);

  HRESULT Sleep(CLR_RT_Thread* caller, const int64_t& timeExpire);

  HRESULT WaitEvents(CLR_RT_Thread* caller, const int64_t& timeExpire, uint32_t events, bool& fSuccess);
  void SignalEvents(CLR_RT_DblLinkedList& threads, uint32_t events);
  void SignalEvents(uint32_t events);

  HRESULT InitTimeout(int64_t& timeExpire, const int64_t& timeout);
  HRESULT InitTimeout(int64_t& timeExpire, int32_t timeout);

  static bool IsInstanceOf(CLR_RT_TypeDescriptor& desc, CLR_RT_TypeDescriptor& descTarget, bool isInstInstruction);
  static bool IsInstanceOf(uint32_t cls, uint32_t clsTarget);
  static bool IsInstanceOf(CLR_RT_HeapBlock& obj, uint32_t clsTarget);
  static bool IsInstanceOf(CLR_RT_HeapBlock& obj, CLR_RT_Assembly* assm, uint32_t token, bool isInstInstruction);

  static HRESULT CastToType(CLR_RT_HeapBlock& ref, uint32_t tk, CLR_RT_Assembly* assm, bool isInstInstruction);


  void SetDebuggingInfoBreakpoints(bool fSet);
  void InstallBreakpoints(const execution_breakpoint_t *data, size_t num);
  void StopOnBreakpoint(const execution_breakpoint_t *def, CLR_RT_Thread* th);
  void StopOnBreakpoint(
    const execution_breakpoint_t *def,
    CLR_RT_StackFrame* stack,
    CLR_PMETADATA ip);
  void Breakpoint_System_Event(
    const execution_breakpoint_t *def,
    uint16_t event,
    CLR_RT_Thread* th,
    CLR_RT_StackFrame* stack,
    CLR_PMETADATA ip);
  bool DequeueActiveBreakpoint(const execution_breakpoint_t *def);

  void Breakpoint_Assemblies_Loaded();

  void Breakpoint_Threads_Prepare(CLR_RT_DblLinkedList& threads);
  void Breakpoint_Thread_Terminated(CLR_RT_Thread* th);
  void Breakpoint_Thread_Created(CLR_RT_Thread* th);

  void Breakpoint_StackFrame_Push(CLR_RT_StackFrame* stack, uint32_t reason);
  void Breakpoint_StackFrame_Pop(CLR_RT_StackFrame* stack, bool stepEH);
  void Breakpoint_StackFrame_Step(CLR_RT_StackFrame* stack, CLR_PMETADATA ip);
  void Breakpoint_StackFrame_Hard(CLR_RT_StackFrame* stack, CLR_PMETADATA ip);

  void Breakpoint_Exception(CLR_RT_StackFrame* stack, uint32_t reason, CLR_PMETADATA ip);
  void Breakpoint_Exception_Intercepted(CLR_RT_StackFrame* stack);
  void Breakpoint_Exception_Uncaught(CLR_RT_Thread* th);

  //--//

  bool IsTimeExpired(const int64_t& timeExpire, int64_t& timeoutMin);

  bool IsThereEnoughIdleTime(uint32_t expectedMsec);

  void SpawnTimer();
  void SpawnFinalizer();
  void SpawnStaticConstructor(CLR_RT_Thread*& pCctorThread);
#if defined(NANOCLR_APPDOMAINS)
  bool SpawnStaticConstructorHelper(
    CLR_RT_AppDomain* appDomain,
    CLR_RT_AppDomainAssembly* appDomainAssembly,
    uint32_t idx);
#else
  bool SpawnStaticConstructorHelper(CLR_RT_Assembly* assembly, uint32_t idx);
#endif
  static void FinalizerTerminationCallback(void* arg);
  static void StaticConstructorTerminationCallback(void* arg);
  bool EnsureSystemThread(CLR_RT_Thread*& thread, int priority);

  int64_t ProcessTimer();

  //--//

  PROHIBIT_COPY_CONSTRUCTORS(CLR_RT_ExecutionEngine);

  //--//

  private:
    HRESULT AllocateHeaps();

    void DeleteLockRequests(CLR_RT_Thread* thTarget, CLR_RT_SubThread* sthTarget, CLR_RT_DblLinkedList& threads);

    CLR_RT_HeapBlock* ExtractHeapBlocks(
      CLR_RT_DblLinkedList& heap,
      uint32_t dataType,
      uint32_t flags,
      uint32_t length);

    CLR_RT_HeapBlock_Lock* FindLockObject(CLR_RT_DblLinkedList& threads, CLR_RT_HeapBlock& object);
    CLR_RT_HeapBlock_Lock* FindLockObject(CLR_RT_HeapBlock& object);

    void CheckTimers(int64_t& timeoutMin);
    void CheckThreads(int64_t& timeoutMin, CLR_RT_DblLinkedList& threads);

    void ReleaseAllThreads(CLR_RT_DblLinkedList& threads);
    void AbortAllThreads(CLR_RT_DblLinkedList& threads);

    void InsertThreadRoundRobin(CLR_RT_DblLinkedList& threads, CLR_RT_Thread* th);

    uint32_t WaitSystemEvents(uint32_t powerLevel, uint32_t events, int64_t timeExpire);

#if defined(_WIN32)
    HRESULT CreateEntryPointArgs(CLR_RT_HeapBlock& args, wchar_t* szCommandLineArgs);
#endif

    // The lowest value for execution counter in threads.
    int m_GlobalExecutionCounter;

    // Increase the value of m_executionCounter for all threads by iUpdateValue.
    // This ensures that counter does not underflow.
    void AdjustExecutionCounter(CLR_RT_DblLinkedList& threadList, int iUpdateValue);

  public:
    // This function updates execution counter in pThread and makes it last to execute.
    // It is used to Thread.Sleep(0) imlementation. The thread is still ready, but is last to execute.
    void UpdateToLowestExecutionCounter(CLR_RT_Thread* pThread) const;

    void RetrieveCurrentMethod(uint32_t& assmIdx, uint32_t& methodIdx);
  };

extern CLR_RT_ExecutionEngine g_CLR_RT_ExecutionEngine;
extern uint32_t g_buildCRC;

//--//

//
// CT_ASSERT macro generates a compiler error in case the size of any structure changes.
//
CT_ASSERT(sizeof(CLR_RT_HeapBlock) == 12)
CT_ASSERT(sizeof(CLR_RT_HeapBlock_Raw) == sizeof(CLR_RT_HeapBlock))

#if defined(NANOCLR_TRACE_MEMORY_STATS)
#define NANOCLR_TRACE_MEMORY_STATS_EXTRA_SIZE 4
#else
#define NANOCLR_TRACE_MEMORY_STATS_EXTRA_SIZE 0
#endif

#if defined(__GNUC__) // Gcc compiler uses 8 bytes for a function pointer
CT_ASSERT(sizeof(CLR_RT_DataTypeLookup) == 20 + NANOCLR_TRACE_MEMORY_STATS_EXTRA_SIZE)

#elif defined(PLATFORM_WINDOWS_EMULATOR) || defined(NANOCLR_TRACE_MEMORY_STATS)
CT_ASSERT(sizeof(CLR_RT_DataTypeLookup) == 16 + 4)

#else

!ERROR

#endif

//--//

#if defined(_MSC_VER)
#pragma pack(pop, NANOCLR_RUNTIME_H)
#endif

#endif // _NANOCLR_RUNTIME_H_
