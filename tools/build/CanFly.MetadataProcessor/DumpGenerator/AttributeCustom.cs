//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Collections.Generic;

namespace CanFly.Tools.MetadataProcessor.Core
{
    public class AttributeCustom
    {
        public string ReferenceId;

        public string Name;

        public string TypeToken;

        public List<AttFixedArgs> FixedArgs = new List<AttFixedArgs>();   
    }
}
