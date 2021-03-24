//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CanFly.Tools.MetadataProcessor
{
    /// <summary>
    /// Encapsulates logic for storing strings list and writing this
    /// list into target assembly in .NET nanoFramework format.
    /// </summary>
    public sealed class StringTable : IAssemblyTable
    {
        /// <summary>
        /// Default implementation of <see cref="ICustomStringSorter"/> interface.
        /// Do nothing and just returns original sequence of string literals.
        /// </summary>
        private sealed class EmptyStringSorter : ICustomStringSorter
        {
            /// <inheritdoc/>
            public IEnumerable<string> Sort(
                ICollection<string> strings)
            {
                return strings;
            }
        }

        /// <summary>
        /// Maps for each unique string and related identifier (offset in strings table).
        /// </summary>
        private Dictionary<string, ushort> _idsByStrings =
            new Dictionary<string, ushort>(StringComparer.Ordinal);

        /// <summary>
        /// Concrete implementation of string literals sorting algorithm (used by UTs).
        /// </summary>
        private readonly ICustomStringSorter _stringSorter;

        /// <summary>
        /// Last available string identifier.
        /// </summary>
        private ushort _lastAvailableId;

        private ushort _lastPreAllocatedId;

        /// <summary>
        /// Assembly tables context - contains all tables used for building target assembly.
        /// </summary>
        private readonly TablesContext _context;

        /// <summary>
        /// Last pre-allocated string identifier.
        /// </summary>
        public ushort LastPreAllocatedId { get => _lastPreAllocatedId; }

        /// <summary>
        /// Creates new instance of <see cref="StringTable"/> object.
        /// </summary>
        public StringTable(
            TablesContext context,
            ICustomStringSorter stringSorter = null)
        {
            GetOrCreateStringId(string.Empty); // First item in string table always empty string
            _stringSorter = stringSorter ?? new EmptyStringSorter();

            _context = context;
        }

        /// <summary>
        /// Gets existing or creates new string reference identifier related to passed string value.
        /// </summary>
        /// <remarks>
        /// Identifier is offset in strings table or just number from table of pre-defined constants.
        /// </remarks>
        /// <param name="value">String value for obtaining identifier.</param>
        /// <param name="useConstantsTable">
        /// If <c>true</c> hard-coded string constants table will be used (should be <c>false</c>
        /// for byte code writer because only loader use this pre-defined string table optimization).
        /// </param>
        /// <returns>Existing identifier if string already in table or new one.</returns>
        public ushort GetOrCreateStringId(
            string value,
            bool useConstantsTable = true)
        {
            ushort id;
            if (useConstantsTable && nanoStringsConstants.TryGetStringIndex(value, out id))
            {
                return id;
            }
            if (!_idsByStrings.TryGetValue(value, out id))
            {
                id = _lastAvailableId;
                _idsByStrings.Add(value, id);
                var length = Encoding.UTF8.GetBytes(value).Length + 1;
                _lastAvailableId += (ushort)(length);
            }
            return id;
        }

        /// <summary>
        /// Try to get a string value from the table providing the reference identifier.
        /// </summary>
        /// <param name="id">Existing identifier in table.</param>
        /// <returns>The string value or a null if the identifier doesn't exist.</returns>
        public string TryGetString(ushort id)
        {
            // try to get string from id table
            var output = _idsByStrings.FirstOrDefault(s => s.Value == id).Key;

            if(output == null)
            {
                // try to find string in string constant table
                output = nanoStringsConstants.TryGetString(id);
            }

            return output;
        }

        /// <inheritdoc/>
        public void Write(
            CLRBinaryWriter writer)
        {
            foreach (var item in _idsByStrings
                .OrderBy(item => item.Value)
                .Select(item => item.Key))
            {
                writer.WriteString(item);
            }
        }

        /// <summary>
        /// Adds all string constants from <paramref name="fakeStringTable"/> table into this one.
        /// </summary>
        /// <param name="fakeStringTable">Additional string table for merging with this one.</param>
        internal void MergeValues(
            StringTable fakeStringTable)
        {
            foreach (var item in _stringSorter.Sort(fakeStringTable._idsByStrings.Keys))
            {
                GetOrCreateStringId(item, false);
            }
        }

        public void RemoveUnusedItems(HashSet<MetadataToken> items)
        {
            var setAll = new HashSet<MetadataToken>();
            List<string> usedStrings = new List<string>();

            // build a collection of the existing tokens
            foreach (var a in _idsByStrings)
            {
                setAll.Add(new MetadataToken(TokenType.String, a.Value));
            }

            // remove the ones that are used
            foreach (var t in items)
            {
                if(setAll.Remove(t))
                {
                    usedStrings.Add(TryGetString((ushort)t.ToUInt32()));
                }
            }

            // reset dictionary
            _idsByStrings = new Dictionary<string, ushort>(StringComparer.Ordinal);
            
            // and last ID too
            _lastAvailableId = 0;

            // First item in string table always empty string
            GetOrCreateStringId(string.Empty);

            // Pre-allocate strings from some tables
            _context.AssemblyReferenceTable.AllocateStrings();
            _context.TypeReferencesTable.AllocateStrings();


            var memberReferences = _context.AssemblyDefinition.MainModule.GetMemberReferences();
            List<MemberReference> memberReferencesInUse = new List<MemberReference>();

            foreach (var item in memberReferences)
            {
                var memberRef = _context.TypeReferencesTable.Items.FirstOrDefault(m => m.FullName.Contains(item.DeclaringType.FullName));
                if (memberRef != null)
                {
                    memberReferencesInUse.Add(item);
                }
            }

            foreach (var item in memberReferencesInUse)
            {
                GetOrCreateStringId(item.Name);
            }

            foreach (var item in _context.TypeDefinitionTable.Items)
            {
                GetOrCreateStringId(item.Namespace);
                GetOrCreateStringId(item.Name);

                foreach (var f in item.Fields.Where(i => !i.IsStatic))
                {
                    GetOrCreateStringId(f.Name);
                }

                foreach (var m in item.Methods)
                {
                    GetOrCreateStringId(m.Name);
                }
            }

            _lastPreAllocatedId = _idsByStrings.Last().Value;

            // fill in the dictionary with the used strings
            foreach (var s in usedStrings)
            {
                GetOrCreateStringId(s);
            }
        }

        public Dictionary<string, ushort> GetItems()
        {
            return _idsByStrings;
        }
    }
}
