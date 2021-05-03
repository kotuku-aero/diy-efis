//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Text.RegularExpressions
{

  /// <summary>
  /// Represents the results from a single subexpression capture. 
  /// System.Text.RegularExpressions. 
  /// Capture respresents one substring to a single successful catpture
  /// </summary>
  [Serializable]
  public class Capture
  {
    internal int _index;
    internal int _length;
    internal string _text;

    /// <summary>
    /// Gets the index.
    /// </summary>
    public int Index
    {
      get { return _index; }
    }

    /// <summary>
    /// Gets the length.
    /// </summary>
    public int Length
    {
      get { return _length; }
    }

    /// <summary>
    /// Gets the value.
    /// </summary>
    public string Value
    {
      get { return _text.Substring(_index, _length); }
    }

    ///<inheritdoc/>
    public override string ToString()
    {
      return Value;
    }

    internal Capture(string text, int i, int l)
    {
      _text = text;
      _index = i;
      _length = l;
    }

    internal string GetLeftSubstring()
    {
      return _text.Substring(0, _index);
    }

    internal string GetOriginalString()
    {
      return _text;
    }

    internal string GetRightSubstring()
    {
      return _text.Substring(_index + _length, (_text.Length - _index) - _length);
    }
  }
}
