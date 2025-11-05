using System;
using Sbx.Managed.Interop;

namespace Sbx.Core
{

  internal static unsafe class InternalCalls
  {
    internal static delegate* unmanaged<Logger.Level, NativeString, void> Log_LogMessage;
  } // class InternalCalls

} // namespace Sbx.Core

