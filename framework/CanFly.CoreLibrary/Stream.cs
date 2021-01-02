using System;

namespace CanFly
{
  public abstract class Stream : IDisposable
  {
    private uint _handle;

    protected Stream(uint handle)
    {
      _handle = handle;
    }
    
    public void Dispose()
    {
      OnDispose();
    }

    /// <summary>
    /// Underlying OS handle
    /// </summary>
    internal uint Handle
    {
      get { return _handle; }
    }

    protected abstract void OnDispose();
  }
}