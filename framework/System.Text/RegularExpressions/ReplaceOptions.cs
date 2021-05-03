//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Text.RegularExpressions
{
    /// <summary>
    /// Replace options
    /// </summary>
    [Flags]
    public enum ReplaceOptions
    {
        /// <summary>
        /// Flag bit that indicates that subst should replace all occurrences of this
        /// regular expression.
        /// </summary>
        ReplaceAll = 0x0000,

        /// <summary>
        /// Flag bit that indicates that subst should only replace the first occurrence
        /// of this regular expression.
        /// </summary>
        ReplaceFirst = 0x0001,

        /// <summary>
        /// Flag bit that indicates that subst should replace backreferences
        /// </summary>
        ReplaceBackrefrences = 0x0002
    }
}
