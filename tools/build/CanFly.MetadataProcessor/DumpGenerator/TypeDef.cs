//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Collections.Generic;

namespace CanFly.Tools.MetadataProcessor.Core
{
    public class TypeDef
    {
        public string ReferenceId;

        public string Flags;

        public string ExtendsType;

        public string EnclosedType;

        public string Name;

        public List<GenericParam> GenericParameters = new List<GenericParam>();
        public List<FieldDef> FieldDefinitions = new List<FieldDef>();
        public List<MethodDef> MethodDefinitions = new List<MethodDef>();
        public List<InterfaceDef> InterfaceDefinitions = new List<InterfaceDef>();

    }
}
