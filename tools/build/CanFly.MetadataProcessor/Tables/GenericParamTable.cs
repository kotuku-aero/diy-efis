//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System;
using System.Collections.Generic;

namespace CanFly.Tools.MetadataProcessor
{
    /// <summary>
    /// Encapsulates logic for storing generic parameters list and writing
    /// this collected list into target assembly in .NET nanoFramework format.
    /// </summary>
    public sealed class GenericParamTable :
        ReferenceTableBase<GenericParameter>
    {
        /// <summary>
        /// Helper class for comparing two instances of <see cref="GenericParameter"/> objects
        /// using <see cref="MetadataToken"/> property as unique key for comparison.
        /// </summary>
        private sealed class MemberReferenceComparer : IEqualityComparer<GenericParameter>
        {
            /// <inheritdoc/>
            public bool Equals(GenericParameter x, GenericParameter y)
            {
                if (x is null)
                {
                    throw new ArgumentNullException(nameof(x));
                }

                if (y is null)
                {
                    throw new ArgumentNullException(nameof(y));
                }

                return x.MetadataToken.ToInt32() == y.MetadataToken.ToInt32();
            }

            /// <inheritdoc/>
            public int GetHashCode(GenericParameter obj)
            {
                return obj.MetadataToken.ToInt32().GetHashCode();
            }
        }

        /// <summary>
        /// Creates new instance of <see cref="GenericParamTable"/> object.
        /// </summary>
        /// <param name="items">List of member references in Mono.Cecil format.</param>
        /// <param name="context">
        /// Assembly tables context - contains all tables used for building target assembly.
        /// </param>
        public GenericParamTable(
            IEnumerable<GenericParameter> items,
            TablesContext context)
            : base(items, new MemberReferenceComparer(), context)
        {
        }

        /// <summary>
        /// Gets method reference ID if possible (if method is external and stored in this table).
        /// </summary>
        /// <param name="genericParameter">Method reference metadata in Mono.Cecil format.</param>
        /// <param name="referenceId">Method reference ID in .NET nanoFramework format.</param>
        /// <returns>Returns <c>true</c> if reference found, otherwise returns <c>false</c>.</returns>
        public bool TryGetParameterId(
            GenericParameter genericParameter,
            out ushort referenceId)
        {
            return TryGetIdByValue(genericParameter, out referenceId);
        }

        /// <inheritdoc/>
        protected override void WriteSingleItem(
            CLRBinaryWriter writer,
            GenericParameter item)
        {
            if (!_context.MinimizeComplete)
            {
                return;
            }

            // TODO
        }
    }
}
