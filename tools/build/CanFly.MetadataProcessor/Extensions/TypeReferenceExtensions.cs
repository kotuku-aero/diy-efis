//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System.Text;

namespace CanFly.Tools.MetadataProcessor.Core.Extensions
{
  internal static class TypeReferenceExtensions
  {
    public static bool IsToInclude(this TypeReference value)
    {
      return !TablesContext.IgnoringAttributes.Contains(value.FullName);
    }

    public static string TypeSignatureAsString(this TypeReference type)
    {
      if (type.MetadataType == MetadataType.IntPtr)
      {
        return "I";
      }

      if (type.MetadataType == MetadataType.UIntPtr)
      {
        return "U";
      }

      nanoCLR_DataType dataType;
      if (SignaturesTable.PrimitiveTypes.TryGetValue(type.FullName, out dataType))
      {
        switch (dataType)
        {
          case nanoCLR_DataType.DATATYPE_VOID:
          case nanoCLR_DataType.DATATYPE_BOOLEAN:
          case nanoCLR_DataType.DATATYPE_CHAR:
          case nanoCLR_DataType.DATATYPE_I1:
          case nanoCLR_DataType.DATATYPE_U1:
          case nanoCLR_DataType.DATATYPE_I2:
          case nanoCLR_DataType.DATATYPE_U2:
          case nanoCLR_DataType.DATATYPE_I4:
          case nanoCLR_DataType.DATATYPE_U4:
          case nanoCLR_DataType.DATATYPE_I8:
          case nanoCLR_DataType.DATATYPE_U8:
          case nanoCLR_DataType.DATATYPE_R4:
          case nanoCLR_DataType.DATATYPE_COLOR:
          case nanoCLR_DataType.DATATYPE_EULER:
          case nanoCLR_DataType.DATATYPE_EXTENT:
          case nanoCLR_DataType.DATATYPE_FONT:
          case nanoCLR_DataType.DATATYPE_PEN:
          case nanoCLR_DataType.DATATYPE_POINT:
          case nanoCLR_DataType.DATATYPE_POS:
          case nanoCLR_DataType.DATATYPE_RECT:
          case nanoCLR_DataType.DATATYPE_BYREF:
          case nanoCLR_DataType.DATATYPE_OBJECT:
            return dataType.ToString().Replace("DATATYPE_", "");
        }
      }

      if (type.MetadataType == MetadataType.Class)
      {
        StringBuilder classSig = new StringBuilder("CLASS [");
        classSig.Append(type.MetadataToken.ToInt32().ToString("x8"));
        classSig.Append("]");

        return classSig.ToString();
      }

      if (type.MetadataType == MetadataType.ValueType)
      {
        StringBuilder valueTypeSig = new StringBuilder("VALUETYPE [");
        valueTypeSig.Append(type.MetadataToken.ToInt32().ToString("x8"));
        valueTypeSig.Append("]");

        return valueTypeSig.ToString();
      }

      if (type.IsArray)
      {
        StringBuilder arraySig = new StringBuilder("SZARRAY ");
        arraySig.Append(type.GetElementType().TypeSignatureAsString());

        return arraySig.ToString();
      }

      if (type.IsByReference)
      {
        StringBuilder byrefSig = new StringBuilder("BYREF ");
        byrefSig.Append(type.GetElementType().TypeSignatureAsString());

        return byrefSig.ToString();
      }

      if (type.IsGenericParameter ||
          type.IsGenericInstance)
      {
        return $"!!{type.Name}";
      }

      return "";
    }

    public static string ToNativeTypeAsString(this TypeReference type)
    {
      nanoCLR_DataType dataType;
      if (SignaturesTable.PrimitiveTypes.TryGetValue(type.FullName, out dataType))
      {
        switch (dataType)
        {
          case nanoCLR_DataType.DATATYPE_VOID:
            return "void";
          case nanoCLR_DataType.DATATYPE_BOOLEAN:
            return "bool";
          case nanoCLR_DataType.DATATYPE_CHAR:
            return "char";
          case nanoCLR_DataType.DATATYPE_I1:
            return "int8_t";
          case nanoCLR_DataType.DATATYPE_U1:
            return "uint8_t";
          case nanoCLR_DataType.DATATYPE_I2:
            return "int16_t";
          case nanoCLR_DataType.DATATYPE_U2:
            return "uint16_t";
          case nanoCLR_DataType.DATATYPE_I4:
            return "signed int";
          case nanoCLR_DataType.DATATYPE_U4:
            return "unsigned int";
          case nanoCLR_DataType.DATATYPE_I8:
            return "int64_t";
          case nanoCLR_DataType.DATATYPE_U8:
            return "uint64_t";
          case nanoCLR_DataType.DATATYPE_R4:
            return "float";
          case nanoCLR_DataType.DATATYPE_BYREF:
            return "";
          case nanoCLR_DataType.DATATYPE_R8:
            return "double";
          case nanoCLR_DataType.DATATYPE_STRING:
            return "const char*";
          case nanoCLR_DataType.DATATYPE_COLOR:
            return "color_t";
          case nanoCLR_DataType.DATATYPE_EULER:
            return "xyz_t";
          case nanoCLR_DataType.DATATYPE_EXTENT:
            return "extent_t";
          case nanoCLR_DataType.DATATYPE_FONT:
            return "font_t";
          case nanoCLR_DataType.DATATYPE_PEN:
            return "pen_t";
          case nanoCLR_DataType.DATATYPE_POINT:
            return "point_t";
          case nanoCLR_DataType.DATATYPE_POS:
            return "lla_t";
          case nanoCLR_DataType.DATATYPE_RECT:
            return "rect_t";
          default:
            return "UNSUPPORTED";
        }
      }

      if (type.MetadataType == MetadataType.Class)
      {
        return "UNSUPPORTED";
      }

      if (type.MetadataType == MetadataType.ValueType)
      {
        return "UNSUPPORTED";
      }

      if (type.IsArray)
      {
        StringBuilder arraySig = new StringBuilder("CLR_RT_TypedArray_");
        arraySig.Append(type.GetElementType().ToCLRTypeAsString());

        return arraySig.ToString();
      }

      if (type.IsGenericParameter)
      {
        return "UNSUPPORTED";
      }
      return "";
    }

    public static string ToCLRTypeAsString(this TypeReference type)
    {
      nanoCLR_DataType dataType;
      if (SignaturesTable.PrimitiveTypes.TryGetValue(type.FullName, out dataType))
      {
        switch (dataType)
        {
          case nanoCLR_DataType.DATATYPE_VOID:
            return "void";
          case nanoCLR_DataType.DATATYPE_BOOLEAN:
            return "bool";
          case nanoCLR_DataType.DATATYPE_CHAR:
            return "CHAR";
          case nanoCLR_DataType.DATATYPE_I1:
            return "INT8";
          case nanoCLR_DataType.DATATYPE_U1:
            return "UINT8";
          case nanoCLR_DataType.DATATYPE_I2:
            return "INT16";
          case nanoCLR_DataType.DATATYPE_U2:
            return "UINT16";
          case nanoCLR_DataType.DATATYPE_I4:
            return "INT32";
          case nanoCLR_DataType.DATATYPE_U4:
            return "UINT32";
          case nanoCLR_DataType.DATATYPE_I8:
            return "INT64";
          case nanoCLR_DataType.DATATYPE_U8:
            return "UINT64";
          case nanoCLR_DataType.DATATYPE_R4:
            return "float";
          case nanoCLR_DataType.DATATYPE_BYREF:
            return "NONE";
          case nanoCLR_DataType.DATATYPE_STRING:
            return "LPCSTR";
          default:
            return "UNSUPPORTED";
        }
      }

      if (type.MetadataType == MetadataType.Class)
      {
        return "UNSUPPORTED";
      }

      if (type.MetadataType == MetadataType.ValueType)
      {
        return "UNSUPPORTED";
      }

      if (type.IsArray)
      {
        StringBuilder arraySig = new StringBuilder();
        arraySig.Append(type.GetElementType().ToCLRTypeAsString());
        arraySig.Append("_ARRAY");

        return arraySig.ToString();
      }

      if (type.IsGenericParameter)
      {
        return "UNSUPPORTED";
      }

      return "";
    }

    public static nanoSerializationType ToSerializationType(this TypeReference value)
    {
      nanoCLR_DataType dataType;
      if (SignaturesTable.PrimitiveTypes.TryGetValue(value.FullName, out dataType))
      {
        switch (dataType)
        {
          case nanoCLR_DataType.DATATYPE_BOOLEAN:
            return nanoSerializationType.ELEMENT_TYPE_BOOLEAN;
          case nanoCLR_DataType.DATATYPE_I1:
            return nanoSerializationType.ELEMENT_TYPE_I1;
          case nanoCLR_DataType.DATATYPE_U1:
            return nanoSerializationType.ELEMENT_TYPE_U1;
          case nanoCLR_DataType.DATATYPE_I2:
            return nanoSerializationType.ELEMENT_TYPE_I2;
          case nanoCLR_DataType.DATATYPE_U2:
            return nanoSerializationType.ELEMENT_TYPE_U2;
          case nanoCLR_DataType.DATATYPE_I4:
            return nanoSerializationType.ELEMENT_TYPE_I4;
          case nanoCLR_DataType.DATATYPE_U4:
            return nanoSerializationType.ELEMENT_TYPE_U4;
          case nanoCLR_DataType.DATATYPE_I8:
            return nanoSerializationType.ELEMENT_TYPE_I8;
          case nanoCLR_DataType.DATATYPE_U8:
            return nanoSerializationType.ELEMENT_TYPE_U8;
          case nanoCLR_DataType.DATATYPE_R4:
            return nanoSerializationType.ELEMENT_TYPE_R4;
          case nanoCLR_DataType.DATATYPE_R8:
            return nanoSerializationType.ELEMENT_TYPE_R8;
          case nanoCLR_DataType.DATATYPE_CHAR:
            return nanoSerializationType.ELEMENT_TYPE_CHAR;
          case nanoCLR_DataType.DATATYPE_STRING:
            return nanoSerializationType.ELEMENT_TYPE_STRING;
          default:
            return 0;
        }
      }

      return 0;
    }

  }
}
