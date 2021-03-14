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
    /// Encapsulates logic for storing type specifications list and writing this
    /// list into target assembly in .NET nanoFramework format.
    /// </summary>
    public sealed class nanoTypeSpecificationsTable : InanoTable
    {
        /// <summary>
        /// Helper class for comparing two instances of <see cref="TypeReference"/> objects
        /// using <see cref="TypeReference.FullName"/> property as unique key for comparison.
        /// </summary>
        private sealed class TypeReferenceComparer : IEqualityComparer<TypeReference>
        {
            /// <inheritdoc/>
            public bool Equals(TypeReference lhs, TypeReference rhs)
            {
                return string.Equals(lhs.FullName, rhs.FullName, StringComparison.Ordinal);
            }

            /// <inheritdoc/>
            public int GetHashCode(TypeReference that)
            {
                return that.FullName.GetHashCode();
            }
        }

        /// <summary>
        /// Maps for each unique type specification and related identifier.
        /// </summary>
        private readonly IDictionary<TypeReference, ushort> _idByTypeSpecifications =
            new Dictionary<TypeReference, ushort>(new TypeReferenceComparer());

        /// <summary>
        /// Assembly tables context - contains all tables used for building target assembly.
        /// </summary>
        private readonly nanoTablesContext _context;

        /// <summary>
        /// Last available type specifier identificator.
        /// </summary>
        private ushort _lastAvailableId;

        /// <summary>
        /// Creates new instance of <see cref="nanoTypeSpecificationsTable"/> object.
        /// </summary>
        /// <param name="context">
        /// Assembly tables context - contains all tables used for building target assembly.
        /// </param>
        public nanoTypeSpecificationsTable(
            nanoTablesContext context)
        {
            _context = context;
        }

        /// <summary>
        /// Gets existing or creates new type specification reference identifier.
        /// </summary>
        /// <param name="typeReference">Type reference value for obtaining identifier.</param>
        /// <returns>Existing identifier if specification already in table or new one.</returns>
        public ushort GetOrCreateTypeSpecificationId(
            TypeReference typeReference)
        {
            ushort referenceId;
            if (!_idByTypeSpecifications.TryGetValue(typeReference, out referenceId))
            {
                // check for array in TypeSpec because we don't support for multidimensional arrays
                if (typeReference.IsArray &&
                    (typeReference as ArrayType).Rank > 1)
                {
                    throw new ArgumentException($".NET nanoFramework doesn't have support for multidimensional arrays. Unable to parse {typeReference.FullName}.");
                }

                _idByTypeSpecifications.Add(typeReference, _lastAvailableId);

                referenceId = _lastAvailableId;
                ++_lastAvailableId;
            }

            return referenceId;
        }

        /// <summary>
        /// Gets type specification identifier (if it already added into type specifications list).
        /// </summary>
        /// <param name="typeReference">Type reference in Mono.Cecil format.</param>
        /// <param name="referenceId">Type reference identifier for filling.</param>
        /// <returns>Returns <c>true</c> if item found, otherwise returns <c>false</c>.</returns>
        public bool TryGetTypeReferenceId(
            TypeReference typeReference,
            out ushort referenceId)
        {
            return _idByTypeSpecifications.TryGetValue(typeReference, out referenceId);
        }

        public TypeReference TryGetTypeSpecification(MetadataToken token)
        {
            foreach (var t in _idByTypeSpecifications)
            {
                if(t.Key.MetadataToken == token)
                {
                    return t.Key;
                }
            }

            return null;
        }

        /// <inheritdoc/>
        public void Write(
            nanoBinaryWriter writer)
        {
            foreach (var item in _idByTypeSpecifications
                .OrderBy(item => item.Value)
                .Select(item => item.Key))
            {
                writer.WriteUInt16(_context.SignaturesTable.GetOrCreateSignatureId(item));
                writer.WriteUInt16(0x0000); // padding
            }
        }
    }
}
