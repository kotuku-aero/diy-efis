//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System;

namespace CanFly.Tools.MetadataProcessor.Core.Extensions
{
    internal static class TypeDefinitionExtensions
    {
        public static bool IncludeInStub(this TypeDefinition value)
        {
            var typeDefFlags = TypeDefinitionTable.GetFlags(value);

            if ( typeDefFlags.HasFlag(nanoTypeDefinitionFlags.TD_Delegate) ||
                 typeDefFlags.HasFlag(nanoTypeDefinitionFlags.TD_MulticastDelegate))
            {
                return false;
            }

            typeDefFlags = typeDefFlags & nanoTypeDefinitionFlags.TD_Semantics;

            // Only generate a stub for classes and value types.
            if  (typeDefFlags == nanoTypeDefinitionFlags.TD_Semantics_Class ||
                 typeDefFlags == nanoTypeDefinitionFlags.TD_Semantics_ValueType )
            { 
                return true;
            }

            return false;
        }

        public static bool IsToExclude(this TypeDefinition value)
        {
            return TablesContext.ClassNamesToExclude.Contains(value.FullName) ||
                   TablesContext.ClassNamesToExclude.Contains(value.Name) ||
                   TablesContext.ClassNamesToExclude.Contains(value.DeclaringType?.FullName);
        }

        public static EnumDeclaration ToEnumDeclaration(this TypeDefinition source)
        {
            // sanity check (to prevent missuse)
            if(!source.IsEnum)
            {
                throw new ArgumentException("Can clone only TypeDefinition that are Enums.");
            }

            // build enum name
            string enumName = "";


            // if there is a namespace remove it (as there can't be two enums with the same name)
            if(!string.IsNullOrEmpty(source.Namespace))
            {
                enumName = source.FullName.Replace(source.Namespace, "");
                
                // remove trailing dot
                enumName = enumName.Replace(".", "");
            }
            else
            {
                if( 
                    source.DeclaringType != null &&
                    !string.IsNullOrEmpty(source.DeclaringType.Namespace))
                {
                    enumName = source.FullName.Replace(source.DeclaringType.Namespace, "");
                    // remove trailing dot
                    enumName = enumName.Replace(".", "");

                    // replace '/' separator
                    enumName = enumName.Replace("/", "_");
                }
                else if (source.DeclaringType != null)
                {
                    // namespace not showing up, remove everything before the last '.'
                    // namespace not showing up, remove everything before the last '.'
                    enumName = source.FullName.Substring(
                        source.DeclaringType.FullName.LastIndexOf('.'))
                        .Replace(".", "");
                }
                else
                {
                    try
                    {
                        // replace '/' separator
                        enumName = source.FullName.Replace("/", "_");
                    }
                    catch
                    {
                        // something very wrong here...
                        throw new ArgumentException($"Can't process enum type {source.FullName}.");
                    }
                }
            }

            EnumDeclaration myEnum = new EnumDeclaration()
            {
                EnumName = enumName,
                FullName = source.FullName,
                Name = source.Name
            };

            foreach (var f in source.Fields)
            {
                if (f.Name == "value__")
                {
                    // skip value field
                    continue;
                }
                else
                {
                    // enum items are named with the enum name followed by the enum item and respective value
                    // pattern: nnnn_yyyyy
                    var emunItem = new EnumItem()
                        {
                            Name = $"{enumName}_{f.Name}",
                        };

                    emunItem.Value = f.Constant.ToString();

                    myEnum.Items.Add(emunItem);
                }
            }

            return myEnum;
        }
    }
}
