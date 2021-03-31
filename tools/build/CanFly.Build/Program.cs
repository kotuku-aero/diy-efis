/*
diy-efis
Copyright (C) 2021 Kotuku Aerospace Limited

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice is
subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.

If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
using CanFly.CommandLine;
using CanFly.CommandLine.Text;
using Mono.Cecil;
using System;
using System.Collections.Generic;
using CanFly.Tools.MetadataProcessor;
using CanFly.Tools.MetadataProcessor.Core;
using System.IO;
using System.Xml;

namespace CanFly
{

  class Program
  {
    private sealed class MetadataProcessor
    {
      private readonly Dictionary<string, string> _loadHints =
          new Dictionary<string, string>(StringComparer.Ordinal);

      private AssemblyDefinition _assemblyDefinition;
      private nanoAssemblyBuilder _assemblyBuilder;

      private List<string> _classNamesToExclude = new List<string>();

      internal string PeFileName;

      internal bool Verbose { get; set; }

      internal bool VerboseMinimize { get; set; }

      public bool DumpMetadata { get; internal set; } = false;

      /// <summary>
      /// Emit debug tables in the pe file
      /// </summary>
      public bool DebugInformation { get; set; } = false;

      public void Parse(string fileName)
      {
        try
        {
          if (Verbose) System.Console.WriteLine("Parsing assembly...");

          _assemblyDefinition = AssemblyDefinition.ReadAssembly(fileName,
              new ReaderParameters { AssemblyResolver = new LoadHintsAssemblyResolver(_loadHints) });
        }
        catch (Exception)
        {
          System.Console.Error.WriteLine(
              "Unable to parse input assembly file '{0}' - check if path and file exists.", fileName);
          Environment.Exit(1);
        }
      }

      public void Compile(
          string fileName,
          bool isCoreLibrary)
      {
        try
        {
          if (Verbose) System.Console.WriteLine("Compiling assembly...");

          _assemblyBuilder = new nanoAssemblyBuilder(_assemblyDefinition, _classNamesToExclude, VerboseMinimize, isCoreLibrary);

          using (var stream = File.Open(Path.ChangeExtension(fileName, "tmp"), FileMode.Create, FileAccess.ReadWrite))
          using (var writer = new BinaryWriter(stream))
          {
            _assemblyBuilder.Write(GetBinaryWriter(writer));
          }
        }
        catch (Exception)
        {
          System.Console.Error.WriteLine(
              $"Unable to compile output assembly file '{fileName}' - check parse command results.");
          throw;
        }

        try
        {
          // OK to delete tmp PE file
          File.Delete(Path.ChangeExtension(fileName, "tmp"));

          if (Verbose) System.Console.WriteLine("Minimizing assembly...");

          _assemblyBuilder.Minimize();

          if (Verbose) System.Console.WriteLine("Recompiling assembly...");

          using (var stream = File.Open(fileName, FileMode.Create, FileAccess.ReadWrite))
          using (var writer = new BinaryWriter(stream))
          {
            _assemblyBuilder.Write(GetBinaryWriter(writer));
          }

          using (var writer = XmlWriter.Create(Path.ChangeExtension(fileName, "pdbx")))
          {
            _assemblyBuilder.Write(writer);
          }

          if (DumpMetadata)
          {
            nanoDumperGenerator dumper = new nanoDumperGenerator(
                _assemblyBuilder.TablesContext,
                Path.ChangeExtension(fileName, "dump.txt"));
            dumper.DumpAll();
          }
        }
        catch (ArgumentException ex)
        {
          System.Console.Error.WriteLine($"Exception minimizing assembly: {ex.Message}.");
        }
        catch (Exception)
        {
          System.Console.Error.WriteLine($"Exception minimizing assembly.");
          throw;
        }
      }

      private CLRBinaryWriter GetBinaryWriter(BinaryWriter writer)
      {
        return CLRBinaryWriter.CreateLittleEndianBinaryWriter(writer);
      }

      public void AddLoadHint(
          string assemblyName,
          string assemblyFileName)
      {
        _loadHints[assemblyName] = assemblyFileName;
      }

      public void AddClassToExclude(
          string className)
      {
        _classNamesToExclude.Add(className);
      }

      public void GenerateSkeleton(
          string file,
          string name,
          string project,
          bool withoutInteropCode,
          bool isCoreLibrary)
      {
        try
        {
          if (Verbose) System.Console.WriteLine("Generating skeleton files...");

          var skeletonGenerator = new nanoSkeletonGenerator(
              _assemblyBuilder.TablesContext,
              file,
              name,
              project,
              withoutInteropCode,
              isCoreLibrary);

          skeletonGenerator.GenerateSkeleton();
        }
        catch (Exception ex)
        {
          System.Console.Error.WriteLine("Unable to generate skeleton files.");
          throw;
        }
      }

      public void GenerateDependency(string fileName)
      {
        try
        {
          var dependencyGenerator = new nanoDependencyGenerator(
              _assemblyDefinition,
              _assemblyBuilder.TablesContext,
              fileName);

          using (var writer = XmlWriter.Create(fileName))
          {
            dependencyGenerator.Write(writer);
          }
        }
        catch (Exception)
        {
          System.Console.Error.WriteLine(
              "Unable to generate and write dependency graph for assembly file '{0}'.", fileName);
          throw;
        }
      }
    }

    public class Options
    {
      [Option('c', "compile", Required = false, HelpText = "<path-to-DLL-file> Compiles an assembly into CanFly format. ")]
      public string CompileFilename { get; set; }
      [Option('d', "debug", Required = false, DefaultValue =false, HelpText="Emit debug tables in the assembly.")]
      public bool DebugInformation { get; set; }
      [Option('m', "mscorlib", DefaultValue = false, Required=false, HelpText = "Is the core library")]
      public bool IsCoreLibrary { get; set; }
      [Option('p', "pefile", Required =false, HelpText = "Output file, defaults to c=<filename>.pe")]
      public string OutFile { get; set; }
      [Option('v', "verbose", Required =false, DefaultValue =false, HelpText ="Verbose logging of processor")]
      public bool Verbose { get; set; }

      [HelpOption]
      public string GetUsage()
      {
        return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
      }
    }

    static void Main(string[] args)
    {
      MetadataProcessor md = new MetadataProcessor();

      Options options = new Options();
      if (!Parser.Default.ParseArguments(args, options))
      {
        throw new ApplicationException("Unable to parse options");
      }

      md.Verbose = options.Verbose;
      md.DebugInformation = options.DebugInformation;

      if(!String.IsNullOrEmpty(options.CompileFilename))
      {
        md.Compile(options.CompileFilename, options.IsCoreLibrary);
      }

      if(options.IsCoreLibrary)
      {
        /*
        md.GenerateSkeleton(
            file,
            name,
            project,
            true,
            true);
        */
      }
    }
  }
}
