//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Text.RegularExpressions
{
    /// <summary>
    /// Escape codes used by the Regex Engine
    /// </summary>
    internal sealed class EscapeCode
    {
        public const char Alphanumeric = 'w';
        public const char NonAlphanumeric = 'W';
        public const char WordBoundry = 'b';
        public const char NonWordBoundry = 'B';
        public const char Whitespace = 's';
        public const char NonWhitespace = 'S';
        public const char Digit = 'd';
        public const char NonDigit = 'D';
    }
}
