//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

namespace CanFly.Tools.MetadataProcessor
{
    public enum nanoCLR_DataType : byte
    {
        // these where defined @ enum CLR_DataType

        DATATYPE_VOID, // 0 bytes

        DATATYPE_BOOLEAN, // 1 byte
        DATATYPE_I1, // 1 byte
        DATATYPE_U1, // 1 byte

        DATATYPE_CHAR, // 2 bytes
        DATATYPE_I2, // 2 bytes
        DATATYPE_U2, // 2 bytes

        DATATYPE_I4, // 4 bytes
        DATATYPE_U4, // 4 bytes
        DATATYPE_R4, // 4 bytes

        DATATYPE_I8, // 8 bytes
        DATATYPE_U8, // 8 bytes
        DATATYPE_R8, // 8 bytes
        DATATYPE_DATETIME, // 8 bytes     // Shortcut for System.DateTime
        DATATYPE_TIMESPAN, // 8 bytes     // Shortcut for System.TimeSpan
        DATATYPE_STRING,

        // This is the last type that doesn't need to be relocated
        DATATYPE_LAST_NONPOINTER = DATATYPE_TIMESPAN,
        // All the above types don't need fix-up on assignment
        DATATYPE_LAST_PRIMITIVE_TO_PRESERVE = DATATYPE_R8,

        // All the above types can be marshaled by assignment.
#if NANOCLR_NO_ASSEMBLY_STRINGS
        DATATYPE_LAST_PRIMITIVE_TO_MARSHAL = DATATYPE_STRING,
#else
        DATATYPE_LAST_PRIMITIVE_TO_MARSHAL = DATATYPE_TIMESPAN,
#endif

        // All the above types don't need fix-up on assignment.
        DATATYPE_LAST_PRIMITIVE = DATATYPE_STRING,

        DATATYPE_OBJECT, // Shortcut for System.Object
        DATATYPE_CLASS, // CLASS <class Token>
        DATATYPE_VALUETYPE, // VALUETYPE <class Token>
        DATATYPE_SZARRAY, // Shortcut for single dimension zero lower bound array SZARRAY <type>
        DATATYPE_BYREF, // BYREF <type>

        /// <summary>
        /// Generic parameter in a generic type definition, represented as number
        /// </summary>
        DATATYPE_VAR,

        /// <summary>
        /// Generic type instantiation
        /// </summary>
        DATATYPE_GENERICINST,

        /// <summary>
        /// Generic parameter in a generic method definition, represented as number
        /// </summary>
        DATATYPE_MVAR,

        ////////////////////////////////////////

        DATATYPE_FREEBLOCK,
        DATATYPE_CACHEDBLOCK,
        DATATYPE_ASSEMBLY,
        DATATYPE_WEAKCLASS,
        DATATYPE_REFLECTION,
        DATATYPE_ARRAY_BYREF,
        DATATYPE_DELEGATE_HEAD,
        DATATYPE_DELEGATELIST_HEAD,
        DATATYPE_OBJECT_TO_EVENT,
        DATATYPE_BINARY_BLOB_HEAD,

        DATATYPE_THREAD,
        DATATYPE_SUBTHREAD,
        DATATYPE_STACK_FRAME,
        DATATYPE_TIMER_HEAD,
        DATATYPE_LOCK_HEAD,
        DATATYPE_LOCK_OWNER_HEAD,
        DATATYPE_LOCK_REQUEST_HEAD,
        DATATYPE_WAIT_FOR_OBJECT_HEAD,
        DATATYPE_FINALIZER_HEAD,
        DATATYPE_MEMORY_STREAM_HEAD, // SubDataType?
        DATATYPE_MEMORY_STREAM_DATA, // SubDataType?

        DATATYPE_SERIALIZER_HEAD, // SubDataType?
        DATATYPE_SERIALIZER_DUPLICATE, // SubDataType?
        DATATYPE_SERIALIZER_STATE, // SubDataType?

        DATATYPE_ENDPOINT_HEAD,

        //These constants are shared by Debugger.dll, and cannot be conditionally compiled away.
        //This adds a couple extra bytes to the lookup table.  But frankly, the lookup table should probably 
        //be shrunk to begin with.  Most of the datatypes are used just to tag memory.
        //For those datatypes, perhaps we should use a subDataType instead (probably what the comments above are about).

        DATATYPE_RADIO_LAST = DATATYPE_ENDPOINT_HEAD + 3,

        DATATYPE_IO_PORT,
        DATATYPE_IO_PORT_LAST = DATATYPE_RADIO_LAST + 1,

        DATATYPE_VTU_PORT_LAST = DATATYPE_IO_PORT_LAST + 1,

#if NANOCLR_APPDOMAINS
        DATATYPE_APPDOMAIN_HEAD             ,
        DATATYPE_TRANSPARENT_PROXY          ,
        DATATYPE_APPDOMAIN_ASSEMBLY         ,
#endif
        DATATYPE_APPDOMAIN_LAST = DATATYPE_VTU_PORT_LAST + 3,

        DATATYPE_FIRST_INVALID,

        // Type modifies. This is exact copy of VALUES ELEMENT_TYPE_* from CorHdr.h
        // 

        DATATYPE_TYPE_MODIFIER = 0x40,
        DATATYPE_TYPE_SENTINEL = 0x01 | DATATYPE_TYPE_MODIFIER, // sentinel for varargs
        DATATYPE_TYPE_PINNED = 0x05 | DATATYPE_TYPE_MODIFIER,
        DATATYPE_TYPE_R4_HFA = 0x06 | DATATYPE_TYPE_MODIFIER, // used only internally for R4 HFA types
        DATATYPE_TYPE_R8_HFA = 0x07 | DATATYPE_TYPE_MODIFIER, // used only internally for R8 HFA types

    }
}
