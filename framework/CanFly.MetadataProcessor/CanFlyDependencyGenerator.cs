//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using CanFly.Tools.MetadataProcessor.Core.Extensions;
using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;

namespace CanFly.Tools.MetadataProcessor.Core
{
    /// <summary>
    /// Generates dependency graph for a .NET nanoFramework assembly.
    /// </summary>
    public sealed class nanoDependencyGenerator
    {
        private readonly nanoTablesContext _tablesContext;
        private readonly AssemblyDefinition _assemblyDefinition;
        private readonly string _path;
        private readonly string _name;
        private readonly string _project;

        private string _assemblyName;
        private nanoTablesContext tablesContext;
        private string _fileName;

        public nanoDependencyGenerator(
            AssemblyDefinition assemblyDefinition, 
            nanoTablesContext tablesContext, 
            string fileName)
        {
            _assemblyDefinition = assemblyDefinition;
            _tablesContext = tablesContext;
            _fileName = fileName;
        }

        public nanoDependencyGenerator(
            nanoTablesContext tablesContext,
            string path,
            string name,
            string project,
            bool interopCode)
        {
            _tablesContext = tablesContext;
            _path = path;
            _name = name;
            _project = project;

        }

        public void Write(
            XmlWriter xmlWriter)
        {
            var dependencyWriter = new nanoDependencyGeneratorWriter(
                _assemblyDefinition,
                _tablesContext);
            dependencyWriter.Write(xmlWriter);
        }

    }
}
