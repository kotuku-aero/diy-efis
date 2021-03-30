//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Collections.Generic;

namespace CanFly.Tools.MetadataProcessor.Core
{
    public class TypeRef
    {
        public string ReferenceId;

        public string Scope;

        public string Name;

        public List<MemberRef> MemberReferences = new List<MemberRef>();
    }
}
