//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Text.RegularExpressions
{
    /// <summary>
    /// MAtch evaluator
    /// </summary>
    /// <param name="match">The match to evaluate</param>
    /// <returns>The evaluation</returns>
    public delegate string MatchEvaluator(Match match);
}
