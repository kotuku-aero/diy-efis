//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using System.Collections.Generic;

namespace CanFly.Tools.MetadataProcessor
{
    /// <summary>
    /// Helper class for sorting string literals before merging (solve strings order problem).
    /// </summary>
    public interface ICustomStringSorter
    {
        /// <summary>
        /// Sorts input sequence according needed logic.
        /// </summary>
        /// <param name="strings">Existing string listerals list.</param>
        /// <returns>Original string listerals list sorted according test pattern.</returns>
        IEnumerable<string> Sort(
            ICollection<string> strings);
    }
}