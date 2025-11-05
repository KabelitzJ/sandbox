using System;
using Sbx.Managed.Interop;

namespace Sbx.Core
{

  internal static unsafe class InternalCalls
  {
    internal static delegate* unmanaged<Logger.Level, NativeString, void> Log_LogMessage;

    internal static delegate* unmanaged<uint, ReflectionType, void> Behavior_CreateComponent;
		internal static delegate* unmanaged<uint, ReflectionType, bool> Behavior_HasComponent;
		// internal static delegate* unmanaged<uint, ReflectionType, bool> Behavior_RemoveComponent;

    internal static delegate* unmanaged<uint, NativeString> Tag_GetTag;
    internal static delegate* unmanaged<uint, NativeString, void> Tag_SetTag;

  } // class InternalCalls

} // namespace Sbx.Core

