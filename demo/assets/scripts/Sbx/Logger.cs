using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Sbx {
    
public static class Logger {

  public static void Info(string message) {
    InternalInfo(message);
  }

  [MethodImpl(MethodImplOptions.InternalCall)]
  internal static extern void InternalInfo(string message);

} // class Logger

} // namespace Sbx