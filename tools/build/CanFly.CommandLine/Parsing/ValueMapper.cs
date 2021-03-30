using System.Collections.Generic;
using System.Globalization;
using System.Reflection;

using CanFly.CommandLine.Infrastructure;

namespace CanFly.CommandLine.Parsing
{
  /// <summary>
  /// Maps unnamed options to property using <see cref="CanFly.CommandLine.ValueOptionAttribute"/> and <see cref="CanFly.CommandLine.ValueListAttribute"/>.
  /// </summary>
  internal sealed class ValueMapper
  {
    private readonly CultureInfo _parsingCulture;
    private readonly object _target;
    private IList<string> _valueList;
    private ValueListAttribute _valueListAttribute;
    private IList<Pair<PropertyInfo, ValueOptionAttribute>> _valueOptionAttributeList;
    private int _valueOptionIndex;

    public ValueMapper(object target, CultureInfo parsingCulture)
    {
      _target = target;
      _parsingCulture = parsingCulture;
      InitializeValueList();
      InitializeValueOption();
    }

    public bool CanReceiveValues
    {
      get { return IsValueListDefined || IsValueOptionDefined; }
    }

    private bool IsValueListDefined
    {
      get { return _valueListAttribute != null; }
    }

    private bool IsValueOptionDefined
    {
      get { return _valueOptionAttributeList.Count > 0; }
    }

    public bool MapValueItem(string item)
    {
      if (IsValueOptionDefined &&
          _valueOptionIndex < _valueOptionAttributeList.Count)
      {
        var valueOption = _valueOptionAttributeList[_valueOptionIndex++];

        var propertyWriter = new PropertyWriter(valueOption.Left, _parsingCulture);

        return ReflectionHelper.IsNullableType(propertyWriter.Property.PropertyType) ?
            propertyWriter.WriteNullable(item, _target) :
            propertyWriter.WriteScalar(item, _target);
      }

      return IsValueListDefined && AddValueItem(item);
    }

    private bool AddValueItem(string item)
    {
      if (_valueListAttribute.MaximumElements == 0 ||
          _valueList.Count == _valueListAttribute.MaximumElements)
      {
        return false;
      }

      _valueList.Add(item);
      return true;
    }

    private void InitializeValueList()
    {
      _valueListAttribute = ValueListAttribute.GetAttribute(_target);
      if (IsValueListDefined)
      {
        _valueList = ValueListAttribute.GetReference(_target);
      }
    }

    private static int CompareProps(Pair<PropertyInfo, ValueOptionAttribute> left, Pair<PropertyInfo, ValueOptionAttribute> right)
    {
      return left.Right.Index.CompareTo(right.Right.Index);
    }

    private void InitializeValueOption()
    {
      IList<Pair<PropertyInfo, ValueOptionAttribute>> list = ReflectionHelper.RetrievePropertyList<ValueOptionAttribute>(_target);

      bool allAreDefault = true;
      foreach (Pair<PropertyInfo, ValueOptionAttribute> value in list)
      {
        if (value.Right.Index != 0)
        {
          allAreDefault = false;
          break;
        }
      }

      if (allAreDefault)
        _valueOptionAttributeList = list;
      else
      {
        List<Pair<PropertyInfo, ValueOptionAttribute>> props = new List<Pair<PropertyInfo, ValueOptionAttribute>>(list);
        props.Sort(CompareProps);
        _valueOptionAttributeList = props;
      }
    }
  }
}