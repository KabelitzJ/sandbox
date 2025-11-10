using System;
using Sbx.Managed.Interop;

namespace Sbx.Core
{

  internal static unsafe class InternalCalls
  {
    internal static delegate* unmanaged<Logger.Level, NativeString, void> Log_LogMessage;

    internal static delegate* unmanaged<uint, ReflectionType, void> Behavior_AddComponent;
		internal static delegate* unmanaged<uint, ReflectionType, bool> Behavior_HasComponent;
		// internal static delegate* unmanaged<uint, ReflectionType, bool> Behavior_RemoveComponent;

    internal static delegate* unmanaged<uint, NativeString> Tag_GetTag;
    internal static delegate* unmanaged<uint, NativeString, void> Tag_SetTag;

    internal static delegate* unmanaged<uint, Vector3*, void> Transform_GetPosition;
    internal static delegate* unmanaged<uint, Vector3*, void> Transform_SetPosition;

    internal static delegate* unmanaged<KeyCode, Bool32> Input_IsKeyPressed;
		internal static delegate* unmanaged<KeyCode, Bool32> Input_IsKeyDown;
    internal static delegate* unmanaged<KeyCode, Bool32> Input_IsKeyReleased;

		internal static delegate* unmanaged<MouseButton, bool> Input_IsMouseButtonPressed;
		internal static delegate* unmanaged<MouseButton, bool> Input_IsMouseButtonDown;
    internal static delegate* unmanaged<MouseButton, bool> Input_IsMouseButtonReleased;

  } // class InternalCalls

} // namespace Sbx.Core

