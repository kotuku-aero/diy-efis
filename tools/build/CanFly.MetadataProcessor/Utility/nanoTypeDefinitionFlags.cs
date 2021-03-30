//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;

namespace CanFly.Tools.MetadataProcessor
{
    /// <summary>
    /// This list contains the type definition flags
    /// </summary>
    [Flags]
    internal enum nanoTypeDefinitionFlags : ushort
    {
        // these where defined @ struct CLR_RECORD_TYPEDEF
        TD_Scope_None = 0x0000,

        // Class is public scope.
        TD_Scope_NotPublic =            0x0000,

        // Class is public scope.
        TD_Scope_Public =               0x0001,
        // Class is nested with public visibility.
        TD_Scope_NestedPublic =         0x0002,
        // Class is nested with private visibility.
        TD_Scope_NestedPrivate =        0x0003,
        // Class is nested with family visibility.
        TD_Scope_NestedFamily =         0x0004,
        // Class is nested with assembly visibility.
        TD_Scope_NestedAssembly =       0x0005,
        // Class is nested with family and assembly visibility.
        TD_Scope_NestedFamANDAssem =    0x0006,
        // Class is nested with family or assembly visibility.
        TD_Scope_NestedFamORAssem =     0x0007,

        /// <summary>
        /// Mask for scope flags
        /// </summary>
        TD_Scope =
            TD_Scope_NotPublic          |
            TD_Scope_Public             | 
            TD_Scope_NestedPublic       | 
            TD_Scope_NestedPrivate      |
            TD_Scope_NestedFamily       |
            TD_Scope_NestedAssembly     |
            TD_Scope_NestedFamANDAssem  |
            TD_Scope_NestedFamORAssem,

        TD_Serializable =               0x0008,

        TD_Semantics_Class =            0x0000,
        TD_Semantics_ValueType =        0x0010,
        TD_Semantics_Interface =        0x0020,
        TD_Semantics_Enum =             0x0030,

        /// <summary>
        /// Mask for semantics flags
        /// </summary>
        TD_Semantics =
            TD_Semantics_Class      |
            TD_Semantics_ValueType  | 
            TD_Semantics_Interface  | 
            TD_Semantics_Enum,

        TD_Abstract =                   0x0040,
        TD_Sealed =                     0x0080,

        TD_SpecialName =                0x0100,
        TD_Delegate =                   0x0200,
        TD_MulticastDelegate =          0x0400,

        TD_Patched =                    0x0800,

        TD_BeforeFieldInit =            0x1000,
        TD_HasSecurity =                0x2000,
        TD_HasFinalizer =               0x4000,
        TD_HasAttributes =              0x8000,
    }
}
