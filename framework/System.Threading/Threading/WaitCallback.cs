//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Threading
{
    /// <summary>
    /// Represents a callback method to be executed by a thread pool thread.
    /// </summary>
    /// <param name="state">An object containing information to be used by the callback method.</param>
    public delegate void WaitCallback(object state);
}
