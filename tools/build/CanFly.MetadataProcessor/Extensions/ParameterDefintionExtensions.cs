//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System.Text;
using System.Linq;

namespace CanFly.Tools.MetadataProcessor.Core.Extensions
{
    internal static class ParameterDefintionExtensions
    {
        public static string TypeToString(this ParameterDefinition parameter)
        {
            if(parameter.ParameterType is ByReferenceType byReference)
            {
                // pointer to native type
                return " *" + byReference.TypeSignatureAsString();
            }
            else if (parameter.ParameterType is ArrayType arrayType)
            {
                return "CLR_RT_TypedArray_" + arrayType.ElementType.TypeSignatureAsString();
            }
            else if (parameter.ParameterType.IsValueType)
            {
                return parameter.ParameterType.Resolve().TypeSignatureAsString();
            }
            else
            {
                return parameter.ParameterType.TypeSignatureAsString();
            }
        }
    }
}
