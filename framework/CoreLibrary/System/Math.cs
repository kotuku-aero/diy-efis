//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System
{

  internal static class MathInternal
  {
    internal static int Abs(int val) { return CanFly.Runtime.Abs(val); }

    internal static int Min(int val1, int val2) { return CanFly.Runtime.Min(val1, val2); }

    internal static int Max(int val1, int val2) { return CanFly.Runtime.Max(val1, val2); }
  }
}
