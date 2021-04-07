using Mono.Cecil;
using Mono.Cecil.Cil;
using System;
using System.Collections.Generic;
using System.Linq;

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
      public ushort StatementEnd { get; set; }
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

                source.FilenameId = _context.StringTable.GetOrCreateStringId(fileName, false);
                source.PathId = _context.StringTable.GetOrCreateStringId(document.Url, false);
              }
            }
          }
        }
      }
    }

    public void Write(CLRBinaryWriter writer)
    {
      if (!_context.MinimizeComplete)
      {
        return;
      }

      foreach (ModuleDefinition definition in _modules)
      {
        foreach (TypeDefinition type in definition.GetTypes())
        {
          foreach (MethodDefinition method in type.Methods)
          {
            // get the offsets table.
            List<Tuple<uint, uint>> offsetsTable = new List<Tuple<uint, uint>>();

            // adjust the table to match its use
            uint cumulative = 0;
            foreach(Tuple<uint, uint> tuple in _context.TypeDefinitionTable.GetByteCodeOffsets(method.MetadataToken.ToUInt32()))
            {
              uint diff = tuple.Item1 - tuple.Item2 - cumulative;
              uint index = tuple.Item1;

              cumulative += diff;
              offsetsTable.Add(new Tuple<uint, uint>(index, diff));
            }

            IDictionary<Instruction, SequencePoint> mapping = method.DebugInformation.GetSequencePointMapping();

            foreach (KeyValuePair<Instruction, SequencePoint> seqPt in mapping)
            {
              Instruction instruction = seqPt.Key;

              StatementDefinition statement = new StatementDefinition();
              statement.Method = method;

              // TODO: I think the relocations are cumulative so there needs
              // to be an adjustment counter.

              // instruction start
              // find the mapping
              statement.StatementStart = (ushort)instruction.Offset;

              foreach (Tuple<uint, uint> relocationAddr in offsetsTable)
              {
                if (relocationAddr.Item1 > (uint)instruction.Offset)
                  break;

                statement.StatementStart -= (ushort)relocationAddr.Item2;
              }

              if (instruction.Next != null)
              {
                statement.StatementEnd = (ushort) instruction.Next.Offset;
                foreach (Tuple<uint, uint> relocationAddr in offsetsTable)
                {
                  if (relocationAddr.Item1 > (uint)instruction.Next.Offset)
                    break;

                  statement.StatementEnd -= (ushort)relocationAddr.Item2;
                }
              }

              List<Tuple<StatementDefinition, SequencePoint>> pts = _sources[new Source(seqPt.Value.Document)];

              pts.Add(new Tuple<StatementDefinition, SequencePoint>(statement, seqPt.Value));
            }
          }
        }
      }

      /* emit the table.  This is the format: 

  typedef struct _CLR_RECORD_SOURCE {
    uint16_t path;                    // index to string table for the path to the directory
                                      // relative to the project root.  All filenames with
                                      // the same path share this
    uint16_t fileName;                // index to string table for name of the file
    uint16_t numSymbols;              // number of symbols in the source
    CLR_RECORD_SYMBOL symbols[0];     // variable number of symbols
    } CLR_RECORD_SOURCE;

  typedef struct _CLR_RECORD_SYMBOL {
    uint16_t symbolType;              // either a fieldDef if 0x8000 otherwise a methodDef
    uint16_t sourceLine;              // start line of the symbol
    uint16_t sourceColumn;            // start column of symbol
    uint16_t endLine;                 // end line of symbol
    uint16_t endColumn;               // end column of symbol
    uint16_t statementStart;          // if a methodDef then start IP of statement
    uint16_t statementEnd;            // end of statement
    } CLR_RECORD_SYMBOL;
       */
      foreach(KeyValuePair<Source, List<Tuple<StatementDefinition, SequencePoint>>> doc in _sources)
      {
        if(doc.Value.Count > 65535)
        {
          Console.WriteLine($"Cannot process a source '{doc.Key.Document.Url}' that has more than 65535 sequence points.  Will not have debug information.");
          continue;
        }

        writer.WriteUInt16(doc.Key.PathId);
        writer.WriteUInt16(doc.Key.FilenameId);
        writer.WriteUInt16((ushort) doc.Value.Count);

        foreach(Tuple<StatementDefinition, SequencePoint> tuple in doc.Value)
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
          writer.WriteUInt16((ushort) seqPt.StartLine);
          writer.WriteUInt16((ushort)seqPt.StartColumn);
          writer.WriteUInt16((ushort)seqPt.EndLine);
          writer.WriteUInt16((ushort)seqPt.EndColumn);
          writer.WriteUInt16(tuple.Item1.StatementStart);
          writer.WriteUInt16(tuple.Item1.StatementEnd);
        }
      }
    }
  }
}
