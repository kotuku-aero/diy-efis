using System;

namespace CanFly
{
  public static class ExceptionHelper
  {
    public static int ThrowIfFailed(int result)
    {
      // TODO: this should use the canfly result types and throw the correct exception types
      if (result < 0)
        throw new InvalidOperationException();

      return result;
    }
  }
}
