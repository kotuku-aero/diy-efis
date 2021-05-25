//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System;
using System.Diagnostics;
using System.Threading;

namespace CanFly.Tools.Utilities
{
  internal static class DebuggerHelper
  {
    public static void WaitForDebuggerIfEnabled(string varName, int timeoutSeconds = 30)
    {
      // this wait should be only available on debug build
      // to prevent unwanted wait on VS in machines where the variable is present
#if DEBUG
      TimeSpan timeoutToWaitForDebugToAttach = TimeSpan.FromSeconds(timeoutSeconds);

      var isToEnablePauseForDebug = Environment.GetEnvironmentVariable(varName, EnvironmentVariableTarget.User);

      if (!string.IsNullOrEmpty(isToEnablePauseForDebug)
          && isToEnablePauseForDebug.Equals("1", StringComparison.Ordinal))
      {
        // output helper messsage to console, hopefully to be read by a human
        Console.WriteLine($".NET CanFly Metadata Processor msbuild task debugging is enabled. Waiting {timeoutSeconds} seconds for debugger to attach...");

        var currentProcessId = Process.GetCurrentProcess().Id;
        var currentProcessName = Process.GetProcessById(currentProcessId).ProcessName;
        Console.WriteLine(
            string.Format("Process Id: {0}, Name: {1}", currentProcessId, currentProcessName)
            );

        // wait N seconds for debugger to attach
        while (!Debugger.IsAttached
            && timeoutToWaitForDebugToAttach.TotalSeconds > 0)
        {
          Thread.Sleep(1000);
          timeoutToWaitForDebugToAttach -= TimeSpan.FromSeconds(1);
        }

        // stop debug
        Debugger.Break();
      }
#endif
    }
  }
}
