//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System;
using System.Collections.Generic;
using System.Linq;

namespace CanFly.Tools.MetadataProcessor
{
    /// <summary>
    /// Encapsulates logic for storing fields definitions list and writing
    /// this collected list into target assembly in .NET nanoFramework format.
    /// </summary>
    public sealed class FieldDefinitionTable :
        ReferenceTableBase<FieldDefinition>
    {
        /// <summary>
        /// Helper class for comparing two instances of <see cref="FieldDefinition"/> objects
        /// using <see cref="FieldDefinition.FullName"/> property as unique key for comparison.
        /// </summary>
        private sealed class FieldDefinitionComparer : IEqualityComparer<FieldDefinition>
        {
            /// <inheritdoc/>
            public bool Equals(FieldDefinition lhs, FieldDefinition rhs)
            {
                return string.Equals(lhs.FullName, rhs.FullName, StringComparison.Ordinal);
            }

            /// <inheritdoc/>
            public int GetHashCode(FieldDefinition that)
            {
                return that.FullName.GetHashCode();
            }
        }

        /// <summary>
        /// Maximal returned field reference id - used for emulating old MetadataProcessor behavior.
        /// </summary>
        private int _maxReferenceId;

        /// <summary>
        /// Creates new instance of <see cref="FieldDefinitionTable"/> object.
        /// </summary>
        /// <param name="items">List of field definitions in Mono.Cecil format.</param>
        /// <param name="context">
        /// Assembly tables context - contains all tables used for building target assembly.
        /// </param>
        public FieldDefinitionTable(
            IEnumerable<FieldDefinition> items,
            TablesContext context)
            : base(items, new FieldDefinitionComparer(), context)
        {
        }

        /// <summary>
        /// Gets maximal returned field reference id - used for emulating old MetadataProcessor behavior.
        /// </summary>
        public ushort MaxFieldId { get { return (ushort)_maxReferenceId; } }

        /// <inheritdoc/>
        protected override void WriteSingleItem(
            CLRBinaryWriter writer,
            FieldDefinition item)
        {
            if (!_context.MinimizeComplete)
            {
                return;
            }

            WriteStringReference(writer, item.Name);
            writer.WriteUInt16(_context.SignaturesTable.GetOrCreateSignatureId(item));

            writer.WriteUInt16(_context.SignaturesTable.GetOrCreateSignatureId(item.InitialValue));
            writer.WriteUInt16(GetFlags(item));
        }

        /// <summary>
        /// Gets field reference identifier (if field is defined inside target assembly).
        /// </summary>
        /// <param name="field">Field definition in Mono.Cecil format.</param>
        /// <param name="trackMaxReferenceId">If set to <c>true</c> we should track max ID value.</param>
        /// <param name="referenceId">Field reference identifier for filling.</param>
        /// <returns>Returns <c>true</c> if item found, otherwise returns <c>false</c>.</returns>
        public bool TryGetFieldReferenceId(
            FieldDefinition field,
            bool trackMaxReferenceId,
            out ushort referenceId)
        {
            var found = TryGetIdByValue(field, out referenceId);
            if (trackMaxReferenceId && found)
            {
                _maxReferenceId = Math.Max(_maxReferenceId, referenceId + 1);
            }
            return found;
        }

        private ushort GetFlags(
            FieldDefinition field)
        {
            const ushort FD_Scope_Private = 0x0001; // Accessible only by the parent type.
            const ushort FD_Scope_FamANDAssem = 0x0002; // Accessible by sub-types only in this Assembly.
            const ushort FD_Scope_Assembly = 0x0003; // Accessibly by anyone in the Assembly.
            const ushort FD_Scope_Family = 0x0004; // Accessible only by type and sub-types.
            const ushort FD_Scope_FamORAssem = 0x0005; // Accessibly by sub-types anywhere, plus anyone in assembly.
            const ushort FD_Scope_Public = 0x0006; // Accessibly by anyone who has visibility to this scope.

            const ushort FD_NotSerialized = 0x0008; // Field does not have to be serialized when type is remoted.

            const ushort FD_Static = 0x0010; // Defined on type, else per instance.
            const ushort FD_InitOnly = 0x0020; // Field may only be initialized, not written to after init.
            const ushort FD_Literal = 0x0040; // Value is compile time constant.

            const ushort FD_SpecialName = 0x0100; // field is special.  Name describes how.
            const ushort FD_HasDefault = 0x0200; // Field has default.
            const ushort FD_HasFieldRVA = 0x0400; // Field has RVA.

            const ushort FD_NoReflection = 0x0800; // field does not allow reflection

            const ushort FD_HasAttributes = 0x8000;

            ushort flag = 0;

            if (field.IsPrivate)
            {
                flag = FD_Scope_Private;
            }
            else if (field.IsFamilyAndAssembly)
            {
                flag = FD_Scope_FamANDAssem;
            }
            else if (field.IsFamilyOrAssembly)
            {
                flag = FD_Scope_FamORAssem;
            }
            else if (field.IsAssembly)
            {
                flag = FD_Scope_Assembly;
            }
            else if (field.IsFamily)
            {
                flag = FD_Scope_Family;
            }
            else if (field.IsPublic)
            {
                flag = FD_Scope_Public;
            }

            if (field.IsNotSerialized)
            {
                flag |= FD_NotSerialized;
            }

            if (field.IsStatic)
            {
                flag |= FD_Static;
            }

            if (field.IsInitOnly)
            {
                flag |= FD_InitOnly;
            }

            if (field.IsLiteral)
            {
                flag |= FD_Literal;
            }

            if (field.IsSpecialName)
            {
                flag |= FD_SpecialName;
            }

            if (field.HasDefault)
            {
                flag |= FD_HasDefault;
            }

            if (field.HasCustomAttributes)
            {
                flag |= FD_HasAttributes;
            }

            if (field.RVA != 0)
            {
                flag |= FD_HasFieldRVA;
            }

            return flag;
        }

        public override void RemoveUnusedItems(HashSet<MetadataToken> set)
        {
            base.RemoveUnusedItems(set);

            // adjust ID
            _maxReferenceId = _items.Count();
        }
    }
}
