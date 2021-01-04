namespace CanFly
{
  public class Screen : Widget
  {
    private static Screen _screen;
    private Screen(uint hwnd) : base(hwnd)
    {
      // handle all generic methods
      //CanFlyMsgEvent.CanFlyMsgEventPosted += CanFlyMsgEvent_CanFlyMsgEventPosted;
      CanFlyMsgSink.AddEventListener(PhotonID.id_paint, OnPaintEvent);
    }

    /// <summary>
    /// Called when a paint message is to be processed
    /// </summary>
    /// <param name="e"></param>
    private void OnPaintEvent(CanFlyMsg e)
    {
      OnPaint();
    }

    private class WidgetLock { };
    private static WidgetLock widgetLock = new WidgetLock();

    /// <summary>
    /// Return the singleton screen widget
    /// </summary>
    public static Screen Instance
    {
      get
      {
        if (_screen == null)
        {
          lock (widgetLock)
            if (_screen == null)
            {
              _screen = new Screen(Photon.OpenScreen(0, 0));
            }
        }

        return _screen;
      }
    }

    /// <summary>
    /// iterate over the child windows
    /// </summary>
    protected override void OnPaint()
    {
      // work over the child windows and paint them
      /*
  result_t result;
  
  // we assume the widget has painted its canvas, we work over our children
  // in z-order
  uint16_t painting_order = 0;
  uint16_t next_z_order = 0;
  uint16_t max_order = 0;
  handle_t child;

  uint8_t z_order;
  if (failed(result = get_z_order(hwnd, &z_order)))
    return result;

  max_order = z_order;
  painting_order = z_order;

  begin_paint(hwnd);

  do
    {
    for (get_first_child(hwnd, &child); child != 0; get_next_sibling(child, &child))
      {
      get_z_order(child, &z_order);

      if (z_order > painting_order)
        {
        // get the next highest order after our own
        if (z_order < next_z_order || next_z_order == painting_order)
          next_z_order = z_order;

        // figure out what the last one to do is.
        if (z_order > max_order)
          max_order = z_order;
        }

      if (z_order == painting_order)
        send_message(child, msg);         // is ok to paint
      }

    if (painting_order >= max_order)
      break;

    painting_order = next_z_order;        // lowest next paint

    } while (true);

  end_paint(hwnd);
  return s_ok;      // processed
       */
    }
  }
}
