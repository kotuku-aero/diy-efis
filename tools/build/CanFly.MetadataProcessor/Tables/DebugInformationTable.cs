using Mono.Cecil;
using Mono.Cecil.Cil;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

namespace CanFly.Tools.MetadataProcessor
{
  public sealed class DebugInformationTable : IAssemblyTable
  {
    private readonly TablesContext _context;

    private class Source
    {
      public ushort PathId { get; set; }
      public ushort FilenameId { get; set; }
      public Document Document { get; private set; }

      public Source(Document document)
      {
        Document = document;
      }
    }

    private class StatementDefinition
    {
      public MethodDefinition Method { get; set; }
      public FieldDefinition Field { get; set; }
      public ushort StatementStart { get; set; }
    }

    private Dictionary<Source, List<Tuple<StatementDefinition, SequencePoint>>> _sources;
    private IEnumerable<ModuleDefinition> _modules;

    private class SourceComparer : EqualityComparer<Source>
    {
      public override bool Equals(Source x, Source y)
      {
        return x.Document.Url == y.Document.Url;
      }

      public override int GetHashCode(Source obj)
      {
        return obj.Document.Url.GetHashCode();
      }
    }

    public DebugInformationTable(IEnumerable<ModuleDefinition> modules, TablesContext context)
    {
      _context = context;
      _sources = new Dictionary<Source, List<Tuple<StatementDefinition, SequencePoint>>>(new SourceComparer());
      _modules = modules;

      GetOrCreateStringId(string.Empty);
    }

    // first pass, create all of the sources
    public void GenerateSources()
    {
      foreach (ModuleDefinition definition in _modules)
      {
        foreach (TypeDefinition type in definition.GetTypes())
        {
          foreach (MethodDefinition method in type.Methods)
          {
            IDictionary<Instruction, SequencePoint> mapping = method.DebugInformation.GetSequencePointMapping();

            foreach (KeyValuePair<Instruction, SequencePoint> seqPt in mapping)
            {
              Source source = new Source(seqPt.Value.Document);
              if (!_sources.ContainsKey(source))
              {
                Document document = seqPt.Value.Document;

                _sources.Add(source, new List<Tuple<StatementDefinition, SequencePoint>>());

                // the document has a path, need to store the string table for tha path
                string fileName = System.IO.Path.GetFileName(document.Url);

                source.FilenameId = GetOrCreateStringId(fileName, false);
                source.PathId = GetOrCreateStringId(document.Url, false);
              }
            }
          }
        }
      }
    }

    //private uint CalculateOffset(uint offset, List<Tuple<uint, uint>> offsetsTable)
    //{
    //  foreach (Tuple<uint, uint> relocationAddr in offsetsTable)
    //  {
    //    if (relocationAddr.Item1 > offset)
    //      break;

    //    offset -= relocationAddr.Item2;
    //  }

    //  return offset;
    //}

    public void Write(CLRBinaryWriter writer)
    {
      if (!_context.MinimizeComplete)
      {
        return;
      }

      List<MethodDefinition> methods = new List<MethodDefinition>();

      foreach (ModuleDefinition definition in _modules)
      {
        foreach (TypeDefinition type in definition.GetTypes())
        {
          foreach (MethodDefinition method in type.Methods)
          {
            methods.Add(method);
            //Trace.TraceInformation("Process offsets for {0}::{1}", method.DeclaringType.FullName, method.Name);
            // get the offsets table.
            //List<Tuple<uint, uint>> offsetsTable = new List<Tuple<uint, uint>>();

            //Trace.TraceInformation(" Relocation Table:");

            //// adjust the table to match its use
            //uint cumulative = 0;
            //foreach(Tuple<uint, uint> tuple in _context.TypeDefinitionTable.GetByteCodeOffsets(method.MetadataToken.ToUInt32()))
            //{
            //  uint diff = tuple.Item1 - tuple.Item2 - cumulative;
            //  uint index = tuple.Item1;
            //  cumulative += diff;
            //  offsetsTable.Add(new Tuple<uint, uint>(index, diff));

            //  Trace.TraceInformation("  Cecil: {0} -> {1}  CanFly {2}", tuple.Item1, tuple.Item2, diff);
            //}

            IDictionary<Instruction, SequencePoint> mapping = method.DebugInformation.GetSequencePointMapping();

            //Trace.TraceInformation(" Method definition:");

            foreach (KeyValuePair<Instruction, SequencePoint> seqPt in mapping)
            {
              Instruction instruction = seqPt.Key;

              StatementDefinition statement = new StatementDefinition();
              statement.Method = method;

              // instruction start
              // find the mapping
              //statement.StatementStart = (ushort) CalculateOffset((uint) instruction.Offset, offsetsTable);
              statement.StatementStart = (ushort)instruction.Offset;

              //Trace.TraceInformation("  Emit Line {0}, Offset {1}, Relocated to, {2}", seqPt.Value.StartLine, instruction.Offset, statement.StatementStart);

              List<Tuple<StatementDefinition, SequencePoint>> pts = _sources[new Source(seqPt.Value.Document)];

              pts.Add(new Tuple<StatementDefinition, SequencePoint>(statement, seqPt.Value));
            }
          }
        }
      }

      /*
      uint16_t stringTableLength
       uint16_t numSources;
       uint16_t numMethods;
       char stringTable[stringTableLength];
        CLR_RECORD_SOURCE sources[numSources];
        CLR_RECORD_METHOD_DEBUG methods[numMethods]
       */
      writer.WriteUInt16(_lastAvailableId);
      writer.WriteUInt16((ushort)_sources.Count);
      writer.WriteUInt16((ushort)methods.Count);

      // emit the sources
      foreach (KeyValuePair<Source, List<Tuple<StatementDefinition, SequencePoint>>> doc in _sources)
      {
        if (doc.Value.Count > 65535)
        {
          Console.WriteLine($"Cannot process a source '{doc.Key.Document.Url}' that has more than 65535 sequence points.  Will not have debug information.");
          continue;
        }

        writer.WriteUInt16(doc.Key.PathId);
        writer.WriteUInt16(doc.Key.FilenameId);
        writer.WriteUInt16((ushort)doc.Value.Count);

        foreach (Tuple<StatementDefinition, SequencePoint> tuple in doc.Value)
        {
          ushort itemId = 0x3fff;

          // 0x00 = field_def
          // 0x01 = method_def
          // 0x02 = field_ref
          // 0x03 = method_ref

          if (tuple.Item1.Method != null)
          {
            _context.MethodDefinitionTable.TryGetMethodReferenceId(tuple.Item1.Method, out itemId);
            itemId |= 0x4000;
          }
          else if (tuple.Item1.Field != null)
          {
            _context.FieldsTable.TryGetFieldReferenceId(tuple.Item1.Field, false, out itemId);
            itemId |= 0x0000;
          }
          writer.WriteUInt16(itemId);

          SequencePoint seqPt = tuple.Item2;
          writer.WriteUInt16((ushort)seqPt.StartLine);
          writer.WriteUInt16((ushort)seqPt.StartColumn);
          writer.WriteUInt16((ushort)seqPt.EndLine);
          writer.WriteUInt16((ushort)seqPt.EndColumn);
          writer.WriteUInt16(tuple.Item1.StatementStart);
        }
      }

      // emit the methods
      foreach (MethodDefinition method in methods)
      {
        WriteStringReference(writer, method.DeclaringType.FullName);
        WriteStringReference(writer, method.Name);
        writer.WriteUInt16(_context.SignaturesTable.GetOrCreateSignatureId(method));

        byte parametersCount = (byte) method.Parameters.Count;

        // the 'this' pointer is not sent
        writer.WriteByte(parametersCount);

        IEnumerable<Mono.Cecil.Cil.ScopeDebugInformation> scopes = method.DebugInformation.GetScopes();
        byte numScopes = 0;
        foreach (Mono.Cecil.Cil.ScopeDebugInformation scope in scopes)
          numScopes++;

        writer.WriteByte(numScopes);

        if(parametersCount > 0)
          for (byte paramNumber = 0; paramNumber < parametersCount; paramNumber++)
          {
            _context.SignaturesTable.WriteDataType(method.Parameters[paramNumber].ParameterType, writer, false, false, false);
            WriteStringReference(writer, method.Parameters[paramNumber].Name);
            writer.WriteUInt16(paramNumber);
          }

        if (numScopes > 0)
        {
          foreach (Mono.Cecil.Cil.ScopeDebugInformation scope in scopes)
          {
            WriteScope(writer, method, scope);
          }
        }

        // write length of the string table
        foreach (var item in _idsByStrings.OrderBy(item => item.Value).Select(item => item.Key))
        {
          writer.WriteString(item);
        }

      }
    }

    private void WriteScope(CLRBinaryWriter writer, MethodDefinition item, Mono.Cecil.Cil.ScopeDebugInformation scope)
    {
      writer.WriteByte((byte)(scope.HasVariables ? scope.Variables.Count : 0));
      writer.WriteByte((byte)(scope.HasScopes ? scope.Scopes.Count : 0));
      writer.WriteUInt16((ushort)scope.Start.Offset);
      writer.WriteUInt16((ushort)(scope.End.IsEndOfMethod ? item.Body.CodeSize : scope.End.Offset));

      // write local variables
      foreach (Mono.Cecil.Cil.VariableDebugInformation defn in scope.Variables)
      {
        _context.SignaturesTable.WriteDataType(item.Body.Variables[defn.Index].VariableType, writer, false, false, false);
        // get the name from the method
        WriteStringReference(writer, defn.Name);
        writer.WriteUInt16((ushort) defn.Index);
      }

      // write scopes
      foreach (Mono.Cecil.Cil.ScopeDebugInformation childScope in scope.Scopes)
      {
        WriteScope(writer, item, childScope);
      }
    }

    /// <summary>
    /// Writes string reference ID related to passed string value into output stream.
    /// </summary>
    /// <param name="writer">Target binary writer for writing reference ID.</param>
    /// <param name="value">String value for obtaining reference and writing.</param>
    private void WriteStringReference(CLRBinaryWriter writer, string value)
    {
      writer.WriteUInt16(GetOrCreateStringId(value));
    }

    /// <summary>
    /// Gets existing or creates new string reference ID for provided string value.
    /// </summary>
    /// <param name="value">String value for lookup in string literals table.</param>
    /// <returns>String reference ID which can be used for filling metadata and byte code.</returns>
    private ushort GetOrCreateStringId(string value)
    {
      ushort id = GetOrCreateStringId(value);
      return id;
    }

    private Dictionary<string, ushort> _idsByStrings =
        new Dictionary<string, ushort>(StringComparer.Ordinal);
    private ushort _lastAvailableId = 0;
    public ushort GetOrCreateStringId(string value, bool useConstantsTable = true)
    {
      ushort id;
      
      if(!_idsByStrings.TryGetValue(value, out id))
      {
        id = _lastAvailableId;
        _idsByStrings.Add(value, id);
        int length = Encoding.UTF8.GetBytes(value).Length + 1;
        _lastAvailableId += (ushort)(length);
      }
      return id;
    }

  }
}
