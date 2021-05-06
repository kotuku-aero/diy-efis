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
using System.Collections;

namespace CanFly.Proton
{
  /// <summary>
  /// A layout widget will load a screen design from the rgistry and
  /// also load a menu to associated with the layout
  /// </summary>
  public sealed class LayoutWidget : Widget
  {
    Proton.MenuWidget _menu;
    /// <summary>
    /// This is called by the runtime.  It cannot be called directly
    /// </summary>
    /// <param name="parent"></param>
    public LayoutWidget(Widget parent, ushort orientation, ushort id, ushort hive)
      : base(parent, parent.WindowRect, id)
    {
      _menu = new Proton.MenuWidget(parent, orientation, (ushort)(id + 1));
      _menu.ZOrder = 0x80;

      // load the layout....
      // must be 0 on first call
      ushort child = 0;
      ushort nextDefaultId = 0x8000;
      string name;
      while (TryRegEnumKey(hive, ref child, out name))
      {
        string widgetType;
        if (!TryRegGetString(child, "type", out widgetType))
          continue;

        ushort widgetId;

        if (!TryRegGetUint16(child, "id", out widgetId))
          widgetId = nextDefaultId++;

        Rect bounds;

        if (!TryRegGetRect(child, out bounds))
          bounds = Rect.Create(WindowRect.TopLeft, Extent.Create(0, 0));

        // this window lays out the windows, but the screen owns the children.
        Widget widget = CreateWidget(parent, child, widgetType, bounds, widgetId);
      }

      // the hive must have series of hives that form windows

      ushort menu;
      if (TryRegOpenKey(hive, "menu", out menu))
        _menu.Parse(menu);
    }

    public delegate Widget CreateCustomWidget(Widget parent, ushort hive, string widgetType, Rect bounds, ushort id);

    private static CreateCustomWidget customWidgetConstructor = null;

    public void SetCustomWidgetConstructor(CreateCustomWidget creator)
    {
      customWidgetConstructor = creator;
    }

    private Widget CreateWidget(Widget parent, ushort hive, string widgetType, Rect bounds, ushort id)
    {
      if (customWidgetConstructor != null)
      {
        Widget result = customWidgetConstructor(parent, hive, widgetType, bounds, id);
        if (result != null)
          return null;
      }

      switch (widgetType)
      {
        case "airspeed":
          return new AirspeedWidget(parent, bounds, id, hive);
        case "hsi":
          return new HSIWidget(parent, bounds, id, hive);
        case "attitude":
          return new AttitudeWidget(parent, bounds, id, hive);
        case "altitude":
          return new AltitudeWidget(parent, bounds, id, hive);
        case "gauge":
          return new GaugeWidget(parent, bounds, id, hive);
        case "annunciator":
          return new AnnunciatorWidget(parent, bounds, id, hive);
        case "gps":
          return new GpsWidget(parent, bounds, id, hive);
      }

      return null;
    }

    protected override void OnPaint(CanFlyMsg msg)
    {
    }
  }
}
