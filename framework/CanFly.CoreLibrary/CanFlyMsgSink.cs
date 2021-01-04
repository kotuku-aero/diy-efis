using System.Collections;
using System.Runtime.CompilerServices;

namespace CanFly
{
  public class CanFlyMsgSink : CanFlyEventDispatcher
  {
    private static CanFlyMsgSink s_eventSink = null;
    private static ArrayList s_eventInfoTable = null;

    private class EventInfo
    {
      public EventInfo()
      {
        EventListener = null;
      }

      public CanFlyMsgPostedEventHandler EventListener;
      // the canfly msg ID of this event
      public ushort EventID;
    }

    static CanFlyMsgSink()
    {
      s_eventSink = new CanFlyMsgSink();
      s_eventSink.OnMessage += new CanFlyEventHandler(s_eventSink.EventDispatchCallback);
    }

    // Pass the name to the base so it connects to driver
    private CanFlyMsgSink()
    {
    }

    private void ProcessEvent(EventInfo eventInfo, CanFlyMsg ev)
    {
      if (eventInfo == null)
        return;

      if (eventInfo.EventListener != null)
        eventInfo.EventListener(ev);
    }

    private void EventDispatchCallback(ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7)
    {
      EventDispatchCallback(new CanFlyMsg(flags, b0, b1, b2, b3, b4, b5, b6, b7));
    }

    private void EventDispatchCallback(CanFlyMsg ev)
    {
      EventInfo eventInfo = GetEventInfo(ev.Id);

      ProcessEvent(eventInfo, ev);
    }

    /// <summary>
    /// Adds a listener for particular types of events.
    /// </summary>
    /// <param name="message_id">Selects the message id.</param>
    /// <param name="eventListener">Specifies the event listener.</param>
    [MethodImpl(MethodImplOptions.Synchronized)]
    public static void AddEventListener(ushort message_id, CanFlyMsgPostedEventHandler eventListener)
    {
      EventInfo eventInfo = GetEventInfo(message_id);
      eventInfo.EventListener += eventListener;
    }

    /// <summary>
    /// Removes an event listener.
    /// </summary>
    /// <param name="ushort">Selects an event EventID.</param>
    /// <param name="eventListener">Specifies the event listener to be removed.</param>
    [MethodImpl(MethodImplOptions.Synchronized)]
    public static void RemoveEventListener(ushort message_id, CanFlyMsgPostedEventHandler eventListener)
    {
      EventInfo eventInfo = GetEventInfo(message_id);

      eventInfo.EventListener -= eventListener;
    }

    /// <summary>
    /// Processes event information.
    /// </summary>
    /// <param name="EventID">The event EventID.</param>
    /// <param name="subEventID">The event subEventID.</param>
    /// <param name="data1">Data related to the event.</param>
    /// <param name="data2">Data related to the event.</param>
    [MethodImpl(MethodImplOptions.Synchronized)]
    public static void PostManagedEvent(CanFlyMsg msg)
    {
      if (s_eventSink != null)
      {
        s_eventSink.EventDispatchCallback(msg);
      }
    }
    /// <summary>
    /// Lookup the handler for an event
    /// </summary>
    /// <param name="EventID"></param>
    /// <returns></returns>
    private static EventInfo GetEventInfo(ushort EventID)
    {
      // because of lack of constructors in native code we need to check if this field has been initialized
      if (s_eventInfoTable == null)
        s_eventInfoTable = new ArrayList();

      var myEvent = FindEvent(EventID);

      if (myEvent != null)
      {
        // event already registered
        return myEvent;
      }
      else
      {
        // create a new EventInfo...
        EventInfo eventInfo = new EventInfo()
        {
          EventID = EventID
        };

        // ... and add it to the events table
        s_eventInfoTable.Add(eventInfo);

        return eventInfo;
      }
    }

    private static EventInfo FindEvent(ushort eventID)
    {
      EventInfo genericEventHandler = null;
      for (int i = 0; i < s_eventInfoTable.Count; i++)
      {
        EventInfo theHandler = (EventInfo)s_eventInfoTable[i];
        ushort idHandled = theHandler.EventID;

        if (idHandled == eventID)
          return theHandler;

        if (idHandled == 0)
          genericEventHandler = theHandler;
      }

      return genericEventHandler;
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private extern void EventConfig();
  }
}
