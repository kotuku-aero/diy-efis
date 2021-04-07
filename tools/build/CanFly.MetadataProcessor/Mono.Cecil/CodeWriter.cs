//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using Mono.Cecil.Cil;
using System;
using System.Collections.Generic;
using System.Linq;

namespace CanFly.Tools.MetadataProcessor
{

  /// <summary>
  /// Encapsulates logic related for writing correct byte code and calculating stack size.
  /// </summary>
  /// <remarks>
  /// This class initially copy-pasted from Mono.Cecil codebase but changed a lot.
  /// </remarks>
  internal sealed class CodeWriter
  {
    /// <summary>
    /// Original method body information in Mono.Cecil format.
    /// </summary>
    private readonly MethodBody _body;

    /// <summary>
    /// Binary writer for writing byte code in correct endianess.
    /// </summary>
    private readonly CLRBinaryWriter _writer;

    /// <summary>
    /// String literals table (used for obtaining string literal ID).
    /// </summary>
    private readonly StringTable _stringTable;

    /// <summary>
    /// Assembly tables context - contains all tables used for building target assembly.
    /// </summary>
    private readonly TablesContext _context;

    /// <summary>
    /// Creates new instance of <see cref="Mono.Cecil.Cil.CodeWriter"/> object.
    /// </summary>
    /// <param name="method">Original method body in Mono.Cecil format.</param>
    /// <param name="writer">Binary writer for writing byte code in correct endianess.</param>
    /// <param name="stringTable">String references table (for obtaining string ID).</param>
    /// <param name="context">
    /// Assembly tables context - contains all tables used for building target assembly.
    /// </param>
    public CodeWriter(
      MethodDefinition method,
        CLRBinaryWriter writer,
        StringTable stringTable,
        TablesContext context)
    {
      _stringTable = stringTable;

      _body = method.Body;
      _context = context;
      _writer = writer;
    }

    /// <summary>
    /// Writes method body into binary writer originally passed into constructor.
    /// </summary>
    public void WriteMethodBody()
    {
      foreach (var instruction in _body.Instructions)
      {
        WriteOpCode(instruction.OpCode);
        WriteOperand(instruction);
      }

      WriteExceptionsTable();
    }

    /// <summary>
    /// Fixes instructions offsets according .NET nanoFramework operands sizes.
    /// </summary>
    /// <param name="methodDefinition">Target method for fixing offsets</param>
    /// <param name="stringTable">String table for populating strings from method.</param>
    public static IEnumerable<Tuple<uint, uint>> PreProcessMethod(
        MethodDefinition methodDefinition,
        StringTable stringTable)
    {
      if (!methodDefinition.HasBody)
      {
        yield break;
      }

      var offset = 0;
      var offsetChanged = true;
      foreach (var instruction in methodDefinition.Body.Instructions)
      {
        if (offset != 0)
        {
          if (offsetChanged)
          {
            yield return new Tuple<uint, uint>(
                (uint)instruction.Offset, (uint)(instruction.Offset + offset));
            offsetChanged = false;
          }
          instruction.Offset += offset;
        }

        switch (instruction.OpCode.OperandType)
        {
          case OperandType.InlineSwitch:
            var targets = (Instruction[])instruction.Operand;
            offset -= 3; // One bye used instead of Int32
            offset -= 2 * targets.Length; // each target use Int16 instead of Int32
            offsetChanged = true;
            break;
          case OperandType.InlineString:
            stringTable.GetOrCreateStringId((string)instruction.Operand, true);
            offset -= 2;
            offsetChanged = true;
            break;
          case OperandType.InlineMethod:
          case OperandType.InlineField:
          case OperandType.InlineType:
          case OperandType.InlineBrTarget:
            // In full .NET these instructions followed by double word operand
            // but in .NET nanoFramework these instruction's operand are word
            offset -= 2;
            offsetChanged = true;
            break;
        }
      }
    }

    /// <summary>
    /// Calculates method stack size for passed <paramref name="methodBody"/> method.
    /// </summary>
    /// <param name="methodBody">Method body in Mono.Cecil format.</param>
    /// <returns>Maximal evaluated stack size for passed method body.</returns>
    public static byte CalculateStackSize(
        MethodBody methodBody)
    {
      if (methodBody == null)
      {
        return 0;
      }

      IDictionary<int, int> offsets = new Dictionary<int, int>();

      var size = 0;
      var maxSize = 0;
      foreach (var instruction in methodBody.Instructions)
      {
        int correctedStackSize;
        if (offsets.TryGetValue(instruction.Offset, out correctedStackSize))
        {
          size = correctedStackSize;
        }

        switch (instruction.OpCode.Code)
        {
          case Code.Throw:
          case Code.Endfinally:
          case Code.Endfilter:
          case Code.Leave_S:
          case Code.Br_S:
          case Code.Leave:
            size = 0;
            continue;

          case Code.Newobj:
            {
              var method = (MethodReference)instruction.Operand;
              size -= method.Parameters.Count;
            }
            break;

          case Code.Callvirt:
          case Code.Call:
            {
              var method = (MethodReference)instruction.Operand;
              if (method.HasThis)
              {
                --size;
              }
              size -= method.Parameters.Count;
              if (method.ReturnType.FullName != "System.Void")
              {
                ++size;
              }
            }
            break;
        }

        size = CorrectStackDepthByPushes(instruction, size);
        size = CorrectStackDepthByPops(instruction, size);

        if (instruction.OpCode.OperandType == OperandType.ShortInlineBrTarget ||
            instruction.OpCode.OperandType == OperandType.InlineBrTarget)
        {
          int stackSize;
          var target = (Instruction)instruction.Operand;
          offsets.TryGetValue(target.Offset, out stackSize);
          offsets[target.Offset] = Math.Max(stackSize, size);
        }

        maxSize = Math.Max(maxSize, size);
      }

      return (byte)maxSize;
    }

    private static int CorrectStackDepthByPushes(
        Instruction instruction,
        int size)
    {
      switch (instruction.OpCode.StackBehaviourPush)
      {
        case StackBehaviour.Push1:
        case StackBehaviour.Pushi:
        case StackBehaviour.Pushi8:
        case StackBehaviour.Pushr4:
        case StackBehaviour.Pushr8:
        case StackBehaviour.Pushref:
          ++size;
          break;
        case StackBehaviour.Push1_push1:
          size += 2;
          break;
      }
      return size;
    }

    private static int CorrectStackDepthByPops(
        Instruction instruction,
        int size)
    {
      switch (instruction.OpCode.StackBehaviourPop)
      {
        case StackBehaviour.Pop1:
        case StackBehaviour.Popi:
        case StackBehaviour.Popref:
          --size;
          break;
        case StackBehaviour.Pop1_pop1:
        case StackBehaviour.Popi_pop1:
        case StackBehaviour.Popi_popi:
        case StackBehaviour.Popi_popi8:
        case StackBehaviour.Popi_popr4:
        case StackBehaviour.Popi_popr8:
        case StackBehaviour.Popref_pop1:
        case StackBehaviour.Popref_popi:
          size -= 2;
          break;
        case StackBehaviour.Popi_popi_popi:
        case StackBehaviour.Popref_popi_popi:
        case StackBehaviour.Popref_popi_popi8:
        case StackBehaviour.Popref_popi_popr4:
        case StackBehaviour.Popref_popi_popr8:
        case StackBehaviour.Popref_popi_popref:
          size -= 3;
          break;
      }
      return size;
    }

    private void WriteExceptionsTable()
    {
      if (!_body.HasExceptionHandlers)
      {
        return;
      }

      foreach (var handler in _body.ExceptionHandlers
          .OrderBy(item => item.HandlerStart.Offset))
      {
        switch (handler.HandlerType)
        {
          case ExceptionHandlerType.Catch:
            _writer.WriteUInt16(0x0000);
            _writer.WriteUInt16(GetTypeReferenceId(handler.CatchType, 0x8000));
            break;
          case ExceptionHandlerType.Fault:
            _writer.WriteUInt16(0x0001);
            _writer.WriteUInt16(0x0000);
            break;
          case ExceptionHandlerType.Finally:
            _writer.WriteUInt16(0x0002);
            _writer.WriteUInt16(0x0000);
            break;
          case ExceptionHandlerType.Filter:
            _writer.WriteUInt16(0x0003);
            _writer.WriteUInt16((ushort)handler.FilterStart.Offset);
            break;
        }

        _writer.WriteUInt16((ushort)handler.TryStart.Offset);
        _writer.WriteUInt16((ushort)handler.TryEnd.Offset);
        _writer.WriteUInt16((ushort)handler.HandlerStart.Offset);
        if (handler.HandlerEnd == null)
        {
          _writer.WriteUInt16((ushort)_body.Instructions.Last().Offset);
        }
        else
        {
          _writer.WriteUInt16((ushort)handler.HandlerEnd.Offset);
        }
      }

      _writer.WriteByte((byte)_body.ExceptionHandlers.Count);
    }

    private void WriteOpCode(
          OpCode opcode)
    {
      if (opcode.Size == 1)
      {
        _writer.WriteByte(opcode.Op2);
      }
      else
      {
        _writer.WriteByte(opcode.Op1);
        _writer.WriteByte(opcode.Op2);
      }
    }

    private void WriteOperand(
            Instruction instruction)
    {
      var opcode = instruction.OpCode;
      var operandType = opcode.OperandType;

      if (operandType == OperandType.InlineNone)
      {
        return;
      }

      var operand = instruction.Operand;
      if (operand == null)
      {
        throw new ArgumentException();
      }

      switch (operandType)
      {
        case OperandType.InlineSwitch:
          {
            var targets = (Instruction[])operand;
            _writer.WriteByte((byte)targets.Length);
            var diff = instruction.Offset + opcode.Size + 2 * targets.Length + 1;
            foreach (var item in targets)
            {
              _writer.WriteInt16((short)(GetTargetOffset(item) - diff));
            }
            break;
          }
        case OperandType.ShortInlineBrTarget:
          {
            var target = (Instruction)operand;
            _writer.WriteSByte((sbyte)
                    (GetTargetOffset(target) - (instruction.Offset + opcode.Size + 1)));
            break;
          }
        case OperandType.InlineBrTarget:
          {
            var target = (Instruction)operand;
            _writer.WriteInt16((short)
                (GetTargetOffset(target) - (instruction.Offset + opcode.Size + 2)));
            break;
          }
        case OperandType.ShortInlineVar:
          _writer.WriteByte((byte)GetVariableIndex((VariableDefinition)operand));
          break;
        case OperandType.ShortInlineArg:
          _writer.WriteByte((byte)GetParameterIndex((ParameterDefinition)operand));
          break;
        case OperandType.InlineVar:
          _writer.WriteInt16((short)GetVariableIndex((VariableDefinition)operand));
          break;
        case OperandType.InlineArg:
          _writer.WriteInt16((short)GetParameterIndex((ParameterDefinition)operand));
          break;
        case OperandType.InlineSig:
          // TODO: implement this properly after finding when such code is generated
          //WriteMetadataToken (GetStandAloneSignature ((CallSite) operand));
          break;
        case OperandType.ShortInlineI:
          if (opcode == OpCodes.Ldc_I4_S)
          {
            _writer.WriteSByte((sbyte)operand);
          }
          else
          {
            _writer.WriteByte((byte)operand);
          }
          break;
        case OperandType.InlineI:
          _writer.WriteInt32((int)operand);
          break;
        case OperandType.InlineI8:
          _writer.WriteInt64((long)operand);
          break;
        case OperandType.ShortInlineR:
          _writer.WriteSingle((float)operand);
          break;
        case OperandType.InlineR:
          _writer.WriteDouble((double)operand);
          break;
        case OperandType.InlineString:
          var stringReferenceId = _stringTable.GetOrCreateStringId((string)operand, true);
          _writer.WriteUInt16(stringReferenceId);
          break;
        case OperandType.InlineMethod:
          _writer.WriteUInt16(_context.GetMethodReferenceId((MethodReference)operand));
          break;
        case OperandType.InlineType:
          _writer.WriteUInt16(GetTypeReferenceId((TypeReference)operand));
          break;
        case OperandType.InlineField:
          _writer.WriteUInt16(GetFieldReferenceId((FieldReference)operand));
          break;
        case OperandType.InlineTok:
          _writer.WriteUInt32(GetMetadataToken((IMetadataTokenProvider)operand));
          break;
        default:
          throw new ArgumentException();
      }
    }

    private uint GetMetadataToken(
        IMetadataTokenProvider token)
    {
      ushort referenceId;
      switch (token.MetadataToken.TokenType)
      {
        case TokenType.TypeRef:
          _context.TypeReferencesTable.TryGetTypeReferenceId((TypeReference)token, out referenceId);
          return (uint)0x01000000 | referenceId;
        case TokenType.TypeDef:
          _context.TypeDefinitionTable.TryGetTypeReferenceId((TypeDefinition)token, out referenceId);
          return (uint)0x04000000 | referenceId;
        case TokenType.TypeSpec:
          _context.TypeSpecificationsTable.TryGetTypeReferenceId((TypeReference)token, out referenceId);
          return (uint)0x08000000 | referenceId;
        case TokenType.Field:
          _context.FieldsTable.TryGetFieldReferenceId((FieldDefinition)token, false, out referenceId);
          return (uint)0x05000000 | referenceId;
      }
      return 0U;
    }

    private ushort GetFieldReferenceId(
        FieldReference fieldReference)
    {
      ushort referenceId;
      if (_context.FieldReferencesTable.TryGetFieldReferenceId(fieldReference, out referenceId))
      {
        referenceId |= 0x8000; // External field reference
      }
      else
      {
        _context.FieldsTable.TryGetFieldReferenceId(fieldReference.Resolve(), false, out referenceId);
      }
      return referenceId;
    }

    private ushort GetTypeReferenceId(
        TypeReference typeReference,
        ushort typeReferenceMask = 0x4000)
    {
      ushort referenceId;

      if (typeReference is TypeSpecification)
      {
        referenceId = _context.TypeSpecificationsTable.GetOrCreateTypeSpecificationId(typeReference);

        return (ushort)(0x8000 | referenceId);
      }
      else if (_context.TypeReferencesTable.TryGetTypeReferenceId(typeReference, out referenceId))
      {
        referenceId |= typeReferenceMask; // External type reference
      }
      else
      {
        if (!_context.TypeDefinitionTable.TryGetTypeReferenceId(typeReference.Resolve(), out referenceId))
        {
          return 0x8000;
        }

      }
      return referenceId;
    }

    private int GetTargetOffset(
        Instruction instruction)
    {
      if (instruction == null)
      {
        var last = _body.Instructions[_body.Instructions.Count - 1];
        return last.Offset + last.GetSize();
      }

      return instruction.Offset;
    }

    private static int GetVariableIndex(
            VariableDefinition variable)
    {
      return variable.Index;
    }

    private int GetParameterIndex(
            ParameterDefinition parameter)
    {
      if (_body.Method.HasThis)
      {
        if (parameter == _body.ThisParameter)
          return 0;

        return parameter.Index + 1;
      }

      return parameter.Index;
    }
  }
}