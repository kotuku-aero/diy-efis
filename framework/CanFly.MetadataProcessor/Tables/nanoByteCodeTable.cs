//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace CanFly.Tools.MetadataProcessor
{
    /// <summary>
    /// Encapsulates logic for storing method bodies (byte code) list and writing
    /// this collected list into target assembly in .NET nanoFramework format.
    /// </summary>
    public sealed class nanoByteCodeTable : InanoTable
    {
        /// <summary>
        /// Maps method bodies (in form of byte array) to method identifiers.
        /// </summary>
        private readonly IList<MethodDefinition> _methods = new List<MethodDefinition>();

        /// <summary>
        /// Maps method full names to method RVAs (offsets in resulting table).
        /// </summary>
        private readonly IDictionary<string, ushort> _rvasByMethodNames =
            new Dictionary<string, ushort>(StringComparer.Ordinal);

        /// <summary>
        /// Temporary string table for code generators used during initial load.
        /// </summary>
        private readonly nanoStringTable _fakeStringTable;

        /// <summary>
        /// Assembly tables context - contains all tables used for building target assembly.
        /// </summary>
        private readonly nanoTablesContext _context;

        /// <summary>
        /// Last available method RVA.
        /// </summary>
        private ushort _lastAvailableRva;

        /// <summary>
        /// Creates new instance of <see cref="nanoByteCodeTable"/> object.
        /// </summary>
        /// <param name="context">
        /// Assembly tables context - contains all tables used for building target assembly.
        /// </param>
        public nanoByteCodeTable(
            nanoTablesContext context)
        {
            _context = context;

            _fakeStringTable = new nanoStringTable(_context);
        }

        /// <summary>
        /// Next method identifier. Used for reproducing strange original MetadataProcessor behaviour.
        /// </summary>
        public ushort NextMethodId { get { return (ushort)_methods.Count; } }

        /// <summary>
        /// Temporary string table for code generators used during initial load.
        /// </summary>
        public nanoStringTable FakeStringTable { get { return _fakeStringTable; } }

        /// <summary>
        /// Returns method reference ID (index in methods definitions table) for passed method definition.
        /// </summary>
        /// <param name="method">Method definition in Mono.Cecil format.</param>
        /// <returns>
        /// New method reference ID (byte code also prepared for writing as part of process).
        /// </returns>
        public ushort GetMethodId(
            MethodDefinition method)
        {
            var rva = method.HasBody ? _lastAvailableRva : (ushort)0xFFFF;
            var id = (ushort)_methods.Count;

            var byteCode = CreateByteCode(method);

            _methods.Add(method);
            _lastAvailableRva += (ushort)byteCode.Length;

            _rvasByMethodNames.Add(method.FullName, rva);

            return id;
        }

        /// <summary>
        /// Returns method RVA (offset in byte code table) for passed method reference.
        /// </summary>
        /// <param name="method">Method reference in Mono.Cecil format.</param>
        /// <returns>
        /// Method RVA (method should be generated using <see cref="GetMethodId"/> before this call.
        /// </returns>
        public ushort GetMethodRva(
            MethodReference method)
        {
            ushort rva;
            return (_rvasByMethodNames.TryGetValue(method.FullName, out rva) ? rva : (ushort)0xFFFF);
        }

        /// <inheritdoc/>
        public void Write(
            nanoBinaryWriter writer)
        {
            foreach (var method in _methods)
            {
                writer.WriteBytes(CreateByteCode(method, writer));
            }
        }

        /// <summary>
        /// Updates main string table with strings stored in temp string table before code generation.
        /// </summary>
        internal void UpdateStringTable()
        {
            _context.StringTable.MergeValues(_fakeStringTable);
        }

        private byte[] CreateByteCode(
            MethodDefinition method)
        {
            if (!method.HasBody)
            {
                return new byte[0];
            }

            using (var stream = new MemoryStream())
            using (var writer = new BinaryWriter(stream))
            {
                var codeWriter = new CodeWriter(
                    method, nanoBinaryWriter.CreateBigEndianBinaryWriter(writer),
                    _fakeStringTable, _context);
                codeWriter.WriteMethodBody();
                return stream.ToArray();
            }
        }

        private byte[] CreateByteCode(
            MethodDefinition method,
            nanoBinaryWriter writer)
        {
            if (!method.HasBody)
            {
                return new byte[0];
            }

            using(var stream = new MemoryStream())
            {
                var codeWriter = new  CodeWriter(
                    method, writer.GetMemoryBasedClone(stream),
                    _context.StringTable, _context);
                codeWriter.WriteMethodBody();
                return stream.ToArray();
            }
        }
    }
}
