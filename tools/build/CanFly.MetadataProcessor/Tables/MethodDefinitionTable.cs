using Mono.Cecil;
using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace CanFly.Tools.MetadataProcessor
{
  /// <summary>
  /// Encapsulates logic for storing methods definitions list and writing
  /// this collected list into target assembly in .NET nanoFramework format.
  /// </summary>
  public sealed class MethodDefinitionTable : ReferenceTableBase<MethodDefinition>
  {
    /// <summary>
    /// Helper class for comparing two instances of <see cref="MethodDefinition"/> objects
    /// using <see cref="MethodDefinition.FullName"/> property as unique key for comparison.
    /// </summary>
    private sealed class MethodDefinitionComparer : IEqualityComparer<MethodDefinition>
    {
      /// <inheritdoc/>
      public bool Equals(MethodDefinition lhs, MethodDefinition rhs)
      {
        return string.Equals(lhs.FullName, rhs.FullName, StringComparison.Ordinal);
      }

      /// <inheritdoc/>
      public int GetHashCode(MethodDefinition that)
      {
        return that.FullName.GetHashCode();
      }
    }

    /// <summary>
    /// Creates new instance of <see cref="MethodDefinitionTable"/> object.
    /// </summary>
    /// <param name="items">List of methods definitions in Mono.Cecil format.</param>
    /// <param name="context">
    /// Assembly tables context - contains all tables used for building target assembly.
    /// </param>
    public MethodDefinitionTable(
        IEnumerable<MethodDefinition> items,
        TablesContext context)
        : base(items, new MethodDefinitionComparer(), context)
    {
    }

    /// <summary>
    /// Gets method reference identifier (if method is defined inside target assembly).
    /// </summary>
    /// <param name="methodDefinition">Method definition in Mono.Cecil format.</param>
    /// <param name="referenceId">Method definition reference identifier for filling.</param>
    /// <returns>Returns <c>true</c> if item found, otherwise returns <c>false</c>.</returns>
    public bool TryGetMethodReferenceId(
        MethodDefinition methodDefinition,
        out ushort referenceId)
    {
      return TryGetIdByValue(methodDefinition, out referenceId);
    }

    /*
 typedef struct _CLR_RECORD_METHODDEF
  {
#define MD_Scope_Mask 0x00000007
#define MD_Scope_PrivateScope 0x00000000    // Member not referenceable.
#define MD_Scope_Private 0x00000001    // Accessible only by the parent type.
#define MD_Scope_FamANDAssem 0x00000002    // Accessible by sub-types only in this Assembly.
#define MD_Scope_Assem 0x00000003    // Accessibly by anyone in the Assembly.
#define MD_Scope_Family 0x00000004    // Accessible only by type and sub-types.
#define MD_Scope_FamORAssem 0x00000005    // Accessibly by sub-types anywhere, plus anyone in assembly.
#define MD_Scope_Public 0x00000006    // Accessibly by anyone who has visibility to this scope.

#define MD_Static 0x00000010    // Defined on type, else per instance.
#define MD_Final 0x00000020    // Method may not be overridden.
#define MD_Virtual 0x00000040    // Method virtual.
#define MD_HideBySig 0x00000080    // Method hides by name+sig, else just by name.

#define MD_VtableLayoutMask 0x00000100//
#define MD_ReuseSlot 0x00000000    // The default.
#define MD_NewSlot 0x00000100    // Method always gets a new slot in the vtable.
#define MD_Abstract 0x00000200    // Method does not provide an implementation.
#define MD_SpecialName 0x00000400    // Method is special.  Name describes how.
#define MD_NativeProfiled 0x00000800

#define MD_Constructor 0x00001000
#define MD_StaticConstructor 0x00002000
#define MD_Finalizer 0x00004000

#define MD_DelegateConstructor 0x00010000
#define MD_DelegateInvoke 0x00020000
#define MD_DelegateBeginInvoke 0x00040000
#define MD_DelegateEndInvoke 0x00080000

#define MD_EntryPoint 0x08000000
#define MD_HasExceptionHandlers 0x40000000
#define MD_HasAttributes 0x80000000

  uint16_t name; // TBL_Strings
  uint16_t RVA;
  //
  uint32_t flags;
  //
  uint8_t retVal;       // return type
  uint8_t numArgs;      // number of arguments
  uint8_t numLocals;    // number of locals
  uint16_t sig;           // method signature
  } CLR_RECORD_METHODDEF;
    */

    protected override void WriteSingleItem(
        CLRBinaryWriter writer,
        MethodDefinition item)
    {
      if (!_context.MinimizeComplete)
      {
        return;
      }

      WriteStringReference(writer, item.Name);
      writer.WriteUInt16(_context.ByteCodeTable.GetMethodRva(item));

      writer.WriteUInt32(GetFlags(item));

      _context.SignaturesTable.WriteDataType(item.ReturnType, writer, false, false, false);

      byte parametersCount = (byte)item.Parameters.Count;
      writer.WriteByte(item.IsStatic ? parametersCount : (byte)(parametersCount +1));

      // add a variable for the return type
      writer.WriteByte((byte)(item.Body == null ? 0 : item.Body.Variables.Count));

      // write the method signature
      writer.WriteUInt16(_context.SignaturesTable.GetOrCreateSignatureId(item));
    }

    public static uint GetFlags(MethodDefinition method)
    {
      const uint MD_Scope_Private = 0x00000001; // Accessible only by the parent type.
      const uint MD_Scope_FamANDAssem = 0x00000002; // Accessible by sub-types only in this Assembly.
      const uint MD_Scope_Assem = 0x00000003; // Accessibly by anyone in the Assembly.
      const uint MD_Scope_Family = 0x00000004; // Accessible only by type and sub-types.
      const uint MD_Scope_FamORAssem = 0x00000005; // Accessibly by sub-types anywhere, plus anyone in assembly.
      const uint MD_Scope_Public = 0x00000006; // Accessibly by anyone who has visibility to this scope.

      const uint MD_Static = 0x00000010; // Defined on type, else per instance.
      const uint MD_Final = 0x00000020; // Method may not be overridden.
      const uint MD_Virtual = 0x00000040; // Method virtual.
      const uint MD_HideBySig = 0x00000080; // Method hides by name+sig, else just by name.

      const uint MD_ReuseSlot = 0x00000000; // The default.
      const uint MD_NewSlot = 0x00000100; // Method always gets a new slot in the vtable.
      const uint MD_Abstract = 0x00000200; // Method does not provide an implementation.
      const uint MD_SpecialName = 0x00000400; // Method is special.  Name describes how.
      const uint MD_NativeProfiled = 0x00000800;

      const uint MD_Constructor = 0x00001000;
      const uint MD_StaticConstructor = 0x00002000;
      const uint MD_Finalizer = 0x00004000;

      const uint MD_DelegateConstructor = 0x00010000;
      const uint MD_DelegateInvoke = 0x00020000;
      const uint MD_DelegateBeginInvoke = 0x00040000;
      const uint MD_DelegateEndInvoke = 0x00080000;

      const uint MD_ContainsGenericParameter = 0x00100000;
      const uint MD_HasGenericParameter = 0x00200000;

      const uint MD_Synchronized = 0x01000000;
      const uint MD_GloballySynchronized = 0x02000000;
      const uint MD_Patched = 0x04000000;
      const uint MD_EntryPoint = 0x08000000;

      const uint MD_RequireSecObject = 0x10000000; // Method calls another method containing security code.
      const uint MD_HasSecurity = 0x20000000; // Method has security associate with it.
      const uint MD_HasExceptionHandlers = 0x40000000;
      const uint MD_HasAttributes = 0x80000000;

      uint flag = 0;
      if (method.IsPrivate)
      {
        flag = MD_Scope_Private;
      }
      else if (method.IsFamilyAndAssembly)
      {
        flag = MD_Scope_FamANDAssem;
      }
      else if (method.IsFamilyOrAssembly)
      {
        flag = MD_Scope_FamORAssem;
      }
      else if (method.IsAssembly)
      {
        flag = MD_Scope_Assem;
      }
      else if (method.IsFamily)
      {
        flag = MD_Scope_Family;
      }
      else if (method.IsPublic)
      {
        flag = MD_Scope_Public;
      }

      if (method.IsStatic)
      {
        flag |= MD_Static;
      }
      if (method.IsFinal)
      {
        flag |= MD_Final;
      }
      if (method.IsVirtual)
      {
        flag |= MD_Virtual;
      }
      if (method.IsHideBySig)
      {
        flag |= MD_HideBySig;
      }

      if (method.IsReuseSlot)
      {
        flag |= MD_ReuseSlot;
      }
      if (method.IsNewSlot)
      {
        flag |= MD_NewSlot;
      }
      if (method.IsAbstract)
      {
        flag |= MD_Abstract;
      }
      if (method.IsSpecialName)
      {
        flag |= MD_SpecialName;
      }

      if (method.IsNative)
      {
        // can't find anything relevant to do with this...
        //    flag |= MD_NativeProfiled; // ???
      }

      if (method.IsConstructor)
      {
        flag |= (method.IsStatic ? MD_StaticConstructor : MD_Constructor);
      }

      if (method.IsSynchronized)
      {
        flag |= MD_Synchronized;
      }
      if (method.HasCustomAttributes)
      {
        // TODO
        // parse special attributes: NativeProfiler, GloballySynchronized
        flag |= MD_HasAttributes;
      }

      if (method.Module != null &&
          method == method.Module.EntryPoint)
      {
        flag |= MD_EntryPoint;
      }

      if (method.HasBody && method.Body.HasExceptionHandlers)
      {
        flag |= MD_HasExceptionHandlers;
      }

      if (method.DeclaringType != null)
      {
        var baseType = method.DeclaringType.BaseType;
        if (baseType != null && baseType.FullName == "System.MulticastDelegate")
        {
          if (method.IsConstructor)
          {
            flag |= MD_DelegateConstructor;
          }
          else if (method.Name == "Invoke")
          {
            flag |= MD_DelegateInvoke;
          }
          else if (method.Name == "BeginInvoke")
          {
            flag |= MD_DelegateBeginInvoke;
          }
          else if (method.Name == "EndInvoke")
          {
            flag |= MD_DelegateEndInvoke;
          }
        }
      }

      if (method.ContainsGenericParameter)
      {
        flag |= MD_ContainsGenericParameter;
      }

      if (method.HasGenericParameters)
      {
        flag |= MD_HasGenericParameter;
      }

      return flag;
    }
  }
}
