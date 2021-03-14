//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using System.Collections.Generic;

namespace CanFly.Tools.MetadataProcessor
{
    /// <summary>
    /// Encapsulates logic for storing concrete resource data items list and writing
    /// this collected list into target assembly in .NET nanoFramework format.
    /// </summary>
    public sealed class nanoResourceDataTable : InanoTable
    {
        /// <summary>
        /// List of registered resouce data for writing into output stream "as is".
        /// </summary>
        private readonly IList<byte[]> _dataByteArrays = new List<byte[]>();

        /// <summary>
        /// Gets current offset in resrouces data table (total size of all data blocks).
        /// </summary>
        public int CurrentOffset { get; private set; }

        /// <summary>
        /// Adds new chunk of binary data for resouces into list of resources.
        /// </summary>
        /// <param name="resourceData">Resouce data in binary format.</param>
        public void AddResourceData(
            byte[] resourceData)
        {
            _dataByteArrays.Add(resourceData);
            CurrentOffset += resourceData.Length;
        }

        /// <inheritdoc/>
        public void Write(
            nanoBinaryWriter writer)
        {
            foreach (var item in _dataByteArrays)
            {
                writer.WriteBytes(item);
            }
        }

        /// <summary>
        /// Aligns current data in table by word boundary and return size of alignment.
        /// </summary>
        /// <returns>Number of bytes added into bytes block for proper data alignment.</returns>
        public int AlignToWord()
        {
            var padding = (4 - (CurrentOffset % 4)) % 4;
            if (padding != 0)
            {
                AddResourceData(new byte[padding]);
            }
            return padding;
        }
    }
}
