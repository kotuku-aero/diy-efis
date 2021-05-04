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
using System;

namespace CanFly.Proton
{
  /*
  * NUMERIC					A number editor
  * 	Params are:
  * 		type=<type>		Can aerospace type, usually SHORT or FLOAT
  * 		value=id		A canerospace value the will be the current value
  * 		set_value=id		Message to set the value
  * 		precision=num		Optional precision, if ommitted 0 is assumed and an integer value
  * 		digits=num		Number of digits, if precision is included will be <digits>.<precision>
  * 		min_value=num
  * 		max_value=num
  * 		circualar=<true/false>	If provided the editor will swap to min on overflow and max on underflow
  * 		decka_increment=<value>	Normally decka increments the digits, this sets by how much.  defaults to 1
  * 		deckb_increment=<value>	If a precision is set then this sets how much the minor value changes, otherwise allows for
  * 					incrementing the digits.
  * 	example for a frequency:	type=FLOAT,value=nnn,set_value=nnn,precision=3,digits=2,min_value=108.0,max_value=136.75,circular=true,decka_increment=1,deckb_increment=0.25
   */

  class MenuItemEdit : MenuItem
  {
    private ushort _valueId;
    private ushort _setId;
    private bool _integerType;
    private float _minValue;
    private float _maxValue;
    private bool _circular;
    private float _deckaIncrement;
    private float _deckbIncrement;
    private ushort _precision;
    private ushort _digits;

    public MenuItemEdit(LayoutWidget widget, ushort key)
      : base(widget, key)
    {
      if(!Widget.TryRegGetUint16(key, "value-id", out _valueId) ||
      !Widget.TryRegGetUint16(key, "set-id", out _setId) ||
      !Widget.TryRegGetBool(key, "send-short", out _integerType) ||
      !Widget.TryRegGetFloat(key, "min-value", out _minValue) ||
      !Widget.TryRegGetFloat(key, "max-value", out _maxValue) ||
      !Widget.TryRegGetFloat(key, "decka-incr", out _deckaIncrement) ||
      !Widget.TryRegGetFloat(key, "deckb-incr", out _deckbIncrement))
        throw new ApplicationException("Edit item is not well defined");

      if(!Widget.TryRegGetUint16(key, "precision", out _precision))
        Precision = 5;

      if(!Widget.TryRegGetUint16(key, "digits", out _digits))
        Digits = 0;
    }
    /// <summary>
    /// Message ID to send
    /// </summary>
    /// <value></value>
    public ushort ValueId
    {
      get { return _valueId; }
      set { _valueId = value; }
    }
    /// <summary>
    /// Message ID to set the value
    /// </summary>
    /// <value></value>
    public ushort SetId
    {
      get { return _setId; }
      set { _setId = value; }
    }
    /// <summary>
    /// If true this is an integer type, otherwise a floating point
    /// </summary>
    /// <value></value>
    public bool IntegerType
    {
      get { return _integerType; }
      set { _integerType = value; }
    }
    /// <summary>
    /// Minimum value to send
    /// </summary>
    /// <value></value>
    public float MinValue
    {
      get { return _minValue; }
      set { _minValue = value; }
    }
    /// <summary>
    /// Maximum valuue to send
    /// </summary>
    /// <value></value>
    public float MaxValue
    {
      get { return _maxValue; }
      set { _maxValue = value; }
    }
    /// <summary>
    /// Increment to add/subtract on each deck-b signal
    /// </summary>
    /// <value></value>
    public float DeckaIncrement
    {
      get { return _deckaIncrement; }
      set { _deckaIncrement = value; }
    }
    /// <summary>
    /// Increment to add/subtract on each deck-b signal
    /// </summary>
    /// <value></value>
    public float DeckbIncrement
    {
      get { return _deckbIncrement; }
      set { _deckbIncrement = value; }
    }
    /// <summary>
    /// Max digits before wrap
    /// </summary>
    /// <value></value>
    public ushort Precision
    {
      get { return _precision; }
      set { _precision = value; }
    }
    /// <summary>
    /// Floating point digits to wrap at
    /// </summary>
    /// <value></value>
    public ushort Digits
    {
      get { return _digits; }
      set { _digits = value; }
    }
    /// <summary>
    /// If set then the control will overflow to min value or max value when the
    /// value reaches the end
    /// </summary>
    /// <value>True to enable auto-wrap</value>
    public bool Circular
    {
      get { return _circular; }
      set { _circular = value; }
    }

    public override void Paint(Rect area, bool isHighlighted)
    {
      base.Paint(area, isHighlighted);
    }

    public override MenuItemActionResult Evaluate(CanFlyMsg msg)
    {
     /*
      // check the enabler
      DefaultEnableHandler(wnd, item, msg);

      menu_item_edit_t* edit = (menu_item_edit_t*)item;
      // see if we update our value
      if (edit.value_id == get_can_id(msg))
        memcpy(&edit.value, msg, sizeof(CanFlyMsg));
      */
      return MenuItemActionResult.MiaNothing;
    }
  }
}
