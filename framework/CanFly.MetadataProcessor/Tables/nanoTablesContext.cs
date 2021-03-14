//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System;
using System.Collections.Generic;
using System.Linq;

namespace CanFly.Tools.MetadataProcessor
{
    public sealed class nanoTablesContext
    {
        internal static HashSet<string> IgnoringAttributes { get; } = new HashSet<string>(StringComparer.Ordinal)
            {
                // Assembly-level attributes
                "System.Reflection.AssemblyFileVersionAttribute",
                "System.Runtime.InteropServices.ComVisibleAttribute",
                "System.Runtime.InteropServices.GuidAttribute",

                // Compiler-specific attributes
                //"System.ParamArrayAttribute",
                //"System.SerializableAttribute",
                //"System.NonSerializedAttribute",
                //"System.Runtime.InteropServices.StructLayoutAttribute",
                //"System.Runtime.InteropServices.LayoutKind",
                //"System.Runtime.InteropServices.OutAttribute",
                //"System.Runtime.CompilerServices.ExtensionAttribute",
                //"System.Runtime.CompilerServices.MethodImplAttribute",
                //"System.Runtime.CompilerServices.InternalsVisibleToAttribute",
                //"System.Runtime.CompilerServices.IndexerNameAttribute",
                //"System.Runtime.CompilerServices.MethodImplOptions",
                //"System.Reflection.FieldNoReflectionAttribute",

                // Debugger-specific attributes
                "System.Diagnostics.DebuggableAttribute",
                "System.Diagnostics.DebuggerBrowsableAttribute",
                "System.Diagnostics.DebuggerBrowsableState",
                "System.Diagnostics.DebuggerHiddenAttribute",
                "System.Diagnostics.DebuggerNonUserCodeAttribute",
                "System.Diagnostics.DebuggerStepThroughAttribute",
                "System.Diagnostics.DebuggerDisplayAttribute",

                // Compile-time attributes
                "System.AttributeUsageAttribute",
                "System.CLSCompliantAttribute",
                "System.FlagsAttribute",
                "System.ObsoleteAttribute",
                "System.Diagnostics.ConditionalAttribute",

                // Intellisense filtering attributes
                "System.ComponentModel.EditorBrowsableAttribute",

                //Not supported
                "System.Reflection.DefaultMemberAttribute",

                // Not supported attributes
                "System.MTAThreadAttribute",
                "System.STAThreadAttribute",
            };

        public nanoTablesContext(
            AssemblyDefinition assemblyDefinition,
            List<string> explicitTypesOrder,
            List<string> classNamesToExclude,
            ICustomStringSorter stringSorter,
            bool applyAttributesCompression,
            bool verbose,
            bool isCoreLibrary)
        {
            AssemblyDefinition = assemblyDefinition;

            ClassNamesToExclude = classNamesToExclude;

            // check CustomAttributes against list of classes to exclude
            foreach (var item in assemblyDefinition.CustomAttributes)
            {
                // add it to ignore list, if it's not already there
                if ((ClassNamesToExclude.Contains(item.AttributeType.FullName) ||
                     ClassNamesToExclude.Contains(item.AttributeType.DeclaringType?.FullName)) &&
                    !(IgnoringAttributes.Contains(item.AttributeType.FullName) ||
                      IgnoringAttributes.Contains(item.AttributeType.DeclaringType?.FullName)))
                {
                    IgnoringAttributes.Add(item.AttributeType.FullName);
                }
            }

            // check ignoring attributes against ClassNamesToExclude 
            foreach(var className in ClassNamesToExclude)
            {
                if(!IgnoringAttributes.Contains(className))
                {
                    IgnoringAttributes.Add(className);
                }
            }

            var mainModule = AssemblyDefinition.MainModule;

            // External references

            AssemblyReferenceTable = new nanoAssemblyReferenceTable(
                mainModule.AssemblyReferences, this);

            var typeReferences = mainModule.GetTypeReferences();

            TypeReferencesTable = new nanoTypeReferenceTable(
                typeReferences, this);

            var typeReferencesNames = new HashSet<string>(
                typeReferences.Select(item => item.FullName),
                StringComparer.Ordinal);

            var memberReferences = mainModule.GetMemberReferences()
                .Where(item => 
                    (typeReferencesNames.Contains(item.DeclaringType.FullName) || 
                    item.DeclaringType.GetElementType().IsPrimitive ||
                    item.ContainsGenericParameter ||
                    item.DeclaringType.IsGenericInstance))

                .ToList();

            FieldReferencesTable = new nanoFieldReferenceTable(
                memberReferences.OfType<FieldReference>(), this);
            MethodReferencesTable = new nanoMethodReferenceTable(
                memberReferences.OfType<MethodReference>(), this);
           
            // Internal types definitions

            var types = GetOrderedTypes(mainModule, explicitTypesOrder);

            TypeDefinitionTable = new nanoTypeDefinitionTable(types, this);
            
            var fields = types
                .SelectMany(item => GetOrderedFields(item.Fields.Where(field => !field.HasConstant)))
                .ToList();
            FieldsTable = new nanoFieldDefinitionTable(fields, this);
            var methods = types.SelectMany(item => GetOrderedMethods(item.Methods)).ToList();

            MethodDefinitionTable = new nanoMethodDefinitionTable(methods, this);

            NativeMethodsCrc = new NativeMethodsCrc(
                assemblyDefinition,
                ClassNamesToExclude);

            NativeMethodsCrc.UpdateCrc(TypeDefinitionTable);

            AttributesTable = new nanoAttributesTable(
                GetAttributes(types, applyAttributesCompression),
                GetAttributes(fields, applyAttributesCompression),
                GetAttributes(methods, applyAttributesCompression),
                this);

            TypeSpecificationsTable = new nanoTypeSpecificationsTable(this);

            // Resources information

            ResourcesTable = new nanoResourcesTable(
                mainModule.Resources, this);
            ResourceDataTable = new nanoResourceDataTable();

            // Strings and signatures

            SignaturesTable = new nanoSignaturesTable(this);
            StringTable = new nanoStringTable(
                this,
                stringSorter);

            // Byte code table
            ByteCodeTable = new nanoByteCodeTable(this);

            // Additional information

            ResourceFileTable = new nanoResourceFileTable(this);

            // build list of generic parameters belonging to method defs
            List<GenericParameter> methodDefsGenericParameters = new List<GenericParameter>();

            foreach (var m in methods)
            {
                if (m.HasGenericParameters)
                {
                    methodDefsGenericParameters.AddRange(m.GenericParameters);
                }
            }

            var generics = types
                            .SelectMany(t => t.GenericParameters)
                            .Concat(methodDefsGenericParameters)
                            .ToList();

            GenericParamsTable = new nanoGenericParamTable(generics, this);

            // Pre-allocate strings from some tables
            AssemblyReferenceTable.AllocateStrings();
            TypeReferencesTable.AllocateStrings();
            foreach (var item in memberReferences)
            {
                StringTable.GetOrCreateStringId(item.Name);
                
                var fieldReference = item as FieldReference;
                if (fieldReference != null)
                {
                    SignaturesTable.GetOrCreateSignatureId(fieldReference);
                }

                var methodReference = item as MethodReference;
                if (methodReference != null)
                {
                    SignaturesTable.GetOrCreateSignatureId(methodReference);
                }
            }
        }

        /// <summary>
        /// Gets method reference identifier (external or internal) encoded with appropriate prefix.
        /// </summary>
        /// <param name="methodReference">Method reference in Mono.Cecil format.</param>
        /// <returns>Reference identifier for passed <paramref name="methodReference"/> value.</returns>
        public ushort GetMethodReferenceId(
            MethodReference methodReference)
        {
            ushort referenceId;
            if (MethodReferencesTable.TryGetMethodReferenceId(methodReference, out referenceId))
            {
                referenceId |= 0x8000; // External method reference
            }
            else
            {
                MethodDefinitionTable.TryGetMethodReferenceId(methodReference.Resolve(), out referenceId);
            }
            return referenceId;
        }

        public AssemblyDefinition AssemblyDefinition { get; private set; }

        public NativeMethodsCrc NativeMethodsCrc { get; private set; }

        public nanoAssemblyReferenceTable AssemblyReferenceTable { get; private set; }

        public nanoTypeReferenceTable TypeReferencesTable { get; private set; }

        public nanoFieldReferenceTable FieldReferencesTable { get; private set; }

        public nanoGenericParamTable GenericParamsTable { get; private set; }

        public nanoMethodReferenceTable MethodReferencesTable { get; private set; }

        public nanoFieldDefinitionTable FieldsTable { get; private set; }

        public nanoMethodDefinitionTable MethodDefinitionTable { get; private set; }

        public nanoTypeDefinitionTable TypeDefinitionTable { get; private set; }

        public nanoAttributesTable AttributesTable { get; private set; }

        public nanoTypeSpecificationsTable TypeSpecificationsTable { get; private set; }

        public nanoResourcesTable ResourcesTable { get; private set; }

        public nanoResourceDataTable ResourceDataTable { get; private set; }

        public nanoSignaturesTable SignaturesTable { get; private set; }

        public nanoStringTable StringTable { get; private set; }

        public nanoByteCodeTable ByteCodeTable { get; private set; }

        public nanoResourceFileTable ResourceFileTable { get; private set; }

        public static List<string> ClassNamesToExclude { get; private set; }
        public bool MinimizeComplete { get; internal set; } = false;

        private IEnumerable<Tuple<CustomAttribute, ushort>> GetAttributes(
            IEnumerable<ICustomAttributeProvider> types,
            bool applyAttributesCompression)
        {
            if (applyAttributesCompression)
            {
                return types.SelectMany(
                    (item, index) => item.CustomAttributes
                        .Where(attr => !IsAttribute(attr.AttributeType))
                        .OrderByDescending(attr => attr.AttributeType.FullName)
                        .Select(attr => new Tuple<CustomAttribute, ushort>(attr, (ushort)index)));
                
            }
            return types.SelectMany(
                (item, index) => item.CustomAttributes
                    .Where(attr => !IsAttribute(attr.AttributeType))
                    .Select(attr => new Tuple<CustomAttribute, ushort>(attr, (ushort)index)));
        }

        private bool IsAttribute(
            MemberReference typeReference)
        {
            return
                (IgnoringAttributes.Contains(typeReference.FullName) ||
                 IgnoringAttributes.Contains(typeReference.DeclaringType?.FullName));
        }

        private static List<TypeDefinition> GetOrderedTypes(
            ModuleDefinition mainModule,
            List<string> explicitTypesOrder)
        {
            var unorderedTypes = mainModule.GetTypes()
                .Where(item => item.FullName != "<Module>")
                .ToList();

            if (explicitTypesOrder == null || explicitTypesOrder.Count == 0)
            {
                return SortTypesAccordingUsages(
                    unorderedTypes, mainModule.FileName);
            }

            return explicitTypesOrder
                .Join(unorderedTypes, outer => outer, inner => inner.FullName, (inner, outer) => outer)
                .ToList();
        }

        private static List<TypeDefinition> SortTypesAccordingUsages(
            IEnumerable<TypeDefinition> types,
            string mainModuleName)
        {
            var processedTypes = new HashSet<string>(StringComparer.Ordinal);
            return SortTypesAccordingUsagesImpl(
                types.OrderBy(item => item.FullName),
                mainModuleName, processedTypes)
                .ToList();
        }

        private static IEnumerable<TypeDefinition> SortTypesAccordingUsagesImpl(
            IEnumerable<TypeDefinition> types,
            string mainModuleName,
            ISet<string> processedTypes)
        {
            foreach (var type in types)
            {
                if (processedTypes.Contains(type.FullName))
                {
                    continue;
                }

                if (type.DeclaringType != null)
                {
                    foreach (var declaredIn in SortTypesAccordingUsagesImpl(
                        Enumerable.Repeat(type.DeclaringType, 1), mainModuleName, processedTypes))
                    {
                        yield return declaredIn;
                    }
                }

                foreach (var implement in SortTypesAccordingUsagesImpl(
                    type.Interfaces.Select(itf => itf.InterfaceType.Resolve())
                        .Where(item => item.Module.FileName == mainModuleName),
                    mainModuleName, processedTypes))
                {
                    yield return implement;
                }

                if (processedTypes.Add(type.FullName))
                {
                    var operands = type.Methods
                        .Where(item => item.HasBody)
                        .SelectMany(item => item.Body.Instructions)
                        .Select(item => item.Operand)
                        .OfType<MethodReference>()
                        .ToList();

                    foreach (var fieldType in SortTypesAccordingUsagesImpl(
                        operands.SelectMany(GetTypesList)
                            .Where(item => item.Module.FileName == mainModuleName),
                        mainModuleName, processedTypes))
                    {
                        yield return fieldType;
                    }

                    yield return type;
                }
            }
        }

        private static IEnumerable<TypeDefinition> GetTypesList(
            MethodReference methodReference)
        {
            var returnType = methodReference.ReturnType.Resolve();
            if (returnType != null && returnType.FullName != "System.Void")
            {
                yield return returnType;
            }
            foreach (var parameter in methodReference.Parameters)
            {
                var parameterType = parameter.ParameterType.Resolve();
                if (parameterType != null)
                {
                    yield return parameterType;
                }
            }
        }

        internal static IEnumerable<MethodDefinition> GetOrderedMethods(
            IEnumerable<MethodDefinition> methods)
        {
            var ordered = methods
                .ToList();

            foreach (var method in ordered.Where(item => item.IsVirtual))
            {
                yield return method;
            }

            foreach (var method in ordered.Where(item => !(item.IsVirtual || item.IsStatic)))
            {
                yield return method;
            }

            foreach (var method in ordered.Where(item => item.IsStatic))
            {
                yield return method;
            }
        }

        private static IEnumerable<FieldDefinition> GetOrderedFields(
            IEnumerable<FieldDefinition> fields)
        {
            var ordered = fields
                .ToList();

            foreach (var method in ordered.Where(item => item.IsStatic))
            {
                yield return method;
            }

            foreach (var method in ordered.Where(item => !item.IsStatic))
            {
                yield return method;
            }
        }

        internal void ResetByteCodeTable()
        {
            ByteCodeTable = new nanoByteCodeTable(this);
        }

        internal void ResetSignaturesTable()
        {
            SignaturesTable = new nanoSignaturesTable(this);
        }

        internal void ResetResourcesTables()
        {
            ResourcesTable = new nanoResourcesTable(
               AssemblyDefinition.MainModule.Resources, this);
            ResourceDataTable = new nanoResourceDataTable();
            ResourceFileTable = new nanoResourceFileTable(this);
        }
    }
}
