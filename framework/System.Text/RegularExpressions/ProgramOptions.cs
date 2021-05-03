//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Text.RegularExpressions
{
    /// <summary>
    /// Program optimization Flags
    /// </summary>
    [Flags]
    internal enum ProgramOptions
    {
        None = 0,
        HasBackrefrence = 1,
        HasBeginOfLine = 2
    }
}
