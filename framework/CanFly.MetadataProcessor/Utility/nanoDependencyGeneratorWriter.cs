//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Xml;

namespace CanFly.Tools.MetadataProcessor
{
    internal sealed class nanoDependencyGeneratorWriter
    {
        private readonly TablesContext _context;
        private readonly AssemblyDefinition _assemblyDefinition;

        public nanoDependencyGeneratorWriter(
            AssemblyDefinition assemblyDefinition, 
            TablesContext context)
        {
            _context = context;
            _assemblyDefinition = assemblyDefinition;
    }

    public void Write(
            XmlWriter writer)
        {
            writer.WriteStartElement("AssemblyGraph");

            writer.WriteStartElement("Assembly");
            writer.WriteAttributeString("Name", _context.AssemblyDefinition.Name.Name);
            writer.WriteAttributeString("Version", _context.AssemblyDefinition.Name.Version.ToString());
            writer.WriteAttributeString("Hash", ComputeAssemblyHash(_assemblyDefinition.MainModule));
            writer.WriteAttributeString("Flags", "0x00000000");

            // references
            foreach(var a in _context.AssemblyDefinition.MainModule.AssemblyReferences)
            {
                writer.WriteStartElement("AssemblyRef");
                writer.WriteAttributeString("Name", a.Name);
                writer.WriteAttributeString("Version", a.Version.ToString());
                writer.WriteAttributeString("Hash", ComputeAssemblyHash(_assemblyDefinition.MainModule));
                writer.WriteAttributeString("Flags", "0x00000000");
                // end AssemblyRef element
                writer.WriteEndElement();
            }

            // types
            foreach (var t in _context.AssemblyDefinition.MainModule.Types)
            {
                writer.WriteStartElement("Type");
                writer.WriteAttributeString("Name", t.FullName);
                writer.WriteAttributeString("Hash", ComputeAssemblyHash(_assemblyDefinition.MainModule));
                // end Type element
                writer.WriteEndElement();
            }

            // end Assembly element
            writer.WriteEndElement();

            // referenced assemblies
            foreach (var a in _context.AssemblyReferenceTable.Items)
            {
                writer.WriteStartElement("Assembly");
                writer.WriteAttributeString("Name", a.Name);
                writer.WriteAttributeString("Version", a.Version.ToString());
                writer.WriteAttributeString("Hash", ComputeAssemblyHash(_assemblyDefinition.MainModule));
                writer.WriteAttributeString("Flags", "0x00000000");

                //types
                // TODO
                //foreach (var t in _context.)
                //{
                //    writer.WriteStartElement("Type");
                //    writer.WriteAttributeString("Name", t.FullName);
                //    writer.WriteAttributeString("Hash", ComputeAssemblyHash(_assemblyDefinition.MainModule));
                //    // end Type element
                //    writer.WriteEndElement();
                //}

                // end Assembly element
                writer.WriteEndElement();
            }



            //_context.AssemblyDefinition.Modules((token, item) => WriteClassInfo(writer, token, item));
            //_context.AssemblyReferenceTable.ForEachItems((token, item) => WriteClassInfo(writer, token, item));


            foreach (var module in _assemblyDefinition.Modules)
            {
            }

            //writer.WriteStartElement("Assembly");

            //WriteTokensPair(writer, _context.AssemblyDefinition.MetadataToken.ToUInt32(), 0x00000000);
            //writer.WriteElementString("FileName", _context.AssemblyDefinition.MainModule.Name);
            //WriteVersionInfo(writer, _context.AssemblyDefinition.Name.Version);

            //writer.WriteStartElement("Classes");
            //_context.TypeDefinitionTable.ForEachItems((token, item) => WriteClassInfo(writer, token, item));

            writer.WriteEndDocument();            
        }

        private string ComputeAssemblyHash(ModuleDefinition module)
        {
            return "0x00000000";
        }

        private void WriteVersionInfo(
            XmlWriter writer,
            Version version)
        {
            writer.WriteStartElement("Version");

            writer.WriteElementString("Major", version.Major.ToString("D", CultureInfo.InvariantCulture));
            writer.WriteElementString("Minor", version.Minor.ToString("D", CultureInfo.InvariantCulture));
            writer.WriteElementString("Build", version.Build.ToString("D", CultureInfo.InvariantCulture));
            writer.WriteElementString("Revision", version.Revision.ToString("D", CultureInfo.InvariantCulture));

            writer.WriteEndElement();
        }

        private void WriteClassInfo(
            XmlWriter writer,
            uint nanoClrItemToken,
            TypeDefinition item)
        {
            writer.WriteStartElement("Class");

            WriteTokensPair(writer, item.MetadataToken.ToUInt32(), 0x04000000 | nanoClrItemToken);

            writer.WriteStartElement("Methods");
            foreach (var tuple in GetMethodsTokens(item.Methods))
            {
                writer.WriteStartElement("Method");

                WriteTokensPair(writer, tuple.Item1, tuple.Item2);

                if (!tuple.Item3.HasBody)
                {
                    writer.WriteElementString("HasByteCode", "false");
                }
                writer.WriteStartElement("ILMap");
                foreach (var offset in _context.TypeDefinitionTable.GetByteCodeOffsets(tuple.Item1))
                {
                    writer.WriteStartElement("IL");

                    writer.WriteElementString("CLR", "0x" + offset.Item1.ToString("X8", CultureInfo.InvariantCulture));
                    writer.WriteElementString("nanoCLR", "0x" + offset.Item2.ToString("X8", CultureInfo.InvariantCulture));

                    writer.WriteEndElement();
                }
                writer.WriteEndElement();

                writer.WriteEndElement();
            }
            writer.WriteEndElement();

            writer.WriteStartElement("Fields");
            foreach (var pair in GetFieldsTokens(item.Fields))
            {
                writer.WriteStartElement("Field");

                WriteTokensPair(writer, pair.Item1, pair.Item2);

                writer.WriteEndElement();
            }
            writer.WriteEndElement();

            writer.WriteEndElement();
        }

        private IEnumerable<Tuple<uint, uint, MethodDefinition>> GetMethodsTokens(
            IEnumerable<MethodDefinition> methods)
        {
            foreach (var method in methods)
            {
                ushort fieldToken;
                _context.MethodDefinitionTable.TryGetMethodReferenceId(method, out fieldToken);
                yield return new Tuple<uint, uint, MethodDefinition>(
                    method.MetadataToken.ToUInt32(), 0x06000000 | (uint)fieldToken, method);
            }
        }

        private IEnumerable<Tuple<uint, uint>> GetFieldsTokens(
            IEnumerable<FieldDefinition> fields)
        {
            foreach (var field in fields.Where(item => !item.HasConstant))
            {
                ushort fieldToken;
                _context.FieldsTable.TryGetFieldReferenceId(field, false, out fieldToken);
                yield return new Tuple<uint, uint>(
                    field.MetadataToken.ToUInt32(), 0x05000000 | (uint)fieldToken);
            }
        }

        private void WriteTokensPair(
            XmlWriter writer,
            uint clrToken,
            uint nanoClrToken)
        {
            writer.WriteStartElement("Token");

            writer.WriteElementString("CLR", "0x" + clrToken.ToString("X8", CultureInfo.InvariantCulture));
            writer.WriteElementString("nanoCLR", "0x" + nanoClrToken.ToString("X8", CultureInfo.InvariantCulture));

            writer.WriteEndElement();
        }
    }
}
