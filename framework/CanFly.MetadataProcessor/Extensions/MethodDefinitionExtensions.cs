//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System.Linq;
using System.Text;

namespace CanFly.Tools.MetadataProcessor.Core.Extensions
{
    internal static class MethodDefinitionExtensions
    {
        public static string FullName(this MethodDefinition value)
        {
            if(value.GenericParameters.Count == 0)
            {
                return value.Name;
            }
            else
            {
                StringBuilder name = new StringBuilder(value.Name);
                name.Append("<");

                foreach(var p in value.GenericParameters)
                {
                    name.Append(p.Name);
                    if (!p.Equals(value.GenericParameters.Last()))
                    {
                        name.Append(",");
                    }
                }

                name.Append(">");

                return name.ToString();
            }
        }
    }
}
