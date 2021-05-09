//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Text.RegularExpressions
{
    /// <summary>
    /// The SpecialEscape bit masks and range values
    /// </summary>
    internal enum SpecialEscape
    {
        /// <summary>
        /// Escape complexity mask
        /// </summary>
        Mask = 0xffff0,

        /// <summary>
        /// Escape is really a backreference
        /// </summary>
        BackReference = 0xfffff,

        /// <summary>
        /// Escape isn't really a true character
        /// </summary>
        Complex = 0xffffe,

        /// <summary>
        /// Escape represents a whole class of characters
        /// </summary>
        CharacterClass = 0xffffd,
    }
}
