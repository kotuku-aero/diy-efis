using System;
using System.Collections.Generic;
using System.Reflection;

using CanFly.CommandLine.Infrastructure;

namespace CanFly.CommandLine
{
  /// <summary>
  /// Models a list of command line arguments that are not options.
  /// Must be applied to a field compatible with an <see cref="System.Collections.Generic.IList&lt;T&gt;"/> interface
  /// of <see cref="System.String"/> instances.
  /// </summary>
  /// <remarks>To map individual values use instead <see cref="CanFly.CommandLine.ValueOptionAttribute"/>.</remarks>
  [AttributeUsage(AttributeTargets.Property, AllowMultiple = false, Inherited = true)]
  public sealed class ValueListAttribute : Attribute
  {
    private readonly Type _concreteType;

    /// <summary>
    /// Initializes a new instance of the <see cref="CanFly.CommandLine.ValueListAttribute"/> class.
    /// </summary>
    /// <param name="concreteType">A type that implements <see cref="System.Collections.Generic.IList&lt;T&gt;"/>.</param>
    /// <exception cref="System.ArgumentNullException">Thrown if <paramref name="concreteType"/> is null.</exception>
    public ValueListAttribute(Type concreteType)
      : this()
    {
      if (concreteType == null)
      {
        throw new ArgumentNullException("concreteType");
      }

      if (!typeof(IList<string>).IsAssignableFrom(concreteType))
      {
        throw new ParserException(SR.CommandLineParserException_IncompatibleTypes);
      }

      _concreteType = concreteType;
    }

    private ValueListAttribute()
    {
      MaximumElements = -1;
    }

    /// <summary>
    /// Gets or sets the maximum element allow for the list managed by <see cref="CanFly.CommandLine.ValueListAttribute"/> type.
    /// If lesser than 0, no upper bound is fixed.
    /// If equal to 0, no elements are allowed.
    /// </summary>
    public int MaximumElements { get; set; }

    /// <summary>
    /// Gets the concrete type specified during initialization.
    /// </summary>
    public Type ConcreteType
    {
      get { return _concreteType; }
    }

    internal static IList<string> GetReference(object target)
    {
      Type concreteType;
      var property = GetProperty(target, out concreteType);
      if (property == null || concreteType == null)
      {
        return null;
      }

      property.SetValue(target, Activator.CreateInstance(concreteType), null);

      return (IList<string>)property.GetValue(target, null);
    }

    internal static ValueListAttribute GetAttribute(object target)
    {
      var list = ReflectionHelper.RetrievePropertyList<ValueListAttribute>(target);
      if (list == null || list.Count == 0)
      {
        return null;
      }

      if (list.Count > 1)
      {
        throw new InvalidOperationException();
      }

      var pairZero = list[0];
      return pairZero.Right;
    }

    private static PropertyInfo GetProperty(object target, out Type concreteType)
    {
      concreteType = null;
      var list = ReflectionHelper.RetrievePropertyList<ValueListAttribute>(target);
      if (list == null || list.Count == 0)
      {
        return null;
      }

      if (list.Count > 1)
      {
        throw new InvalidOperationException();
      }

      var pairZero = list[0];
      concreteType = pairZero.Right.ConcreteType;
      return pairZero.Left;
    }
  }
}