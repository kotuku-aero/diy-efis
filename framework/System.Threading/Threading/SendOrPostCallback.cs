//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Threading
{
    /// <summary>
    /// Represents a method to be called when a message is to be dispatched to a synchronization context.
    /// </summary>
    /// <param name="state"></param>
    public delegate void SendOrPostCallback(object state);
}
