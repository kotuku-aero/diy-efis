using System;
using System.Threading;

namespace CanFly
{
  /// <summary>
  /// Base for singleton application class.
  /// </summary>
  public abstract class Application
  {
    protected Application()
    {
    }

    /// <summary>
    /// Called by the framework.
    /// </summary>
    /// <param name="screen"></param>
    /// <param name="rootKey"></param>
    public abstract void Run(uint screen, uint rootKey);

    private bool WndProc(uint hwnd, CanFlyMsg msg)
    {
      // call the default
      return false;
    }

    /// <summary>
    /// Places (posts) a message in the message queue associated with the specified
    /// window and returns without waiting for the application to process the message.
    /// </summary>
    /// <param name="hwnd">Window to receive the message
    /// If the Widget is null then the message is posted to the
    /// can-bus.  If the Flags.Loopback is set then the message
    /// is posted to all windows as well.
    /// If the handle is Flags.Broadcast the message is only sent
    /// to windows, not the can bus.
    /// </param>
    /// <param name="msg">Message to be sent</param>
    /// <param name="max_wait">Time to wait to post the message</param>
    void PostMessage(Widget widget, CanFlyMsg msg, uint max_wait = 0)
    {
      msg.PostMessage(widget, max_wait);
    }
    /// <summary>
    /// Sends the specified message to a window or windows. The SendMessage function
    /// calls the window procedure for the specified window and does not return
    /// until the window procedure has processed the message.
    /// </summary>
    /// <param name="hwnd"></param>
    /// <param name="msg"></param>
    void SendMessage(Widget widget, CanFlyMsg msg)
    {
    }
    /// <summary>
    /// Decode a png image from a stream
    /// </summary>
    /// <param name="stream">stream to read from</param>
    /// <returns></returns>
    Canvas CreatePngCanvas(Stream stream)
    {
      return null;
    }
    /// <summary>
    /// Load a png image onto a canvas
    /// </summary>
    /// <param name="canvas">Canvas to render to</param>
    /// <param name="stream">Stream to read from</param>
    /// <param name="pt">Point on canvas to render to</param>
    void LoadPng(Canvas canvas, Stream stream, Point pt)
    {
      
    }
    /// <summary>
    /// Open a previously registered font
    /// </summary>
    /// <param name="name"></param>
    /// <param name="pixels"></param>
    /// <returns></returns>
    Font OpenFont(string name, ushort pixels)
    {
      return null;
    }

    /// <summary>
    /// Load a font into the font cache
    /// </summary>
    /// <param name="stream">Font encoded stream</param>
    void LoadFont(Stream stream)
    {
      
    }
  }
}