//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System;
using System.Collections.Generic;

namespace CanFly.Tools.MetadataProcessor.Core
{
    public class AssemblyLookupTable
    {
        public bool IsCoreLib;

        public string Name;
        public string AssemblyName;
        public string HeaderFileName;
        public string NativeCRC32;

        public Version NativeVersion;

        public List<MethodStub> LookupTable = new List<MethodStub>();
    }
}
