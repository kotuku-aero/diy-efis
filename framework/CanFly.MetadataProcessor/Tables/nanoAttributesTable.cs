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
    /// Encapsulates logic for storing attributes for types/methods/fields list and writing
    /// this collected list into target assembly in .NET nanoFramework format.
    /// </summary>
    public sealed class nanoAttributesTable : InanoTable
    {
        /// <summary>
        /// List of custom attributes in Mono.Cecil format for all internal types.
        /// </summary>
        private IEnumerable<Tuple<CustomAttribute, ushort>> _typesAttributes;

        /// <summary>
        /// List of custom attributes in Mono.Cecil format for all internal fields.
        /// </summary>
        /// 
        private IEnumerable<Tuple<CustomAttribute, ushort>> _fieldsAttributes;

        /// <summary>
        /// List of custom attributes in Mono.Cecil format for all internal methods.
        /// </summary>
        private IEnumerable<Tuple<CustomAttribute, ushort>> _methodsAttributes;

        /// <summary>
        /// Assembly tables context - contains all tables used for building target assembly.
        /// </summary>
        private readonly nanoTablesContext _context;

        /// <summary>
        /// Creates new instance of <see cref="nanoAttributesTable"/> object.
        /// </summary>
        /// <param name="typesAttributes">
        /// List of custom attributes in Mono.Cecil format for all internal types.
        /// </param>
        /// <param name="fieldsAttributes">
        /// List of custom attributes in Mono.Cecil format for all internal fields.
        /// </param>
        /// <param name="methodsAttributes">
        /// List of custom attributes in Mono.Cecil format for all internal methods.
        /// </param>
        /// <param name="context">
        /// Assembly tables context - contains all tables used for building target assembly.
        /// </param>
        public nanoAttributesTable(
            IEnumerable<Tuple<CustomAttribute, ushort>> typesAttributes,
            IEnumerable<Tuple<CustomAttribute, ushort>> fieldsAttributes,
            IEnumerable<Tuple<CustomAttribute, ushort>> methodsAttributes,
            nanoTablesContext context)
        {
            _typesAttributes = typesAttributes.ToList();
            _fieldsAttributes = fieldsAttributes.ToList();
            _methodsAttributes = methodsAttributes.ToList();

            _context = context;
        }

        /// <inheritdoc/>
        public void Write(
            nanoBinaryWriter writer)
        {
            WriteAttributes(writer, 0x0004, _typesAttributes);
            WriteAttributes(writer, 0x0005, _fieldsAttributes);
            WriteAttributes(writer, 0x0006, _methodsAttributes);
        }

        private void WriteAttributes(
            nanoBinaryWriter writer,
            ushort tableNumber,
            IEnumerable<Tuple<CustomAttribute, ushort>> attributes)
        {
            foreach (var item in attributes)
            {
                var attribute = item.Item1;
                var targetIdentifier = item.Item2;

                writer.WriteUInt16(tableNumber);
                writer.WriteUInt16(targetIdentifier);

                writer.WriteUInt16(_context.GetMethodReferenceId(attribute.Constructor));
                writer.WriteUInt16(_context.SignaturesTable.GetOrCreateSignatureId(attribute));
            }
        }

        /// <summary>
        /// Remove unused items from attribute tables.
        /// </summary>
        public void RemoveUnusedItems(HashSet<MetadataToken> set)
        {
            // build a collection of the current items that are present in the used items set
            List<Tuple<CustomAttribute, ushort>> usedItems = new List<Tuple<CustomAttribute, ushort>>();

            // types attributes
            foreach (var item in _typesAttributes
                                    .Where(item => set.Contains(((IMetadataTokenProvider)item.Item1.Constructor).MetadataToken)))
            {
                usedItems.Add(item);
            }

            // re-create the items dictionary with the used items only
            _typesAttributes = usedItems
            .Select(a => new Tuple<CustomAttribute, ushort>(a.Item1, a.Item2));

            // fields attributes
            usedItems = new List<Tuple<CustomAttribute, ushort>>();

            foreach (var item in _fieldsAttributes
                                    .Where(item => set.Contains(((IMetadataTokenProvider)item.Item1.Constructor).MetadataToken)))
            {
                usedItems.Add(item);
            }

            // re-create the items dictionary with the used items only
            _fieldsAttributes = usedItems
            .Select(a => new Tuple<CustomAttribute, ushort>(a.Item1, a.Item2));

            // methods attributes
            usedItems = new List<Tuple<CustomAttribute, ushort>>();

            foreach (var item in _methodsAttributes
                                    .Where(item => set.Contains(((IMetadataTokenProvider)item.Item1.Constructor).MetadataToken)))
            {
                usedItems.Add(item);
            }

            // re-create the items dictionary with the used items only
            _methodsAttributes = usedItems
            .Select(a => new Tuple<CustomAttribute, ushort>(a.Item1, a.Item2));
        }
    }
}