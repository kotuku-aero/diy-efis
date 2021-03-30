//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System;
using System.Collections.Generic;
using System.Reflection;

namespace CanFly.Tools.MetadataProcessor
{
    /// <summary>
    /// Need to use .NET nanoFramework assemblies for resolution.
    /// When calling MDP it's required to provide load hints with pairs of assembly name and assebly path.
    /// </summary>
    public sealed class LoadHintsAssemblyResolver : BaseAssemblyResolver
    {
        /// <summary>
        /// List of 'load hints' - map between assembly name and assembly path.
        /// </summary>
        private readonly IDictionary<string, string> _loadHints;

        /// <summary>
        /// Creates new instance of <see cref="LoadHintsAssemblyResolver"/> object.
        /// </summary>
        /// <param name="loadHints">Metadata transformer load hints.</param>
        public LoadHintsAssemblyResolver(
            IDictionary<string, string> loadHints)
        {
            _loadHints = loadHints;
        }

        /// <inheritdoc/>
        public override AssemblyDefinition Resolve(AssemblyNameReference name)
        {
            try
            {
                if (_loadHints.TryGetValue(name.Name, out string assemblyFileName))
                {
                    return AssemblyDefinition.ReadAssembly(assemblyFileName);
                }
                else
                {
                    return null;
                }
            }
            catch(System.IO.DirectoryNotFoundException)
            {
                Console.Error.WriteLine(
                    $"Unable to find assembly '{name.Name}' in load hints collection.");

                throw;
            }
            catch (Exception)
            {
                throw;
            }
        }
       
        /// <inheritdoc/>
        public override AssemblyDefinition Resolve(AssemblyNameReference name, ReaderParameters parameters)
        {
            try
            {
                if (_loadHints.TryGetValue(new AssemblyName(name.FullName).Name, out string assemblyFileName))
                {
                    return AssemblyDefinition.ReadAssembly(assemblyFileName);
                }
                else
                {
                    return null;
                }
            }
            catch (Exception)
            {
                throw;
            }
        }
    }
}
