﻿//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

namespace CanFly.Tools.MetadataProcessor
{
    /// <summary>
    /// This list contains type codes used for serializing attributes signatures
    /// </summary>
    internal enum nanoSerializationType : byte
    {
        // these where defined @ typedef enum CorElementType
        ELEMENT_TYPE_END = 0x0,
        ELEMENT_TYPE_VOID = 0x1,
        ELEMENT_TYPE_BOOLEAN = 0x2,
        ELEMENT_TYPE_CHAR = 0x3,
        ELEMENT_TYPE_I1 = 0x4,
        ELEMENT_TYPE_U1 = 0x5,
        ELEMENT_TYPE_I2 = 0x6,
        ELEMENT_TYPE_U2 = 0x7,
        ELEMENT_TYPE_I4 = 0x8,
        ELEMENT_TYPE_U4 = 0x9,
        ELEMENT_TYPE_I8 = 0xa,
        ELEMENT_TYPE_U8 = 0xb,
        ELEMENT_TYPE_R4 = 0xc,
        ELEMENT_TYPE_R8 = 0xd,
        ELEMENT_TYPE_STRING = 0xe,

        // every type above PTR will be simple type
        ELEMENT_TYPE_PTR = 0x0F,      // PTR <type>
        ELEMENT_TYPE_BYREF = 0x10,     // BYREF <type>

        // Please use ELEMENT_TYPE_VALUETYPE. ELEMENT_TYPE_VALUECLASS is deprecated.
        ELEMENT_TYPE_VALUETYPE = 0x11,     // VALUETYPE <class Token>
        ELEMENT_TYPE_CLASS = 0x12,     // CLASS <class Token>
        ELEMENT_TYPE_VAR = 0x13,     // a class type variable VAR <U1>
        ELEMENT_TYPE_ARRAY = 0x14,     // MDARRAY <type> <rank> <bcount> <bound1> ... <lbcount> <lb1> ...
        ELEMENT_TYPE_GENERICINST = 0x15,     // instantiated type
        ELEMENT_TYPE_TYPEDBYREF = 0x16,     // This is a simple type.

        ELEMENT_TYPE_I = 0x18,     // native integer size
        ELEMENT_TYPE_U = 0x19,     // native unsigned integer size
        ELEMENT_TYPE_FNPTR = 0x1B,     // FNPTR <complete sig for the function including calling convention>
        ELEMENT_TYPE_OBJECT = 0x1C,     // Shortcut for System.Object
        ELEMENT_TYPE_SZARRAY = 0x1D,     // Shortcut for single dimension zero lower bound array
                                         // SZARRAY <type>
        ELEMENT_TYPE_MVAR = 0x1E,     // a method type variable MVAR <U1>

        // This is only for binding
        ELEMENT_TYPE_CMOD_REQD = 0x1F,     // required C modifier : E_T_CMOD_REQD <mdTypeRef/mdTypeDef>
        ELEMENT_TYPE_CMOD_OPT = 0x20,     // optional C modifier : E_T_CMOD_OPT <mdTypeRef/mdTypeDef>

        // This is for signatures generated internally (which will not be persisted in any way).
        ELEMENT_TYPE_INTERNAL = 0x21,     // INTERNAL <typehandle>

        // Note that this is the max of base type excluding modifiers
        ELEMENT_TYPE_MAX = 0x22,     // first invalid element type


        ELEMENT_TYPE_MODIFIER = 0x40,
        ELEMENT_TYPE_SENTINEL = 0x01 | ELEMENT_TYPE_MODIFIER, // sentinel for varargs
        ELEMENT_TYPE_PINNED = 0x05 | ELEMENT_TYPE_MODIFIER,
        ELEMENT_TYPE_R4_HFA = 0x06 | ELEMENT_TYPE_MODIFIER, // used only internally for R4 HFA types
        ELEMENT_TYPE_R8_HFA = 0x07 | ELEMENT_TYPE_MODIFIER, // used only internally for R8 HFA types

        SERIALIZATION_TYPE_FIELD = 0x53,
        SERIALIZATION_TYPE_PROPERTY = 0x54,
    }
}
