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
    /// Base class for all metadata tables with lookup functionality. Stores items in Mono.Cecil
    /// format in dictionary and allows to get item ID by item value (using custom comparer).
    /// </summary>
    /// <typeparam name="T">Type of stored metadata item (in Mono.Cecil format).</typeparam>
    public abstract class nanoReferenceTableBase<T> : InanoTable
    {
        /// <summary>
        /// Lookup table for finding item ID by item value.
        /// </summary>
        private Dictionary<T, ushort> _idsByItemsDictionary;

        /// <summary>
        /// Assembly tables context - contains all tables used for building target assembly.
        /// </summary>
        protected readonly nanoTablesContext _context;

        /// <summary>
        /// Lookup table for finding item ID by item value.
        /// </summary>
        protected IEnumerable<T> _items;

        public IEnumerable<T> Items => _items;

        private readonly IEqualityComparer<T> _comparer;

        /// <summary>
        /// Creates new instance of <see cref="nanoReferenceTableBase{T}"/> object.
        /// </summary>
        /// <param name="nanoTableItems">List of items for initial loading.</param>
        /// <param name="comparer">Custom comparer for items (type-specific).</param>
        /// <param name="context">
        /// Assembly tables context - contains all tables used for building target assembly.
        /// </param>
        protected nanoReferenceTableBase(
            IEnumerable<T> nanoTableItems,
            IEqualityComparer<T> comparer,
            nanoTablesContext context)
        {
            _idsByItemsDictionary = nanoTableItems
                .Select((reference, index) => new { reference, index })
                .ToDictionary(item => item.reference, item => (ushort)item.index,
                    comparer);

            _context = context;

            _comparer = comparer;

            _items = nanoTableItems;
        }

        /// <inheritdoc/>
        public void Write(
            nanoBinaryWriter writer)
        {
            foreach (var item in _idsByItemsDictionary
                .OrderBy(item => item.Value)
                .Select(item => item.Key))
            {
                WriteSingleItem(writer, item);
            }
        }

        public void ForEachItems(Action<uint, T> action)
        {
            foreach (var item in _idsByItemsDictionary
                .OrderBy(item => item.Value))
            {
                action(item.Value, item.Key);
            }
        }

        /// <summary>
        /// Helper method for allocating strings from table before table will be written.
        /// </summary>
        public void AllocateStrings()
        {
            foreach (var item in _idsByItemsDictionary
                .OrderBy(item => item.Value)
                .Select(item => item.Key))
            {
                AllocateSingleItemStrings(item);
            }
        }

        /// <summary>
        /// Writes string reference ID related to passed string value into output stream.
        /// </summary>
        /// <param name="writer">Target binary writer for writing reference ID.</param>
        /// <param name="value">String value for obtaining reference and writing.</param>
        protected void WriteStringReference(
            nanoBinaryWriter writer,
            string value)
        {
            writer.WriteUInt16(GetOrCreateStringId(value));
        }

        /// <summary>
        /// Gets existing or creates new string reference ID for provided string value.
        /// </summary>
        /// <param name="value">String value for lookup in string literals table.</param>
        /// <returns>String reference ID which can be used for filling metadata and byte code.</returns>
        protected ushort GetOrCreateStringId(
            string value)
        {
            return _context.StringTable.GetOrCreateStringId(value);
        }

        /// <summary>
        /// Helper method for lookup in internal dictionary. Wraps
        /// <see cref="IDictionary{TKey,TValue}.TryGetValue"/> method.
        /// </summary>
        /// <param name="key">Key value for lookup.</param>
        /// <param name="id">Item reference identifier.</param>
        /// <returns>Returns <c>true</c> if item found, otherwise returns <c>false</c>.</returns>
        protected bool TryGetIdByValue(
            T key,
            out ushort id)
        {
            return _idsByItemsDictionary.TryGetValue(key, out id);
        }

        /// <summary>
        /// Provides concrete implementation for string allocation method in this table.
        /// </summary>
        /// <param name="item">Table item for allocating strings.</param>
        protected virtual void AllocateSingleItemStrings(T item)
        {
        }

        /// <summary>
        /// Inherited class should provides concrete implementation for writing single table item here.
        /// </summary>
        /// <param name="writer">Target binary writer for writing item data.</param>
        /// <param name="item">Single table item for writing into ouptut stream.</param>
        protected abstract void WriteSingleItem(
            nanoBinaryWriter writer,
            T item);

        /// <summary>
        /// Remove unused items from table.
        /// </summary>
        public virtual void RemoveUnusedItems(HashSet<MetadataToken> set)
        {
            // build a collection of the current items that are present in the used items set
            List<T> usedItems = new List<T>();

            foreach (var item in _idsByItemsDictionary
                                    .Where(item => set.Contains(((IMetadataTokenProvider)item.Key).MetadataToken)))
            {
                usedItems.Add(item.Key);
            }

            // re-create the items dictionary with the used items only
            _idsByItemsDictionary = usedItems
            .Select((reference, index) => new { reference, index })
            .ToDictionary(item => item.reference, item => (ushort)item.index,
                _comparer);

            _items = usedItems;
        }
    }
}
