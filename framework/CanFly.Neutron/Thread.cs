using System;

namespace CanFly
{
  public abstract class Thread : IDisposable
  {
    private uint _handle;

    private void ThreadProc()
    {
      Run();
    }

    protected Thread(byte priority, string name)
    {
      // create the thread
      CanFly.Syscall.CreateThread(priority, name, ThreadProc, this, out _handle);
    }

    public void Dispose()
    {
      if (_handle != 0)
      {
        Terminate();
        _handle = 0;
      }
    }

    protected abstract void Run();
    public abstract void Terminate();
  }
}
